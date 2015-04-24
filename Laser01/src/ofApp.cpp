#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    image.loadImage("images/shapes.jpg");
    
    etherdream.setup();
    etherdream.setPPS(30000);
    //etherdream.setWaitBeforeSend(false);
    etherdream.setPoints(points);
    
    for(int i=0; i<360; i++) {
        ofxIlda::Point p;
        
        p.x = 0.5 + cos(ofDegToRad(i)) * 0.5;
        p.y = 0.5 + sin(ofDegToRad(i)) * 0.5;
        
        
        
        points.push_back(p);
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    ofTranslate(250, 50);
    image.draw(0, 0);
    ofPopMatrix();
    
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
