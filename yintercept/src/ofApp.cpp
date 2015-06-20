#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    ofSetLineWidth(3);
    
    line.p1.set(100, 50);
    line.p2.set(600, 500);
}


double yIntercept(LINE& line, double yintercept)
{
    float dx = line.p2.x - line.p1.x;
    float dy = line.p2.y - line.p1.y;
    if(dy==0) return 0;
    
    float deltay = yintercept - line.p2.y;
    return  line.p2.x + (dx/dy) * deltay;
}

//--------------------------------------------------------------
void ofApp::update(){
    //ofLogNotice() << mouseY;
    if(ofInRange(mouseY, line.p1.y, line.p2.y)) {
        intercept.y = mouseY;
        intercept.x = yIntercept(line, mouseY);
    } else {
        intercept.set(-100, -100);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofLine(line.p1, line.p2);
    ofLine(0, mouseY, ofGetWidth(), mouseY);
    ofCircle(intercept, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
