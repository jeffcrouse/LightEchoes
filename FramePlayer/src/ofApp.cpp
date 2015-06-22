#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Frame Player");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("ofDirectory", OF_LOG_SILENT);
    ofSetDataPathRoot("../Resources/data/");
    
    bDebug = false;
    font.loadFont("verdana.ttf", 24);
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory("LightEchoesSmall");
    contentPath = path.toString();
    ofDirectory::createDirectory(contentPath, false, true);
    
    
    path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.setFileName("sync.lock");
    lockfile = path.toString();
    ofLogNotice() << "lockfile: " << lockfile;
    
    
//    buffer = ofBufferFromFile("mountCommand.txt");
//    mountCommand = buffer.getFirstLine();
  
    nextContentCheck=5;
    state = STATE_LOOKING_FOR_DIR;
}



//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    
    if(state==STATE_LOOKING_FOR_DIR && now > nextContentCheck) {
        loadContent();
        nextContentCheck = now + 5;
    }
    
    if(state==STATE_RUNNING && now > nextFrameAt) {
        it++;
        
        if(it == frames.end()) {
            it = frames.begin();
            checkForNewFrame();
        } else {
            nextFrameAt = now + (1/PLAYBACK_FRAMERATE);
        }
    }
}

//--------------------------------------------------------------
string ofSystemCall(string command)
{
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
void ofApp::loadContent() {
    
    if(!ofFile::doesFileExist(contentPath)) return;
    
    dir.allowExt("jpg");
    dir.listDir(contentPath);
    dir.sort();
    
    int start = max(0, (int)dir.size()-NUM_FRAMES);
    for(int i=start; i<dir.size(); i++) {
        string path = dir.getPath(i);
        loader.clear();
        ofLogNotice() << "Loading " << frames.size() << ": " << path;
        if(loader.loadImage( path )) {
            frames.push_back( loader.getTextureReference() );
            newest = i; //dir.getFile(i).getPocoFile().created();
        }
    }
    
    if(frames.size()>0) {
        it = frames.begin();
        nextFrameAt = ofGetElapsedTimef();
        state = STATE_RUNNING;
    }
}

//--------------------------------------------------------------
void ofApp::checkForNewFrame(){
    
    if(!ofFile::doesFileExist(contentPath)) {
        frames.clear();
        nextContentCheck = ofGetElapsedTimef() + 5;
        state = STATE_LOOKING_FOR_DIR;
        return;
    }
    
    if(ofFile::doesFileExist(lockfile)){
        ofLogNotice() << "Waiting for sync to complete.";
        return;
    };
    
    dir.allowExt("jpg");
    dir.listDir(contentPath);
    dir.sort();
    
    // If any frames are newer than newest, do onNewFrame
    // and update the newest file date
    
    int start = max(0, (int)dir.size()-NUM_FRAMES);
    ofLogNotice() << "checking frames " << start << " to " << dir.size();
    
    for(int i= start; i<dir.size(); i++) {
        //Poco::Timestamp created = dir.getFile(i).getPocoFile().created();
        string path = dir.getPath(i);
        if(i > newest && onNewFrame( path )) {
            newest = i;
        }
    }
}

//--------------------------------------------------------------
bool ofApp::onNewFrame(string path) {
    loader.clear();
    
    ofLogNotice() << "Loading: " << path;
    bool loaded = loader.loadImage( path );
    
    if(loaded) {
        ofLogNotice() << "LOADING SUCCEEDED!";
        frames.push_back( loader.getTextureReference() );
        
        ofLogNotice() << "ADDED TEXTURE";
        
        if(frames.size() > NUM_FRAMES) {
            ofLogNotice() << "REMOVING OLD TEXTURE";
            frames.erase( frames.begin() );
        }
        
        ofLogNotice() << "PAUSING ON FRAME " << frames.size()-1;
        it = frames.begin() + (frames.size()-1);
        
        nextFrameAt = ofGetElapsedTimef() + PAUSE_ON_NEW_FRAME;
    } else {
        ofLogNotice() << "LOADING FAILED!";
    }
    
    ofLogNotice() << "DONE";
    return loaded;
}


//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(ofColor::white);
    
    if(state==STATE_LOOKING_FOR_DIR) {
        
        stringstream msg;
        msg << "Looking for directory" << endl;
        msg << contentPath << endl;
        int time = (nextContentCheck - ofGetElapsedTimef())+1;
        msg << "next check in " << time;
        ofRectangle bb = font.getStringBoundingBox(msg.str(), 0, 0);
        float x = (ofGetWidth()/2.0) - (bb.getWidth()/2.0);
        float y = (ofGetHeight()/2.0);
        
        
        font.drawString(msg.str(), x, y);
    }
    
    if(state==STATE_RUNNING) {
    
        if(it != frames.end()) {
            
            ofTexture& frame = *it;
            
            float ratio =  ofGetHeight() / frame.getHeight();
            bounds.height = frame.getHeight() * ratio;
            bounds.width = frame.getWidth() * ratio;
            bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
            bounds.y = 0;
            
            frame.draw(bounds);
        }
    }
    
    if(bDebug) {
        stringstream ss;
        ss << "framerate " << ofGetFrameRate() << endl;
        ss << "contentPath " << contentPath << endl;
        ss << "NUM_FRAMES " << NUM_FRAMES << endl;
        ss << "PLAYBACK_FRAMERATE " << PLAYBACK_FRAMERATE << endl;
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
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
