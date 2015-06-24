#include "ofApp.h"
#include "ofxModifierKeys.h"

#define GUI_SETTINGS_XML "settings.xml"
//#define PERSIST_JSON_FILE "persist.json"
#define TRACK_TIME 190
// TEST 1 // 3:16.5
// TEST 2 // 2:53 // 2:55
// TEST 3 // 2:47 // 2:37 // 2:35.6 // 2:39.6 // 2:36 //2:32
// TEST 4 // 2:45 // 2:35

#define POST_RETURN_PAUSE 10
#define PIXELS_ON 10
#define PIXELS_OFF 10
#define DMX_CHANNEL_MOTOR_RELEASE 3
#define DMX_CHANNEL_MOTOR_RETURN 1
#define DMX_CHANNEL_LIGHT 15
#define DMX_CHANNEL_LIGHT_R DMX_CHANNEL_LIGHT+0
#define DMX_CHANNEL_LIGHT_G DMX_CHANNEL_LIGHT+1
#define DMX_CHANNEL_LIGHT_B DMX_CHANNEL_LIGHT+2
//#define DMX_CHANNEL_LIGHT_COLOR_MIX DMX_CHANNEL_LIGHT+3
//#define DMX_CHANNEL_LIGHT_STROBE DMX_CHANNEL_LIGHT+4
//#define DMX_CHANNEL_LIGHT_SOUND_ACTIVE DMX_CHANNEL_LIGHT+5
//#define DMX_CHANNEL_LIGHT_DIMMER DMX_CHANNEL_LIGHT+6

#define LIGHT_R 192
#define LIGHT_G 191
#define LIGHT_B 173

//--------------------------------------------------------------
string ofSystemCall(string command) {
    
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
    ofSetWindowTitle("LightEchoes");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(50);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    //ofSetDataPathRoot("../Resources/data/");
    
    //
    // SETUP AND INITIALIZE!!!
    //
    dmx.connect(0, 64); // or use a number
    etherdream.setup();
    camera.setup();
    //camera.lockUI();
    if(camera.isConnected()) camera.releaseShutterButton();
    
    font.loadFont("fonts/Neue Helvetica/HelveticaNeueLTCom-Bd.ttf", 420);
    calibPattern.drawCalibration();
    startTime = -1;
    bRunning = false;
    trackPos = 0;
    brightness = 0;
    brightnessVelocity = 0;
    //pendulum.bDraw=false;
    bPaused = false;
    bReturnClap=false;
    bStartClap=false;
    bLightOn=false;
    elapsedTime = 0;
    
    //
    // PERSIST: load some persistent variables (that aren't sliders)
    //
//    persist.open(PERSIST_JSON_FILE);
//    if(persist.isMember("currentName")) {
//        currentName = persist["currentName"].asString();
//        ofSetWindowTitle("LightEchoes - "+currentName);
//    } else makeNewName();
    
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    path.pushDirectory("_PhotosBig");
    savePathBig = path.toString();
    ofDirectory::createDirectory(savePathBig, false, true);
    
    path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    path.pushDirectory("_PhotosSmall");
    savePathSmall = path.toString();
    ofDirectory::createDirectory(savePathSmall, false, true);
    
    
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
    //trackTimeSlider = gui->addSlider("TRACK TIME", 60, 210, 150);
    //directionToggle = gui->addLabelToggle("FORWARD", &bForward);
    autoRunToggle = gui->addLabelToggle("AUTO RUN", false);
    //autoRunDelaySlider = gui->addSlider("POST RUN PAUSE", 6, 30, 10);
    //cutout = gui->addRangeSlider("CUTOUT", 0, 1, 0.4, 0.6);
    forceOnToggle = gui->addLabelToggle("FORCE LASER ON", false);
    trackPosSlider = gui->addSlider("TRACK POSITION", 0.0, 1.0, 0.0);
    
    gui->addSpacer();
    gui->addLabel("DMX");
    motorReleaseToggle = gui->addLabelToggle("MOTOR RELEASE", false);
    motorReturnToggle = gui->addLabelToggle("MOTOR RETURN", false);
    lightLevelSlider = gui->addSlider("LIGHT LEVEL", 0.0, 1.0, 0.0);
//    lightColorSlider[0] = gui->addIntSlider("LIGHT RED", 0, 255, 0);
//    lightColorSlider[1] = gui->addIntSlider("LIGHT GREEN", 0, 255, 0);
//    lightColorSlider[2] = gui->addIntSlider("LIGHT BLUE", 0, 255, 0);
    //lightDimmerSlider = gui->addIntSlider("LIGHT LEVEL", 0, 255, 0);
    //lightStrobeSlider = gui->addIntSlider("LIGHT STROBE", 0, 255, 0);
    
    gui->addSpacer();
    gui->addLabel("LASER COLOR ADJUST");
    colorAdjust[0] = gui->addSlider("RED ADJUST", 0, 1, 1);
    colorAdjust[1] = gui->addSlider("GREEN ADJUST", 0, 1, 1);
    colorAdjust[2] = gui->addSlider("BLUE ADJUST", 0, 1, 1);
    
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
    sound.masterVolume = gui->addSlider("MASTER VOLUME", 0, 1, 1);
    sound.tempo = gui->addSlider("TEMPO", 60, 220, 90);
    briChangeThresh = gui->addSlider("PLUCK THRESHOLD", 0, 0.2, 0.05);
    sound.fxVolume = gui->addSlider("FX VOLUME", 0, 1, 1);
    sound.padVolume = gui->addSlider("PAD VOLUME", 0, 1, 1);
    sound.harpVolume = gui->addRangeSlider("HARP VOLUME", 0, 1, 0.25, 0.75);
    sound.drumVolume = gui->addSlider("DRUM VOLUME", 0, 1, 1);
    sound.clapVolume = gui->addSlider("CLAP VOUME", 0, 1, 1);
    //sound.arpVolume = gui->addSlider("ARP VOUME", 0, 1, 0.5);
    sound.lightATheEndVolume = gui->addSlider("LIGHT VOLUME", 0, 1, 0.5);
    
    
    gui->autoSizeToFitWidgets();
    gui->setPosition(0, 20);
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings(GUI_SETTINGS_XML);

    
    trackPosSlider->setValue(trackPos);
    motorReturnToggle->setValue(false);
    motorReleaseToggle->setValue(false);
    forceOnToggle->setValue(false);
//    lightStrobeSlider->setValue(0);
//    lightDimmerSlider->setValue(0);
//    lightColorSlider[0]->setValue(LIGHT_R);
//    lightColorSlider[1]->setValue(LIGHT_G);
//    lightColorSlider[2]->setValue(LIGHT_B);
    lightLevelSlider->setValue(1);
    sound.setup();
}

