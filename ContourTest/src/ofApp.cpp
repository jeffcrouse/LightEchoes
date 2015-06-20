#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    pct = 0;
    
    for(int y = 20; y < ofGetHeight()-20; y+=20) {
        left.addVertex(100, y);
        right.addVertex(500, y);
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    pct += ofGetLastFrameTime() / 5.0;
    if(pct > 1) {
        pct = 0;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    left.draw();
    right.draw();

    ofLine(0, mouseY, ofGetWidth(), mouseY);
    
    stringstream ss;
    ss << "width = " << width;
    ofDrawBitmapStringHighlight(ss.str(), 10, 20);
    
    
    
    
    
    ofSetColor(ofColor::red);
    ofVec3f p1 = right.getClosestPointY(ofPoint(0, mouseY));
    ofCircle(p1, 10);
 
    
    ofVec3f p2 = left.getClosestPointY(ofPoint(0, mouseY));
    ofCircle(p2, 10);
    
    ofLine(p1, p2);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    left.onMouseMoved(x, y);
    right.onMouseMoved(x, y);

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    left.onMouseDragged(x, y);
    right.onMouseDragged(x, y);
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
