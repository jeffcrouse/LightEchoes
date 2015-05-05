#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    bIsRecordingMovie = false;
    
    camera.setup();

}

//--------------------------------------------------------------
void ofApp::exit()
{
    camera.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    camera.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    camera.draw(10, 10, 640, 480);

    if(camera.isPhotoNew()) {
        string path = ofToDataPath(ofToString(ofGetFrameNum()) + ".jpg");
        ofLogNotice() << path;
        camera.savePhoto(path);
    }
    
    if(camera.isMovieNew()) {
        string path = ofToDataPath(ofToString(ofGetFrameNum()) + ".mov");
        ofLogNotice() << path;
        camera.savePhoto(path);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == ' ') {
        bIsRecordingMovie = !bIsRecordingMovie;
        if (bIsRecordingMovie) {
            ofLogNotice() << "Begin Recording";
            camera.beginMovieRecording();
        } else {
            ofLogNotice() << "End Recording";
            camera.endMovieRecording();
        }
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
