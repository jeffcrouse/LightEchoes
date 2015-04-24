#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    bIsRecordingMovie = false;
    camera.setup();
}

//--------------------------------------------------------------
void ofApp::exit() {
    camera.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    camera.update();
    if(camera.isFrameNew()) {
        // process the live view with camera.getLivePixels()
    }
    
    if(camera.isPhotoNew()) {
        // process the photo with camera.getPhotoPixels()
        // or just save the photo to disk (jpg only):
        string path = ofToDataPath(ofToString(ofGetFrameNum()) + ".jpg");
        ofLogNotice() << path;
        camera.savePhoto(path);
    }
    
    if(camera.isMovieNew()) {
        string path = ofToDataPath(ofToString(ofGetFrameNum()) + ".mov");
        ofLogNotice() << path;
        camera.savePhoto(ofToString(ofGetFrameNum()) + ".mov");
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    camera.draw(0, 0);
    // camera.drawPhoto(0, 0, 432, 288);
    
    if(camera.isLiveDataReady()) {
        stringstream status;
        status << camera.getWidth() << "x" << camera.getHeight() << " @ " <<
        (int) ofGetFrameRate() << " app-fps / " <<
        (int) camera.getFrameRate() << " cam-fps / " <<
        (camera.getBandwidth() / (1<<20)) << " MiB/s";
        ofDrawBitmapString(status.str(), 10, 20);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        camera.takePhoto();
    }
    if(key == 's') {
        camera.setup();
    }
    if(key == 'c') {
        camera.close();
    }
    else if(key == 'v') {
        bIsRecordingMovie ^= true;
        if (bIsRecordingMovie) {
            camera.beginMovieRecording();
        } else {
            camera.endMovieRecording();
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
