#include "ofApp.h"

//--------------------------------------------------------------
string ofSystemCall(string command) {
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Video Player");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofBackground(0);
    ofEnableAlphaBlending();
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("ofDirectory", OF_LOG_SILENT);
    ofSetDataPathRoot("../Resources/data/");
    
    video = NULL;
    bDebug = false;
    nextVideoCheck=0;
    font.loadFont("HelveticaNeueLTCom-Hv.ttf", 48);
    
    desktop = Poco::Path::home();
    desktop.pushDirectory("Desktop");
    
    framesPath = Poco::Path::home();
    framesPath.pushDirectory("Dropbox");
    framesPath.pushDirectory("LE Shared");
    framesPath.pushDirectory("_PhotosSmall");
    
    
    ofDirectory dir;
    dir.allowExt("mp4");
    dir.sort();
    dir.listDir(desktop.toString());
    
    startThread();
}

//--------------------------------------------------------------
void ofApp::threadedFunction()
{
    ofDirectory dir;
    dir.allowExt("jpg");
    int nFrames = -1;
    
    while(isThreadRunning()) {
        
        dir.listDir( framesPath.toString() );
        ofLogNotice() << "FOUND " << dir.size() << " FRAMES";
        
        if(dir.size() != nFrames) {
            if(lock()) {
                bMakingVideo = true;
                unlock();
            }
            
            makeVideo();
            nFrames = dir.size();
            cleanup();
            
            if(lock()) {
                bMakingVideo = false;
                unlock();
            }
        } else {
            ofLogNotice() << "NEW VIDEO NOT NEEDED";
        }
        sleep(1000); // Sleep for 1 second.
    }
}

//--------------------------------------------------------------
void ofApp::makeVideo() {
    string basename = ofGetTimestampString("LE-%m-%d-%H-%M-%S-%i");
    
    stringstream cmd;
    cmd << "/usr/local/bin/ffmpeg -y -f image2 ";
    cmd << "-pattern_type glob -i \"" << framesPath.toString() << "*.jpg\" ";
    cmd << "-s 1920x1280 -c:v libx264 -an -q:v 2 -pix_fmt yuv420p ";
    cmd << "-filter:v \"setpts=2.0*PTS\" ";
    cmd << desktop.toString() << basename << ".mp4";
    
    ofLogNotice() << cmd.str();
    ofSystemCall( cmd.str() );
}

//--------------------------------------------------------------
void ofApp::cleanup() {
    ofDirectory dir;
    dir.allowExt("mp4");
    dir.listDir(desktop.toString());
    if(dir.size() < 3) return;
    
    vector<ofFile> toRemove;
    for(int i=0; i<dir.size(); i++) {
        string name = dir.getName(i);
        if( name.find("LE") == 0 && i < dir.size()-3) {
            toRemove.push_back( dir.getFile(i) );
        }
    }
    
    for(int i=0; i<toRemove.size(); i++) {
        toRemove[i].remove();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    bool bReloadNeeded = video==NULL || now > nextVideoCheck;
    
    lock();
    if( bReloadNeeded && !bMakingVideo ) {

        ofDirectory dir;
        dir.allowExt("mp4");
        dir.listDir(desktop.toString());
        
        if(dir.size()>0) {
            
            dir.sort();
            string newsetVideo = dir.getPath(dir.size()-1);
            
            if(currentVideo != newsetVideo) {
                ofLogNotice() << "RELOADING VIDEO";
                
                if(video) delete video;
                video = new ofVideoPlayer();
                video->loadMovie(newsetVideo);
                video->setLoopState(OF_LOOP_NONE);
                video->play();
                
                float ratio =  ofGetWidth() / video->getWidth();
                bounds.height = video->getHeight() * ratio;
                bounds.width = video->getWidth() * ratio;
                bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
                bounds.y = 0;
                currentVideo = newsetVideo;
            }
        }
        
        nextVideoCheck = now + 1;
    }
    unlock();
    
    if(video) {
        video->update();
        if(video->getIsMovieDone()) video->play();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    
    string message = "loading";
    ofRectangle bb = font.getStringBoundingBox(message, 0, 0);
    float x = (ofGetWidth()/2.0) - (bb.getWidth()/2.0);
    float y = (ofGetHeight()/2.0) +100;
    font.drawString(message, x, y);
    
    if(video) video->draw(bounds);
    
    ofSetColor(ofColor::black);
    ofRect(0, ofGetHeight()-20, ofGetWidth(), 20);
    
    if(bDebug) {
        stringstream ss;
        ss << "framerate " << ofGetFrameRate() << endl;
        if(video!=NULL) {
            ss << "duration " << video->getDuration() << endl;
            //ss << "frames " << video->getTotalNumFrames() << endl;
        }
        ofDrawBitmapStringHighlight(ss.str(), 10, 20);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='f') {
        ofToggleFullscreen();
    }
    if(key=='d') {
        bDebug = !bDebug;
    }
    if(key=='p') {
        if(video) video->setPaused(!video->isPaused());
    }
    if(key==OF_KEY_RIGHT) {
        if(video) video->nextFrame();
    }
    if(key==OF_KEY_LEFT) {
        if(video) video->previousFrame();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    if(video) {
        float ratio =  ofGetWidth() / video->getWidth();
        bounds.height = video->getHeight() * ratio;
        bounds.width = video->getWidth() * ratio;
        bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
        bounds.y = 0;
    }
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
