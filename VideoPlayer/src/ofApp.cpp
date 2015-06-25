#include "ofApp.h"

#define PAUSE_ON_NEW_FRAME 60
#define PHOTOS_DIR "_PhotosSmall"
#define VIDEO_FILE "video.mp4"
#define LOCKFILE "video.lock"


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
    bDebug=false;
    nextFramecheck = ofGetElapsedTimef()+2;
    frameStart = -1;
    frameAlpha = 0;
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    dropboxPath = path.toString();
    photosDir = dropboxPath + PHOTOS_DIR;
    
    
    path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.setFileName("LightEchoes.mp4");
    videoFile.open(path.toString());
    
    
    path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.setFileName("video.lock");
    lockFile.open(path.toString());
    
    
    
    // Get the number of photos in the photo directory
    dir.allowExt("jpg");
    dir.listDir(photosDir);
    numPhotos = dir.size();
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    
    
    
    if(now > nextFramecheck) {
        ofLogNotice() << "CHECKING FOR NEW FRAMES";
        dir.allowExt("jpg");
        dir.listDir(photosDir);
        
        if (dir.size() > numPhotos) {
            ofLogNotice() << "NEW FRAME FOUND ";
            
            dir.sort();
            string path = dir.getPath(dir.size()-1);
            frame.loadImage(path);
            
            frameStart = now;
            frameEnd = frameStart + PAUSE_ON_NEW_FRAME;
            numPhotos = dir.size();
        }
        
        nextFramecheck = now + 2;
    }

    if( (video==NULL|| videoFile.getPocoFile().getLastModified()>videoModified) && !lockFile.exists()) {

        ofLogNotice() << "RELOADING VIDEO";
        
        
        if(video) delete video;
        video = new ofVideoPlayer();
        video->loadMovie(videoFile.getAbsolutePath());
        video->setLoopState(OF_LOOP_NORMAL);
        video->play();
        
        float ratio =  ofGetHeight() / video->getHeight();
        bounds.height = video->getHeight() * ratio;
        bounds.width = video->getWidth() * ratio;
        bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
        bounds.y = 0;
        
        
        videoModified =  videoFile.getPocoFile().getLastModified();
    }
    
    if(frameStart != -1) {
        
        float theta = ofMap(now, frameStart, frameEnd, 0, PI);
        float fade = ofMap(sin(theta), 0, 1, 0, 2048);
        
        frameAlpha = ofClamp(fade, 0, 255);
        
        if(now > frameEnd) {
            frameAlpha = 0;
            frameStart = -1;
        }
    }
    
    if(video) video->update();
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white, 255-frameAlpha);
    if(video) video->draw(bounds);
    
    if(frameAlpha > 0) {
        ofSetColor(ofColor::white, frameAlpha);
        frame.draw(bounds);
    }
    
    if(bDebug) {
        stringstream ss;
        ss << "framerate " << ofGetFrameRate() << endl;
        ss << "dropboxPath " << dropboxPath << endl;
        ss << "frameStart " << frameStart << endl;
        ss << "frameEnd " << frameEnd << endl;
        ss << "frameAlpha " << frameAlpha << endl;
        ss << "numPhotos " << numPhotos << endl;
        ss << "PAUSE_ON_NEW_FRAME " << PAUSE_ON_NEW_FRAME;
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
        float ratio =  ofGetHeight() / video->getHeight();
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
