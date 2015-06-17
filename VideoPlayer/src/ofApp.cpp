#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Video Player");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetDataPathRoot("../Resources/data/");
    
    bDebug = false;
    
    ofBuffer buffer = ofBufferFromFile("videopath.txt");
    videoFolder = buffer.getFirstLine();
    
    nextVideo();
}

//--------------------------------------------------------------
void ofApp::nextVideo() {
    dir.allowExt("mp4");
    dir.allowExt("mov");
    dir.sort();
    dir.listDir(videoFolder);
    
    if(dir.size()==0) return;
    
    // get the last video

    currentVideo = dir.getPath(dir.size()-1);
    video.loadMovie(currentVideo);
    video.setLoopState(OF_LOOP_NONE);
    
    
    float ratio =  ofGetHeight() / video.getHeight();
    bounds.height = video.getHeight() * ratio;
    bounds.width = video.getWidth() * ratio;
    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
    bounds.y = 0;
    
    video.play();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(video.isLoaded()) {
        video.update();
    }
    
    if(video.getIsMovieDone()) {
        nextVideo();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(video.isLoaded()) {
        video.draw(bounds);
    } else {
        ofDrawBitmapStringHighlight("NO VIDEO", 10, 30, ofColor::red, ofColor::white);
    }
    
    if(bDebug) {
        stringstream info;
        info << "videoFolder = " << videoFolder << endl;
        if(video.isLoaded()) {
            info << "video.getPosition() " << video.getPosition() << endl;
        }
        ofDrawBitmapStringHighlight(info.str(), 10, ofGetHeight()-20);
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
    if(key==OF_KEY_TAB) {
        nextVideo();
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
    float ratio =  ofGetHeight() / video.getHeight();
    bounds.height = video.getHeight() * ratio;
    bounds.width = video.getWidth() * ratio;
    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
    bounds.y = 0;
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
