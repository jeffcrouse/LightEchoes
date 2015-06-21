#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    harp[0].loadSound("LE.Elements_0619.harp_A.aif");
    harp[1].loadSound("LE.Elements_0619.harp_B.aif");
    harp[2].loadSound("LE.Elements_0619.harp_D.aif");
    harp[3].loadSound("LE.Elements_0619.harp_E.aif");
    harp[4].loadSound("LE.Elements_0619.harp_G.aif");
    harp[5].loadSound("LE.Elements_0619.harp_A.aif");
    
    ofFmodSelectDriver(0);
    ofFmodSetNumOutputs(6);
    
    ofLogNotice() << "outputs = " << ofFmodGetNumOutputs();
}

//--------------------------------------------------------------
void ofApp::update(){
    ofFmodSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key>='1' && key <= '6') {
        int n = key-49;
        ofLogNotice() << "speaker " << n;
        harp[n].playTo(n);
    }
    
    if(key=='f') {
        harp[0].playTo(0, 1);
    }
    if(key=='m') {
        harp[1].playTo(2, 3);
    }
    if(key=='b') {
        harp[2].playTo(4, 5);
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
