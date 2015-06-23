#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Generative Frame Test");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(50);
    ofSetEscapeQuitsApp(false);
    
    bAutoAdvance = false;
    frame.setup();
    
    frameRate = 60;
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    path.pushDirectory("GeneratedFrames");
    outputPath = path.toString();
    ofDirectory::createDirectory(outputPath, false, true);
    
    
    index = 0;
    frame.generate(index);
    incrementAt = ofGetElapsedTimef()+(1/frameRate);
}


//--------------------------------------------------------------
void ofApp::update(){

    float now = ofGetElapsedTimef();
    
    if(bAutoAdvance && now > incrementAt) {
        index++;
        frame.generate(index);
        
        stringstream path;
        path << outputPath << std::setw(5) << std::setfill('0') << index << ".png";
        saver.setFromPixels(frame.pixels);
        saver.rotate90(1);
        saver.saveImage(path.str());
        
        
        incrementAt = ofGetElapsedTimef()+(1/frameRate);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    
    float ratio = (ofGetWidth()-20) / frame.getWidth();
    float width = frame.getWidth() * ratio;
    float height = frame.getHeight() * ratio;
    frame.draw(10, 50, width, height);
    
    
    stringstream info;
    info << "frameRate = " << ofGetFrameRate() << endl;
    info << "size = " << frame.getWidth() << "x" << frame.getHeight();
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
    if(key=='a') bAutoAdvance = !bAutoAdvance;
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
