#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    image.loadImage("images/shapes.jpg");
    
    etherdream.setup();
    //etherdream.setWaitBeforeSend(false);
    
    calibration.drawCalibration();
    
    gui0 = new ofxUISuperCanvas("SLIDER WIDGETS");
    
    gui0->addSpacer();
    gui0->addFPSSlider("FPS SLIDER");
    gui0->addSpacer();
    gui0->addLabel("SETTINGS");
    gui0->addIntSlider("PPS", 10000, 60000, 30000);
    gui0->addToggle("CALIBRATION", &bDrawCalibration);
    gui0->addSpacer();
    
    gui0->addSpacer();
    gui0->addLabel("V SLIDERS");
    gui0->addSlider("DRAWPOS", 0.0, 1.0, 0.5, 17, 160);

    gui0->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui0->addSlider("SAMPLEPOS", 0.0, 1.0, 0.5, 17, 160);
    gui0->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui0->autoSizeToFitWidgets();
    
    
    ofAddListener(gui0->newGUIEvent, this, &ofApp::guiEvent);
    gui0->loadSettings("gui0.xml");
}

//--------------------------------------------------------------
void ofApp::exit()
{
    gui0->saveSettings("gui0.xml");
    delete gui0;
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    int kind = e.getKind();
    cout << "got event from: " << name << endl;
    
    if(name == "PPS")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider *) e.getSlider();
        etherdream.setPPS(slider->getValue());
    }
    else if(name == "CALIBRATION")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        bDrawCalibration = toggle->getValue();
    }
    else if(name=="DRAWPOS")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        drawPos = slider->getValue();
    }
    else if(name=="SAMPLEPOS")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        samplePos = slider->getValue();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    points.clear();
    
    ofPoint pos;
    ofFloatColor color;
/*
    for(int i=0; i<360; i++) {

        pos.x = 0.5 + cos(ofDegToRad(i)) * 0.5;
        pos.y = ypos;
        
        color.r = ofRandomf();
        color.g = ofRandomf();
        color.b = ofRandomf();
        
        ofxIlda::Point p;
        p.set(pos, color);
  
        points.push_back(p);
    }
     */
    
    
    
    float y = samplePos * image.getHeight();
    for (int x=0; x<image.getWidth(); x++) {

        color.set( image.getColor(x, y) );
        pos.set(x/(float)image.getWidth(), drawPos);
        
        ofxIlda::Point p;
        p.set(pos, color);
        points.push_back(p);
    }
    
    
    etherdream.setPoints(points);
    
    if(bDrawCalibration) {
        calibration.update();
        etherdream.addPoints(calibration);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){


    ofSetColor(255);

    ofPushMatrix();
    ofTranslate(225, 10);
    ofScale(0.75, 0.75);
    image.draw(0, 0);
    float y = samplePos * image.getHeight();
    ofNoFill();
    ofSetColor(255);
    ofRect(0, y-1, image.getWidth(), 3);
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