//--------------------------------------------------------------
ofFloatColor ofApp::mapColor(ofFloatColor c) {
    c.r *= colorAdjust[0]->getValue();
    c.g *= colorAdjust[1]->getValue();
    c.b *= colorAdjust[2]->getValue();
    
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
    //persist.save(PERSIST_JSON_FILE, true);
    delete gui;
}

//--------------------------------------------------------------
void ofApp::drawSafetyPattern() {
    ofxIlda::Frame safetyPattern[3];
    
    safetyPattern[0].clear();
    safetyPattern[1].clear();
    safetyPattern[2].clear();
    
    safetyPattern[0].params.output.color = mapColor(ofFloatColor::white);
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
void ofApp::pause() {
    bPaused = true;
}

//--------------------------------------------------------------
void ofApp::unpause() {
    bPaused = false;
    
}
//--------------------------------------------------------------
void ofApp::togglePaused() {
    if(bPaused) unpause();
    else pause();
}


//--------------------------------------------------------------
void ofApp::update(){
    if(!bPaused)
        elapsedTime += ofGetLastFrameTime();
    
    camera.update();
    dmx.update();
    sound.update();

    if(bPaused) {
        dmx.setLevel(DMX_CHANNEL_MOTOR_RETURN, 0);
        dmx.setLevel(DMX_CHANNEL_MOTOR_RELEASE, 0);
    } else {

        dmx.setLevel(DMX_CHANNEL_MOTOR_RETURN, motorReturnToggle->getValue() ? 255 : 0);
        dmx.setLevel(DMX_CHANNEL_MOTOR_RELEASE, motorReleaseToggle->getValue() ? 255 : 0);
    }
    
    float val = lightLevelSlider->getValue();
    dmx.setLevel(DMX_CHANNEL_LIGHT_R, ofMap(val, 0, 1, 0, LIGHT_R));
    dmx.setLevel(DMX_CHANNEL_LIGHT_G, ofMap(val, 0, 1, 0, LIGHT_G));
    dmx.setLevel(DMX_CHANNEL_LIGHT_B, ofMap(val, 0, 1, 0, LIGHT_B));
    //dmx.setLevel(DMX_CHANNEL_LIGHT_DIMMER, lightDimmerSlider->getValue());
    //dmx.setLevel(DMX_CHANNEL_LIGHT_STROBE, lightStrobeSlider->getValue());
    
    
    // RUN LOGIC
    if(bRunning && !bPaused) {
        ofSetWindowTitle("LightEchoes (RUNNING)");
        
        endTime = startTime + TRACK_TIME;
        if(elapsedTime > endTime) {
            endRun();
        } else {
            trackPos = ofMap(elapsedTime, startTime, endTime, 0, 1, true);
            trackPosSlider->setValue(trackPos);
        }
    }

    if(startTime != -1) {
        ofSetWindowTitle("LightEchoes (COUNTDOWN)");
        
        float timeToStart = startTime - elapsedTime;
        
        if(timeToStart < 2.964 && bStartClap) {
            sound.startClap.playTo(0, 1);
            bStartClap = false;
        }
        
        if(timeToStart<POST_RETURN_PAUSE && bReturnClap) {
            sound.endClap.playTo(0,1);
            bReturnClap = false;
        }
        
        if(timeToStart <= 0) startRun();
    } else {
        ofSetWindowTitle("LightEchoes (IDLE)");
    }
    
    
    etherdream.clear();
    if(bRunning || bPaused || forceOnToggle->getValue()) {
        if(etherdream.checkConnection(true)) {
            drawMainLine();
            //drawPendulum();
        }
    }
    
    
    // Draw calibration pattern if it's on
    if(drawCalibPatternToggle->getValue()==true) {
        calibPattern.update();
        etherdream.addPoints(calibPattern);
    }
    
    // Deal with photos from the camera
    if(camera.isPhotoNew()) {
        string basename = ofGetTimestampString("%m-%d-%H-%M-%S-%i");
        
        stringstream path;
        path << savePathBig << basename << ".jpg";
        ofLogNotice() << "=== SAVING " << path.str();
        camera.savePhoto( path.str() );
        
        stringstream cmd;
        cmd << "sips -Z 1920 \"" << path.str() << "\" --out \"" << savePathSmall << basename << ".jpg\"";
        ofLogNotice() << "=== RESIZING " << cmd.str();
        ofLogNotice() << ofSystemCall( cmd.str() );
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    updatePreviewFBO();
    
    stringstream info1;
    info1 << "etherdream" << endl;
    info1 << "state = " << etherdream.getState() << endl;
    info1 << "mainLine = " << mainLine.points.size();
    ofDrawBitmapStringHighlight(info1.str(), 10, ofGetHeight()-60);

    if(bPaused) {
        ofDrawBitmapStringHighlight("PAUSED", 250, 20);
    }
    
    
    // Brightness bars
    ofFill();
    ofSetColor(ofColor::red);
    ofRect(0, 0, ofMap(brightness, 0, 1, 0, ofGetWidth()), 10);
    ofRect(0, 10, ofMap(brightnessVelocity, 0, 1, 0, ofGetWidth()), 10);
    
    // LEFT SIDE
    ofSetColor(ofColor::white);
    //float ratio = (ofGetHeight()-40) / (float)preview.getHeight();
    preview.draw(250, 20, 400, 900);

    stringstream info2;
    info2 << "index = " << source.getIndex() << endl;
    info2 << "name = " << source.getName() << endl;
    if(startTime!=-1 && elapsedTime > startTime)
        info2 << "time = " << toHMS(endTime - elapsedTime);
    ofDrawBitmapStringHighlight(info2.str(), 250, ofGetHeight()-60);
    
    
    if(camera.isConnected()) {
        // UPPER LEFT - live camera image
        camera.draw(690, 20, 640, 480);
        
        // LOWER LEFT - most recent photo
        camera.drawPhoto(690, 510, 640, 480);
        
        stringstream info3;
        info3 << "BulbExposureTime: " << camera.bulbExposureTime;
        ofColor textColor = camera.isShutterButtonPressed()? ofColor::green: ofColor::red;
        ofDrawBitmapStringHighlight(info3.str(), 690, ofGetHeight()-60, ofColor::black, textColor);
        
    } else {
        ofSetColor(ofColor::black);
        
        ofRect(690, 20, 640, 480);
        ofRect(690, 510, 640, 480);
        
        ofDrawBitmapStringHighlight("NO CAMERA", 690, ofGetHeight()-60, ofColor::red, ofColor::white);
    }
    
    
    if(!bRunning && startTime != -1) {
        float timeToStart = startTime - elapsedTime;
        string message = toHMS(timeToStart+1); //ofToString((int)timeToStart+1);
        ofRectangle bb = font.getStringBoundingBox(message, 0, 0);
        float x = (ofGetWidth()/2.0) - (bb.width/2.0);
        float y = (ofGetHeight()/2.0) + 200;
        ofSetColor(ofColor::black);
        font.drawString(message, x+5, y+5);
        ofSetColor(ofColor::white);
        font.drawString(message, x, y);
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
        ofSetLineWidth(10);
        ofSetColor(bRunning ? ofColor::green : ofColor::red);
        float y1 = ofMap(trackPos, 0, 1, source.getHeight(), 0);
        ofLine(0, y1, source.getWidth(), y1);
    
    /*
        if(bRunning && pendulum.bDraw) {
            ofSetColor(ofColor::red);
            float min = y1 - 100;
            float max = y1 + 100;
            float y2 = ofMap(pendulum.sin, -1, 1, min, max);
            ofLine(0, y2, source.getWidth(), y2);
        }
*/
        ofPopStyle();
    preview.end();
}


//--------------------------------------------------------------
// Add the lines to the etherdream that represent the pendulum.
/*
void ofApp::drawPendulum() {
    pendulum.frame.clear();
    
    if(pendulum.bDraw)
    {
        pendulum.frame.params.output.color = pendulum.color;
        float oldSin = pendulum.sin;
        float newSin =sin(elapsedTime * pendulum.speed);
        float oldVel = pendulum.vel;
        float newVel = newSin-oldSin;
        pendulum.vel = newVel;
        

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
*/


//--------------------------------------------------------------
void ofApp::drawMainLine() {
    // Scan image from "bottom" (max height) to "top" (0)
    int sampleY = ofMap(trackPos, 0, 1, source.getHeight(), 0);
    
    //if(sampleY==mainLine.lastSampleY) return;
    
    vector<ofxIlda::Point> pts;
    ofFloatColor color;
    ofPoint drawPos; // Where in the etherdream frame are we drawing the sampled color?
    drawPos.y = 0.5;
    
    float totalBrightness = 0;
    int sampleX;
    for (int sampleX=0; sampleX<source.getWidth(); sampleX++) {
     
        drawPos.x = sampleX / (float)source.getWidth();
        drawPos.x = ofClamp(drawPos.x, 0, 1);
        
        color = (sampleX % PIXELS_ON<PIXELS_OFF)
            ? mapColor( source.getColor(sampleX, sampleY) )
            : ofFloatColor::black;

        totalBrightness += color.getBrightness();

        pts.push_back(ofxIlda::Point(drawPos, color));

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
}



//--------------------------------------------------------------
void ofApp::motorReturn() {
    motorReturnToggle->setValue(true);
    motorReleaseToggle->setValue(false);
}

//--------------------------------------------------------------
void ofApp::motorRelease() {
    motorReturnToggle->setValue(false);
    motorReleaseToggle->setValue(true);
}

//--------------------------------------------------------------
void ofApp::motorStopSignal() {
    motorReturnToggle->setValue(false);
    motorReleaseToggle->setValue(false);
}

//--------------------------------------------------------------
void ofApp::startRun() {
    if(bRunning) return;
    
    ofLogNotice() << "startRun";
    startTime = elapsedTime;

    sound.newMelody();
    
    motorRelease();

    lightOff();
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
    motorReturn();
    lightOn();
    
    //toggleDirection();
    if(autoRunToggle->getValue()) {
        startTime = elapsedTime + TRACK_TIME + POST_RETURN_PAUSE;
        bReturnClap = true;
        bStartClap = true;
    } else {
        startTime = -1;
    }
}
//--------------------------------------------------------------
void ofApp::lightOn() {
    lightLevelSlider->setValue(1);
//    lightColorSlider[0]->setValue(LIGHT_R);
//    lightColorSlider[1]->setValue(LIGHT_G);
//    lightColorSlider[2]->setValue(LIGHT_B);
    bLightOn = true;
    
}
//--------------------------------------------------------------
void ofApp::lightOff() {
    for(int i=0; i<3; i++)
        lightLevelSlider->setValue(0);
    bLightOn = false;
}

//--------------------------------------------------------------
void ofApp::lightToggle() {
    if(bLightOn) lightOff();
    else lightOn();
}


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
void ofApp::toggleDirection() {
    bForward = !bForward;
    directionToggle->setValue(bForward);
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
      */
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
    else if(name=="MOTOR RELEASE")
    {
        motorReturnToggle->setValue(false);
    }
    else if(name=="MOTOR RETURN")
    {
        motorReleaseToggle->setValue(false);
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
        startTime = elapsedTime + 5;
        bStartClap = true;
    }
    if(key==OF_KEY_ESC) {
        endRun();
    }
    if(key=='e') {
        etherdream.setup();
    }
    if(key=='p') {
        togglePaused();
    }
    if(key=='l'){
        lightToggle();
    }
    if(key=='m') {
        motorRelease();
    }
    if(key=='M') {
        motorReturn();
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

