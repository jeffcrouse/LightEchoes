#include "ofApp.h"
#include "ofxModifierKeys.h"

#define GUI_SETTINGS_XML "settings.xml"
#define PERSIST_JSON_FILE "persist.json"



//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    ofSerial serial;
    serial.listDevices();
    
    
    //
    // SETUP AND INITIALIZE!!!
    //
    etherdream.setup();
    camera.setup();
    //camera.lockUI();
    if(camera.isConnected()) camera.releaseShutterButton();
    dmx.connect(0);
    font.loadFont("fonts/verdana.ttf", 24);
    calibPattern.drawCalibration();
    startTime = -1;
    bRunning = false;
    trackPos = 0;
    brightness = 0;
    brightnessVelocity = 0;
    mainLine.drawPos.y = 0.5;
    pendulum.bDraw=false;
    bPaused = false;

    dmxLevels[0] = 0;
    dmxLevels[1] = 0;
    dmxLevels[2] = 0;
    
    //
    // PERSIST: load some persistent variables (that aren't sliders)
    //
//    persist.open(PERSIST_JSON_FILE);
//    if(persist.isMember("currentName")) {
//        currentName = persist["currentName"].asString();
//        ofSetWindowTitle("LightEchoes - "+currentName);
//    } else makeNewName();
    
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory("LightEchoes");
    savePath = path.toString();
    
    
    
    //
    //  Set up the source material object
    //
    source.setup();
    preview.allocate(source.getWidth(), source.getHeight(), GL_RGB);
    
    
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
    trackTimeSlider = gui->addSlider("TRACK TIME", 60, 210, 150);
    //directionToggle = gui->addLabelToggle("FORWARD", &bForward);
    autoRunToggle = gui->addLabelToggle("AUTO RUN", false);
    autoRunDelaySlider = gui->addSlider("POST RUN PAUSE", 6, 30, 10);
    //cutout = gui->addRangeSlider("CUTOUT", 0, 1, 0.4, 0.6);
    forceOnToggle = gui->addLabelToggle("FORCE ON", false);
    trackPosSlider = gui->addSlider("TRACK POSITION", 0.0, 1.0, 0.0);
    
    gui->addSpacer();
    colorAdjust[0] = gui->addSlider("RED ADJUST", -1, 0, 1);
    colorAdjust[1] = gui->addSlider("GREEN ADJUST", -1, 0, 1);
    colorAdjust[2] = gui->addSlider("BLUE ADJUST", -1, 0, 1);
    
    gui->addSpacer();
    gui->addLabel("MAIN LINE");
    gui->addIntSlider("LINE END COUNT", 0, 50, 10);
    gui->addIntSlider("LINE BLANK COUNT", 0, 50, 10);
    
    /*
    gui->addSpacer();
    gui->addLabel("PENDULUM");
    pendulum.draw = gui->addLabelToggle("DRAW PENDULUM", true);
    gui->addSlider("SWING SPEED", 0, 5, 2);
    gui->addSlider("SWING SIZE", 0, 1, 0.6);
    gui->addSlider("SWING OFFSET", -0.4, 0.4, 0.0);
    gui->addSlider("PENDULUM STRIPE WIDTH", 1, 1000, 2);
    gui->addSlider("PENDULUM RED", 0, 1, 0.4);
    gui->addSlider("PENDULUM GREEN", 0, 1, 0.4);
    gui->addSlider("PENDULUM BLUE", 0, 1, 0.4);
    gui->addIntSlider("PENDULUM BLANK COUNT", 0, 50, 10);
    float gap = 1/8.0;
    float x = 0;
    for(int i=0; i<9; i++) {
        ofxUISlider* s = gui->addSlider("STRIPE "+ofToString(i), 0, 1, x);
        pendulum.stripeX.push_back(s);
        x += gap;
    }
     */
    gui->addSpacer();
    
    gui->addLabel("SOUND");
    sound.tempo = gui->addSlider("TEMPO", 60, 220, 90);
    briChangeThresh = gui->addSlider("PLUCK THRESHOLD", 0, 0.2, 0.05);
    sound.fxVolume = gui->addSlider("FX VOLUME", 0, 1, 1);
    sound.padVolume = gui->addSlider("PAD VOLUME", 0, 1, 1);
    sound.harpVolume = gui->addRangeSlider("HARP VOLUME", 0, 1, 0.25, 0.75);
    sound.drumVolume = gui->addSlider("DRUM VOLUME", 0, 1, 1);
    sound.arpVolume = gui->addSlider("ARP VOUME", 0, 1, 0.5);
    sound.lightATheEndVolume = gui->addSlider("LIGHT VOLUME", 0, 1, 0.5);
    
    
    gui->autoSizeToFitWidgets();
    gui->setPosition(0, 20);
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings(GUI_SETTINGS_XML);
    
    
    
    sound.setup();

}

