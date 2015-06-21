#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Generative Frame Test");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(50);
    ofSetEscapeQuitsApp(false);
    
    
    frame.setup();
    
    frameRate = 24;
    
    index = 0;
    frame.generate(index);
    incrementAt = ofGetElapsedTimef()+(1/frameRate);
}


//--------------------------------------------------------------
void ofApp::update(){

    float now = ofGetElapsedTimef();
    
    if(now > incrementAt) {
        index++;
        frame.generate(index);
        
        incrementAt = ofGetElapsedTimef()+(1/frameRate);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    frame.draw(10, 30);
    
    
    stringstream info;
    info << "frameRate = " << ofGetFrameRate();
    ofDrawBitmapStringHighlight(info.str(), 10, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==OF_KEY_RETURN) {
        index++;
        frame.generate(index);
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
