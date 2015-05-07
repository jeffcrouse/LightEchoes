#include "ofApp.h"
#include "ofxModifierKeys.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory(ofGetTimestampString("%m-%d-%H-%M-%S-%i"));
    sourceDir = path.toString();
    
    
    gui0 = new ofxUISuperCanvas("SETTINGS");
    
    gui0->addSpacer();
    gui0->addFPSSlider("FPS SLIDER");
    gui0->addSpacer();
    gui0->addLabel("SETTINGS");

    gui0->addSlider("PREVIEW SPEED", 1, 24, 3);
    gui0->addSpacer();
    sourceDirInput = gui0->addTextInput("SAVE LOCATION", sourceDir);
    
    
    gui0->autoSizeToFitWidgets();
    
    ofAddListener(gui0->newGUIEvent, this, &ofApp::guiEvent);
    gui0->loadSettings("gui0.xml");
    gui0->setVisible(false);
}


//--------------------------------------------------------------
void ofApp::exit()
{
    gui0->saveSettings("gui0.xml");
    delete gui0;
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    
    if(now > nextFrame) {
        iter++;
        if(iter == frames.end()) {
            iter = frames.begin();
        }
        
        float delay = 1/speed;
        nextFrame = now + delay;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    if(frames.size() > 0) {
        
        float scale = ofGetHeight() / iter->getHeight();
        int y = 0;
        int h = scale * iter->getHeight();
        int w = scale * iter->getWidth();
        int x = (ofGetWidth() / 2.0) - (w/2.0);
        iter->draw(x, y, w, h);

    } else {
        ofDrawBitmapStringHighlight("Press apple+O to open a directory", 300, 20);
    }
    
}

//--------------------------------------------------------------
void ofApp::refresh(){
    
    ofDirectory dir;
    dir.allowExt("jpg");
    dir.listDir(sourceDir);
    if( dir.size() ){
        
        frames.assign(dir.size(), ofImage());
        
        for(int i = 0; i < (int)dir.size(); i++){
            frames[i].loadImage( dir.getPath(i) );
            int w = frames[i].getWidth();
            int h = frames[i].getHeight();
            frames[i].resize(w/3.0, h/3.0);
        }
        nextFrame = ofGetElapsedTimef();
        iter = frames.begin();
        
    } else {
        frames.clear();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='o' && ofGetModifierPressed(OF_KEY_SPECIAL)) {
        ofFileDialogResult res = ofSystemLoadDialog("Choose a directory", true, sourceDir);
        
        sourceDir = res.getPath();
        sourceDirInput->setTextString(sourceDir);
        
        ofSetWindowTitle(sourceDir);
        refresh();
    }
    if(key=='r') {
        refresh();
    }
    
    if(key=='g') {
        gui0->toggleVisible();
    }
    if(key=='f') {
        ofToggleFullscreen();
    }
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


//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    int kind = e.getKind();
    //cout << "got event from: " << name << endl;
   
    if(name=="SAVE LOCATION")
    {
        sourceDir = sourceDirInput->getTextString();
    }
    else if (name=="PREVIEW SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        speed = slider->getValue();
    }
    gui0->saveSettings("gui0.xml");
}
