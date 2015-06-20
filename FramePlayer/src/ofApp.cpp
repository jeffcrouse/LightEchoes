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
    
    ofBuffer buffer = ofBufferFromFile("framespath.txt");
    videoFolder = buffer.getFirstLine();
    
    dir.allowExt("jpg");
    dir.listDir(videoFolder);
    dir.sort();
    
    for(int i=max(0, (int)dir.size()-NUM_FRAMES); i<dir.size(); i++) {
        
        loader.loadImage( dir.getPath(i) );
        frames.push_back( loader.getTextureReference() );
        newest = dir.getFile(i).getPocoFile().created();
    }
    
    it = frames.begin();
    nextFrameAt=0;
}



//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    if (now > nextFrameAt) {
        it++;
        
        if(it == frames.end()) {
            it = frames.begin();
            checkForNewFrame();
        } else {
            nextFrameAt = now + (1/FRAMERATE);
        }
    }
}


//--------------------------------------------------------------
void ofApp::checkForNewFrame(){
    dir.allowExt("jpg");
    dir.listDir(videoFolder);
    dir.sort();
    
    for(int i= max(0, (int)dir.size()-NUM_FRAMES); i<dir.size(); i++) {
        Poco::Timestamp created = dir.getFile(i).getPocoFile().created();
        if(created > newest) {
            onNewFrame( dir.getPath(i) );
            newest = created;
        }
    }
}

//--------------------------------------------------------------
void ofApp::onNewFrame(string path) {
    
    loader.loadImage( path );
    frames.push_back( loader.getTextureReference() );
    loader.clear();
    
    it = frames.begin()+ (frames.size()-1);
    
    
    if(frames.size() > NUM_FRAMES) {
        frames.erase( frames.begin() );
    }
    
    nextFrameAt = ofGetElapsedTimef() + 10;
}


//--------------------------------------------------------------
void ofApp::draw(){

    if(it == frames.end()) {
        
        
    } else {
        ofTexture& frame = *it;
        
        float ratio =  ofGetHeight() / frame.getHeight();
        bounds.height = frame.getHeight() * ratio;
        bounds.width = frame.getWidth() * ratio;
        bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
        bounds.y = 0;
        
        frame.draw(bounds);
    }
    
    stringstream ss;
    ss << ofGetFrameRate();
    ofDrawBitmapStringHighlight(ss.str(), 10, 20);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='f') {
        ofToggleFullscreen();
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

    float ratio =  ofGetHeight() / it->getHeight();
    bounds.height = it->getHeight() * ratio;
    bounds.width = it->getWidth() * ratio;
    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
    bounds.y = 0;

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