//--------------------------------------------------------------
ofFloatColor ofApp::mapColor(ofFloatColor c) {
    c.r += colorAdjust[0]->getValue();
    c.g += colorAdjust[1]->getValue();
    c.b += colorAdjust[2]->getValue();
    
    c.r = ofClamp(c.r, 0, 1);
    c.g = ofClamp(c.g, 0, 1);
    c.b = ofClamp(c.b, 0, 1);
    
    return c;
}

//--------------------------------------------------------------
void ofApp::exit() {
    dmx.clear();
    dmx.update(true); // black on shutdown
    
    camera.releaseShutterButton();
    //camera.unlockUI();
    camera.close();
    
    gui->saveSettings(GUI_SETTINGS_XML);
    persist.save(PERSIST_JSON_FILE, true);
    delete gui;
}

//--------------------------------------------------------------
void ofApp::drawSafetyPattern() {
    ofxIlda::Frame safetyPattern[3];
    
    safetyPattern[0].clear();
    safetyPattern[1].clear();
    safetyPattern[2].clear();
    
    safetyPattern[0].params.output.color = mapColor(ofFloatColor::red);
    safetyPattern[1].params.output.color = mapColor(ofFloatColor::green);
    safetyPattern[2].params.output.color = mapColor(ofFloatColor::blue);
    
    ofPolyline line1;
    line1.addVertex(0, 0.25);
    line1.addVertex(1, 0.25);
    safetyPattern[0].addPoly(line1);
    
    ofPolyline line2;
    line2.addVertex(0, 0.5);
    line2.addVertex(1, 0.5);
    safetyPattern[1].addPoly(line2);
    
    ofPolyline line3;
    line3.addVertex(0, 0.75);
    line3.addVertex(1, 0.75);
    safetyPattern[2].addPoly(line3);
    
    for(int i=0; i<3; i++) {
        safetyPattern[i].update();
        etherdream.addPoints(safetyPattern[i]);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    float endTime = startTime + trackTimeSlider->getValue();
    
    camera.update();
    
    dmx.setLevel(1, dmxLevels[0]);
    dmx.setLevel(2, dmxLevels[1]);
    dmx.setLevel(3, dmxLevels[2]);
    
    dmx.update();
    sound.update(trackPos);
    
    //drawSafetyPattern();
    
    // RUN LOGIC
    if(bRunning && !bPaused) {
        if(now > endTime) {
            endRun();
        } else {
//            trackPos = bForward
//                ? ofMap(now, startTime, endTime, 0, 1, true)
//                : ofMap(now, startTime, endTime, 1, 0, true);
            trackPos = ofMap(now, startTime, endTime, 0, 1, true);
            trackPosSlider->setValue(trackPos);
        }
    }
    
    if(!bRunning && startTime != -1) {
        float timeToStart = startTime - now;
        
        if(timeToStart < 2.975 && !sound.startClap.getIsPlaying())
            sound.startClap.playTo(0, 1);
        
        if(timeToStart < 0) startRun();
    }
    
    
    etherdream.clear();
    if(bRunning || forceOnToggle->getValue()) {
        if(etherdream.checkConnection(true)) {
            drawMainLine();
            drawPendulum();
        }
    }
    
    
    // Draw calibration pattern if it's on
    if(drawCalibPatternToggle->getValue()==true) {
        calibPattern.update();
        etherdream.addPoints(calibPattern);
    }
    
    // Deal with photos from the camera
    if(camera.isPhotoNew()) {
        ofDirectory::createDirectory(savePath, false, true);
        stringstream path;
        path << savePath << "/" << ofGetTimestampString("%m-%d-%H-%M-%S-%i") << ".jpg";
        ofLogNotice() << "=== SAVING " << path.str();
        camera.savePhoto(path.str());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    updatePreviewFBO();
    
    stringstream info1;
    info1 << "etherdream" << endl;
    info1 << "state = " << etherdream.getState() << endl;
    info1 << "mainLine = " << mainLine.points.size() << endl;
    ofDrawBitmapStringHighlight(info1.str(), 10, ofGetHeight()-60);


    // Brightness bars
    ofFill();
    ofSetColor(ofColor::red);
    ofRect(0, 0, ofMap(brightness, 0, 1, 0, ofGetWidth()), 10);
    ofRect(0, 10, ofMap(brightnessVelocity, 0, 1, 0, ofGetWidth()), 10);
    
    // LEFT SIDE
    ofSetColor(ofColor::white);
    preview.draw(250, 30, 500, 1000);

    stringstream info2;
    info2 << "index = " << source.getIndex() << endl;
    info2 << "name = " << source.getName();
    ofDrawBitmapStringHighlight(info2.str(), 250, ofGetHeight()-60);
    
    
    if(camera.isConnected()) {
        // UPPER LEFT - live camera image
        camera.draw(790, 30, 640, 480);
        
        // LOWER LEFT - most recent photo
        camera.drawPhoto(790, 520, 640, 480);
        
        stringstream info3;
        info3 << "BulbExposureTime: " << camera.bulbExposureTime;
        ofColor textColor = camera.isShutterButtonPressed()? ofColor::green: ofColor::red;
        ofDrawBitmapStringHighlight(info3.str(), 790, ofGetHeight()-60, ofColor::black, textColor);
        
    } else {
        ofDrawBitmapStringHighlight("NO CAMERA", 790, 30, ofColor::red, ofColor::white);
    }


    

    // Draw some stuff to show what the laser is projecting
    ofRectMode(OF_RECTMODE_CENTER);
    for(int i=0; i<mainLine.points.size(); i++) {
        ofxIlda::Point& pt = mainLine.points[i];
        if(pt.a>0) {
            ofSetColor(pt.r, pt.g, pt.b);
            ofRect(pt.getPosition().x * ofGetWidth(), ofGetHeight()-5, 2, 2);
        }
    }
    ofRectMode(OF_RECTMODE_CORNER);
}


//--------------------------------------------------------------
// Generate a preview of the source material and some
// lines that represent the laser
void ofApp::updatePreviewFBO() {
    preview.begin();
        ofClear(0);
        ofPushStyle();
    
        ofSetColor(ofColor::white);
        source.draw(0, 0);
        
    
        ofNoFill();
        ofSetLineWidth(3);
        ofSetColor(bRunning ? ofColor::green : ofColor::red);
        float y1 = trackPos * source.getHeight();
        ofLine(0, y1, source.getWidth(), y1);
        
        if(bRunning && pendulum.bDraw) {
            ofSetColor(ofColor::red);
            float min = y1 - 100;
            float max = y1 + 100;
            float y2 = ofMap(pendulum.sin, -1, 1, min, max);
            ofLine(0, y2, source.getWidth(), y2);
        }

        ofPopStyle();
    preview.end();
}


//--------------------------------------------------------------
// Add the lines to the etherdream that represent the pendulum.
void ofApp::drawPendulum() {
    pendulum.frame.clear();
    
    if(pendulum.bDraw)
    {
        pendulum.frame.params.output.color = pendulum.color;
        float oldSin = pendulum.sin;
        float newSin =sin(ofGetElapsedTimef() * pendulum.speed);
        float oldVel = pendulum.vel;
        float newVel = newSin-oldSin;
        pendulum.vel = newVel;
        
        /*
        if(oldSin > 0 && newSin < 0) {
            hihat.play();
            kick.play();
        }
        if(oldSin < 0 && newSin > 0) {
            hihat.play();
            snare.play();
        }
        if(newVel < 0 && oldVel > 0) {
            hihat.play();
        }
        if(newVel > 0 && oldVel < 0) {
            hihat.play();
        }
        */
        
        pendulum.sin = newSin;
        float min = 0.5 - pendulum.height;
        float max = 0.5 + pendulum.height;
        float y = ofMap(pendulum.sin, -1, 1, min, max) + pendulum.offset;
    
        // safety points
        vector<ofxIlda::Point> points;
        for(int n=0; n<pendulum.blankCount; n++) {
            points.push_back( ofxIlda::Point(ofPoint(0, y), ofFloatColor(0, 0, 0, 0) ));
        }
        etherdream.addPoints(points);
        
        // Now add the actual pendulum points
        float w = pendulum.stripeWidth/1000.0;
        for(int i=0; i<pendulum.stripeX.size(); i++) {
            float x1 = pendulum.stripeX[i]->getValue();
            float x2 = (i==pendulum.stripeX.size()-1) ? x1-w : x1+w;
            ofPolyline p;
            p.addVertex(x1, y);
            p.addVertex(x2, y);
            pendulum.frame.addPoly(p);
        }
    }
    
    pendulum.frame.update();
    etherdream.addPoints(pendulum.frame);
}


//--------------------------------------------------------------
void ofApp::drawMainLine() {
    int sampleY = trackPos * source.getHeight();
    
    //if(sampleY==mainLine.lastSampleY) return;
    
    vector<ofxIlda::Point> pts;
    ofFloatColor color;
    
    float totalBrightness = 0;
    int sampleX;
    for (int sampleX=0; sampleX<source.getWidth(); sampleX++) {
     
        mainLine.drawPos.x = sampleX / (float)source.getWidth();
        //bool inCutout = ofInRange(mainLine.drawPos.x, cutout->getValueLow(), cutout->getValueHigh());

        color = (sampleX%10<9)
            ? mapColor( source.getColor(sampleX, sampleY) )
            : ofFloatColor::black;

        totalBrightness += color.getBrightness();
        
        mainLine.drawPos.x = ofClamp(mainLine.drawPos.x, 0, 1);
        
        pts.push_back(ofxIlda::Point(mainLine.drawPos, color));

    }
    
    float oldBrightness = brightness;
    float newBrightness = totalBrightness / (float)source.getWidth();
    float diff = abs(newBrightness - oldBrightness);
    
    brightness = newBrightness;
    brightnessVelocity = diff;

    
    if(brightnessVelocity > briChangeThresh->getValue()) {
        sound.playHarp();
    }

    
    mainLine.points.clear();
    
    // Add safety points to the line
    ofxIlda::Point startPoint = pts[0];
    ofxIlda::Point endPoint = pts[1];
    
    // repeat at start
    for(int n=0; n<mainLine.blankCount; n++) {
        mainLine.points.push_back( ofxIlda::Point(startPoint.getPosition(), ofFloatColor(0, 0, 0, 0)) );
    }
    for(int n=0; n<mainLine.endCount; n++) {
        mainLine.points.push_back( startPoint );
    }
    for(int n=0; n<pts.size(); n++) {
        mainLine.points.push_back( pts[n] );
    }
    for(int n=0; n<mainLine.endCount; n++) {
        mainLine.points.push_back( endPoint );
    }
    for(int n=0; n<mainLine.blankCount; n++) {
        mainLine.points.push_back( ofxIlda::Point(endPoint.getPosition(), ofFloatColor(0, 0, 0, 0) ));
    }
    
    etherdream.addPoints(mainLine.points);
    
    mainLine.lastSampleY = sampleY;
}

//--------------------------------------------------------------
void ofApp::startRun() {
    if(bRunning) return;
    
    ofLogNotice() << "startRun";
    startTime = ofGetElapsedTimef();
    
    sound.newMelody();
    
    // DMX START MOTOR
    
    ofLogNotice() << "===== Pressing shutter button";
    if(camera.isConnected()) camera.pressShutterButton();
    bRunning=true;
}

//--------------------------------------------------------------
void ofApp::endRun() {
    if(!bRunning) return;
    ofLogNotice() << "endRun";
    
    ofLogNotice() << "===== Releasing shutter button";
   if(camera.isConnected()) camera.releaseShutterButton();
    
    bRunning = false;
    
    sound.endClap.playTo(4, 5);
    incrementSource();
    
    
    // DMX RETURN CARRIAGE
    //toggleDirection();
    
    
    
    if(autoRunToggle->getValue()) {
        startTime = ofGetElapsedTimef() + autoRunDelaySlider->getValue();
    } else {
        startTime = -1;
    }
}

/*
//--------------------------------------------------------------
void ofApp::toggleDirection() {
    bForward = !bForward;
    directionToggle->setValue(bForward);
}
*/

//--------------------------------------------------------------
void ofApp::incrementSource() {
    /*
    bool incremented = source.increment();
    if(!incremented) {
        ofLogNotice("ofApp::incrementSource()") << "Processing frames and resetting source materials";
        processFrames();
        makeNewName();
        source.reset();
    }
     */
    
    source.increment();
}


/*
//--------------------------------------------------------------
string ofSystemCall(string command)
{
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

//--------------------------------------------------------------
void ofApp::processFrames() {

    stringstream cmd;
    cmd << "cd " << getSavePath() << ";";
    cmd << "ffmpeg -f image2 -pattern_type glob -i '*.jpg' ";
    cmd << "-s 1920x1080 ";
    cmd << "-c:v libx264 ";
    cmd << "-an ";
    cmd << "-q:v 2 ";
    cmd << "-r 24 ";
    cmd << "-pix_fmt yuv420p ";
    cmd << currentName << ".mp4; ";
    cmd << "open " << currentName << ".mp4";
    
    
    ofSystem(cmd.str());
}

//--------------------------------------------------------------
void ofApp::makeNewName() {
    currentName = ofGetTimestampString("%m-%d-%H-%M-%S-%i");
    ofSetWindowTitle("LightEchoes - "+currentName);
    persist["currentName"] = currentName;
    persist.save(PERSIST_JSON_FILE, true);
}

//--------------------------------------------------------------
string ofApp::getSavePath() {
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory(currentName);
    return path.toString();
}
*/



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
    /*
    else if(name == "FORWARD")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        bForward = toggle->getValue();
    }
     */
    else if(name=="SWING SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.speed = slider->getValue();
    }
    else if(name=="SWING OFFSET")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.offset = slider->getValue();
    }
    else if(name=="SWING SIZE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.height = slider->getValue();
    }
    else if(name=="PENDULUM RED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.color.r =  slider->getValue() ;
    }
    else if(name=="PENDULUM GREEN")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.color.g = slider->getValue();
    }
    else if(name=="PENDULUM BLUE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.color.b = slider->getValue();
    }
    else if(name=="PENDULUM STRIPE WIDTH")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.stripeWidth = slider->getValue();
    }
    else if(name == "LINE END COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        mainLine.endCount = slider->getValue();
    }
    else if(name == "LINE BLANK COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        mainLine.blankCount = slider->getValue();
    }
    else if(name == "TRACK POSITION")
    {
        ofxUISlider *slider = (ofxUISlider*)e.getSlider();
        trackPos = slider->getValue();
    }
    gui->saveSettings(GUI_SETTINGS_XML);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==OF_KEY_TAB) {
        incrementSource();
    }
    if(key==OF_KEY_RETURN) {
        startTime = ofGetElapsedTimef() + 5;
    }
    if(key==OF_KEY_ESC) {
        endRun();
        startTime = -1;
    }
    if(key=='e') {
        etherdream.setup();
    }
    if(key=='h') {
        sound.playHarp();
    }
    if(key=='s') {
        sound.newMelody();
    }
    if(key=='p') {
        bPaused=!bPaused;
    }

    if(key==OF_KEY_UP) {
        dmxLevels[0] = 255;
        dmxLevels[2] = 0;
    }
    
    if(key==OF_KEY_DOWN) {
        dmxLevels[0] = 0;
        dmxLevels[2] = 255;
    }
    
    source.onKeyReleased(key);
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
