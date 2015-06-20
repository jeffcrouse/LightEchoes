#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    channel = 1;
    dmx.connect(0);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    dmx.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapStringHighlight("channel = "+ofToString(channel), 10, 20);
    ofDrawBitmapStringHighlight("value = "+ofToString(value), 10, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='1') {
        value = 512;
        dmx.setLevel(channel, value);
    }
    if(key=='2') {
        value = 0;
        dmx.setLevel(channel, value);
    }
    if(key=='+') {
        channel++;
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
