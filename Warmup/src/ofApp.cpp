#include "ofApp.h"

#define TRACK_TIME 195
#define DMX_CHANNEL_MOTOR_RELEASE 3
#define DMX_CHANNEL_MOTOR_RETURN 1

//--------------------------------------------------------------
string toHMS(int time) {
    int minutes = time / 60;
    int seconds = time - (minutes*60);
    stringstream ss;
    ss << std::setfill('0') << std::setw(1) << minutes;
    ss << ":";
    ss << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("Warmup");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(50);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetDataPathRoot("../Resources/data/");
    font.loadFont("HelveticaNeueLTCom-Hv.ttf", 60);
    dmx.connect(0, 64); // or use a number
    bForward=true;
    changeDirectionAt = ofGetElapsedTimef()+TRACK_TIME;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(bForward) {
        dmx.setLevel(DMX_CHANNEL_MOTOR_RETURN, 0);
        dmx.setLevel(DMX_CHANNEL_MOTOR_RELEASE, 255);
    } else {
        dmx.setLevel(DMX_CHANNEL_MOTOR_RETURN, 255);
        dmx.setLevel(DMX_CHANNEL_MOTOR_RELEASE, 0);
    }
    
    if(ofGetElapsedTimef() > changeDirectionAt) {
        bForward = !bForward;
        changeDirectionAt = ofGetElapsedTimef() +TRACK_TIME;
    }
    dmx.update();
}

//--------------------------------------------------------------
void ofApp::drawCentered(string msg, int y) {
    ofRectangle bb = font.getStringBoundingBox(msg, 0, 0);
    float _x = (ofGetWidth()/2.0) - (bb.getWidth()/2.0);
    //float _y = (ofGetHeight()/2.0) - (bb.getHeight()/2.0);
    font.drawString(msg, _x, y);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    
    stringstream message;
    message << "WARMUP" << endl;
    message << "direction: ";
    message << (bForward ? "forward" : "back") << endl;
    message << toHMS(changeDirectionAt-ofGetElapsedTimef());
    
    drawCentered(message.str(), 100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==OF_KEY_RETURN) bForward=!bForward;
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
