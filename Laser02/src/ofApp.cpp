#include "ofApp.h"
#include "ofxModifierKeys.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);


    //
    // SETUP AND INITIALIZE!!!
    //
    camera.setup();
    etherdream.setup();
    dmx.connect("tty.usbserial-EN143965");
    font.loadFont("fonts/verdana.ttf", 24);
    calibPattern.drawCalibration();
    sourceImageName = "NONE";
    startTime = -1;
    bRunning = false;
    yPos = 0;
    
    
    
    //
    // PERSIST: load some persistent variables (that aren't sliders)
    //
    persist.open("persist.json");
    
    if (!persist.isMember("sourceImageIndex")) persist["sourceImageIndex"] = 0;
    sourceImageIndex = persist["sourceImageIndex"].asInt();
    
    if (!persist.isMember("sourceDirPath")) persist["sourceDirPath"] = "Source";
    sourceDirPath = persist["sourceDirPath"].asString();
    
    

    
    // Prepare input and output stuff
    loadSourceImage(false);
    if(!persist.isMember("savePath")) {
        makeNewSavePath();
    } else {
        savePath = persist["savePath"].asString();
    }
    
    
    
    
    //
    //  GUI
    //
    gui = new ofxUISuperCanvas("SETTINGS");
    gui->addSpacer();
    gui->addFPSSlider("FPS SLIDER");
    gui->addSpacer();
    gui->addLabel("LASER SETTINGS");
    gui->addIntSlider("PPS", 10000, 60000, 30000);
    drawCalibPatternToggle = gui->addLabelToggle("CALIBRATION PATTERN", false);
    trackTimeSlider = gui->addSlider("TRACK TIME", 40, 150, 114.30);
    directionToggle = gui->addLabelToggle("FORWARD", &bForward);
    
    gui->addSpacer();
    gui->addLabel("DRAW SETTINGS");
    autoRunToggle = gui->addLabelToggle("AUTO RUN", false);
    autoRunDelaySlider = gui->addSlider("POST RUN PAUSE", 1, 10, 10);


    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings("settings.xml");
}

//--------------------------------------------------------------
void ofApp::exit() {
    dmx.clear();
    dmx.update(true); // black on shutdown
    
    camera.releaseShutterButton();
    camera.close();
    gui->saveSettings("settings.xml");
    persist.save("persist.json", true);
    delete gui;
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    float endTime = startTime + trackTimeSlider->getValue();
    
    camera.update();
    dmx.update();
    
    // RUN LOGIC
    if(bRunning) {
        if(now > endTime) {
            endRun();
        } else {
            yPos = bForward
                ? ofMap(now, startTime, endTime, 0, 1, true)
                : ofMap(now, startTime, endTime, 1, 0, true);
            etherdream.clear();
            
            drawLine.update(yPos, sourceImage);
            etherdream.addPoints(drawLine);
            
            pendulum.update(yPos);
            etherdream.addPoints(pendulum);
        }
    } else {
        if(startTime!=-1 && now > startTime) {
            startRun();
        }
    }
    
    laserPreview.begin();
        ofClear(0);
        sourceImage.draw(0, 0);
    
        ofPushStyle();
        ofNoFill();
        ofSetLineWidth(3);
        ofSetColor(bRunning ? ofColor::green : ofColor::red);
        float y = yPos * sourceImage.getHeight();
        ofRect(0, y-4, sourceImage.getWidth(), 8);
        ofPopStyle();
    laserPreview.end();
    
    
    if(drawCalibPatternToggle->getValue()) {
        calibPattern.update();
        etherdream.addPoints(calibPattern);
    }
    
    if(camera.isPhotoNew()) {
        ofDirectory::createDirectory(savePath, false, true);
        stringstream path;
        path << savePath << "/" << ofGetTimestampString("%m-%d-%H-%M-%S-%i") << ".jpg";
        ofLogNotice() << path.str();
        camera.savePhoto(path.str());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    stringstream info;
    info << "sourceImageIndex " << sourceImageIndex << endl;
    info << "savePath " << savePath << endl;
    ofDrawBitmapStringHighlight(info.str(), 10, ofGetHeight()-40);
    
    
    ofPushMatrix();
    ofTranslate(250, 0);
    
    if(!sourceImage.isAllocated()) {
        ofSetColor(ofColor::red);
        font.drawString("WARNING: NO SOURCE IMAGE", 0, 40);
    } else {
        ofDrawBitmapStringHighlight(sourceImageName, 0, 20);
        float width = 450;
        float height = (width/laserPreview.getWidth()) * laserPreview.getHeight();

        laserPreview.draw(0, 30, width, height);
    }
    
    // UPPER LEFT - live camera image
    camera.draw(460, 10, 640, 480);
    
    // LOWER LEFT - most recent photo
    camera.drawPhoto(460, 500, 640, 480);
   
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::startRun() {
    if(bRunning) return;
    
    ofLogNotice() << "startRun";
    startTime = ofGetElapsedTimef();
    yPos = 0;
    //camera.pressShutterButton();
    bRunning=true;
}

//--------------------------------------------------------------
void ofApp::endRun() {
    if(!bRunning) return;
    
    ofLogNotice() << "endRun";
    //camera.releaseShutterButton();
    bRunning = false;
    
    toggleDirection();
    loadSourceImage(true);
    
    if(autoRunToggle->getValue())
        startTime = ofGetElapsedTimef() + 10;
}

//--------------------------------------------------------------
void ofApp::toggleDirection() {
    bForward = !bForward;
    directionToggle->setValue(bForward);
}

//--------------------------------------------------------------
void ofApp::loadSourceImage(bool increment) {
    sourceImage.clear();
    sourceImageDir.listDir(sourceDirPath);
    if(sourceImageDir.size()==0) return;
    
    if(increment) {
        sourceImageIndex++;
        
        // Have we reached the end of the frames?
        if(sourceImageIndex>sourceImageDir.size()-1) {
            processFrames();
            sourceImageIndex=0;
        }
        persist["sourceImageIndex"] = sourceImageIndex;
        persist.save("persist.json", true);
    }
    
    sourceImageName = sourceImageDir.getName(sourceImageIndex);
    sourceImage.loadImage(sourceImageDir.getPath(sourceImageIndex));
    laserPreview.allocate(sourceImage.getWidth(), sourceImage.getHeight(), GL_RGB);
    sourceImage.mirror(true, true);
}

//--------------------------------------------------------------
void ofApp::processFrames() {
    
    ofDirectory dir;
    dir.listDir(savePath);
    
    
    // process frames here.
    makeNewSavePath();
}

//--------------------------------------------------------------
void ofApp::makeNewSavePath() {
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory(ofGetTimestampString("%m-%d-%H-%M-%S-%i"));
    savePath =  path.toString();
    persist["savePath"] = savePath;
    persist.save("persist.json", true);
}


//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    string name = e.getName();
    int kind = e.getKind();
    cout << "got event from: " << name << endl;
    
    
    if(name == "PPS")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider *) e.getSlider();
        etherdream.setPPS(slider->getValue());
    }
    else if(name == "FORWARD")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        bForward = toggle->getValue();
    }
    
    gui->saveSettings("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==OF_KEY_TAB) {
        loadSourceImage(true);
    }
    if(key==OF_KEY_RETURN) {
        startRun();
    }
    if(key==OF_KEY_ESC) {
        endRun();
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
