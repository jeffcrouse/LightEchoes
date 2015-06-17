#include "ofApp.h"
#include "ofxModifierKeys.h"

#define GUI_SETTINGS_XML "settings.xml"
#define PERSIST_JSON_FILE "persist.json"


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
    source.imageName = "NONE";
    startTime = -1;
    bRunning = false;
    trackPos = 0;
    mainLine.sampleWidth = 800;
    mainLine.drawPos.y = 0.5;
    // LOAD SOUNDS
    clap.loadSound("sounds/clap.wav");
    woosh.loadSound("sounds/woosh2.wav");
    foundation.loadSound("sounds/ambience-synth.wav");
    foundation.setLoop(true);
    foundation.play();
    buzz.loadSound("sounds/buzz.wav");
    buzz.setLoop(true);
    buzz.setVolume(0);
    buzz.play();
    
    
    //
    // PERSIST: load some persistent variables (that aren't sliders)
    //
    persist.open(PERSIST_JSON_FILE);
    
    if (!persist.isMember("sourceImageIndex")) persist["sourceImageIndex"] = 0;
    source.index = persist["sourceImageIndex"].asInt();
    
    if (!persist.isMember("sourceDirPath")) persist["sourceDirPath"] = "Source";
    source.dirPath = persist["sourceDirPath"].asString();
    
    

    
    // Prepare source image
    loadSourceImage(false);
    if(!persist.isMember("currentName")) {
        makeNewName();
    } else {
        currentName = persist["currentName"].asString();
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
    autoRunToggle = gui->addLabelToggle("AUTO RUN", false);
    autoRunDelaySlider = gui->addSlider("POST RUN PAUSE", 1, 10, 10);

    gui->addSpacer();
    gui->addLabel("MAIN LINE");
    gui->addIntSlider("SAMPLE WIDTH", 400, 2000, 800);
    gui->addIntSlider("LINE END COUNT", 0, 50, 10);
    gui->addIntSlider("LINE BLANK COUNT", 0, 50, 10);
    
    gui->addSpacer();
    gui->addLabel("PENDULUM");
    pendulum.draw = gui->addLabelToggle("DRAW PENDULUM", true);
    gui->addSlider("SWING SPEED", 0, 5, 2);
    gui->addSlider("SWING SIZE", 0, 1, 0.6);
    gui->addSlider("SWING OFFSET", -0.4, 0.4, 0.0);
    gui->addSlider("PENDULUM WIDTH", 1, 1000, 2);
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
    gui->addSpacer();
    

    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings(GUI_SETTINGS_XML);
}

//--------------------------------------------------------------
void ofApp::exit() {
    dmx.clear();
    dmx.update(true); // black on shutdown
    
    camera.releaseShutterButton();
    camera.close();
    gui->saveSettings(GUI_SETTINGS_XML);
    persist.save(PERSIST_JSON_FILE, true);
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
            trackPos = bForward
                ? ofMap(now, startTime, endTime, 0, 1, true)
                : ofMap(now, startTime, endTime, 1, 0, true);
            
            etherdream.clear();
            drawMainLine();
            drawPendulum();
        }
    } else {
        if(startTime!=-1 && now > startTime) {
            startRun();
        }
    }
    
    
    // Generate source image preview
    laserPreview.begin();
        ofClear(0);
        ofSetColor(ofColor::white);
        source.image.draw(0, 0);
    
        ofPushStyle();
        ofNoFill();
        ofSetLineWidth(3);
        ofSetColor(bRunning ? ofColor::green : ofColor::red);
        float y1 = trackPos * source.image.getHeight();
        ofLine(0, y1, source.image.getWidth(), y1);
    
        if(pendulum.draw->getValue()) {
            ofSetColor(ofColor::red);
            float min = y1 - 200;
            float max = y1 + 200;
            float y2 = ofMap(pendulum.sin, -1, 1, min, max);
            ofLine(0, y2, source.image.getWidth(), y2);
        }
        ofPopStyle();
    laserPreview.end();
    
    
    // Draw calibration pattern if it's on
    if(drawCalibPatternToggle->getValue()==true) {
        calibPattern.update();
        etherdream.addPoints(calibPattern);
    }
    
    // Deal with photos from the camera
    if(camera.isPhotoNew()) {
        ofDirectory::createDirectory(getSavePath(), false, true);
        stringstream path;
        path << getSavePath() << "/" << ofGetTimestampString("%m-%d-%H-%M-%S-%i") << ".jpg";
        ofLogNotice() << path.str();
        camera.savePhoto(path.str());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofPushMatrix();
    ofTranslate(250, 0);
    
        if(!source.image.isAllocated()) {
            ofSetColor(ofColor::red);
            font.drawString("WARNING: NO SOURCE IMAGE", 0, 40);
        } else {
            ofSetColor(ofColor::white);
            ofDrawBitmapStringHighlight(source.imageName, 0, 20);
            float width = 450;
            float height = (width/laserPreview.getWidth()) * laserPreview.getHeight();
            laserPreview.draw(0, 30, width, height);
        }
        
        // UPPER LEFT - live camera image
        camera.draw(460, 10, 640, 480);
        
        // LOWER LEFT - most recent photo
        camera.drawPhoto(460, 500, 640, 480);
       
        stringstream info;
        info << "sourceImageIndex " << source.index << endl;
        info << "currentName " << currentName << endl;
        ofDrawBitmapStringHighlight(info.str(), 0, ofGetHeight()-40);
        
        // Brightness bars
        ofSetColor(ofColor::red);
        ofRect(0, 0, ofMap(mainLine.brightness, 0, 1, 0, ofGetWidth()), 10);
        ofRect(0, 10, ofMap(mainLine.brightnessVelocity, 0, 1, 0, ofGetWidth()), 10);
    
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::drawPendulum() {
    pendulum.frame.clear();
    
    if(pendulum.draw->getValue())
    {
        pendulum.frame.params.output.color = pendulum.color;
        float oldSin = pendulum.sin;
        float newSin =sin(ofGetElapsedTimef() * pendulum.speed);
        
        if((oldSin > 0 && newSin < 0) || (oldSin < 0 && newSin > 0)) {
            woosh.play();
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

//--------------------------------------------------------------
void ofApp::drawMainLine() {
    mainLine.samplePos.y = trackPos * source.image.getHeight();
    
    vector<ofxIlda::Point> pts;
    ofFloatColor color;
    
    float totalBrightness = 0;
    for (int i=0; i<mainLine.sampleWidth; i++) {
        mainLine.samplePos.x = ofMap(i, 0, mainLine.sampleWidth, 0, source.image.getWidth(), true);
        
        color = (i%10<9)
            ? (ofFloatColor)source.image.getColor(mainLine.samplePos.x, mainLine.samplePos.y)
            : ofFloatColor::black;
        
        totalBrightness += color.getBrightness() ;
        
        mainLine.drawPos.x = mainLine.samplePos.x / (float)source.image.getWidth();
        mainLine.drawPos.x = ofClamp(mainLine.drawPos.x, 0, 1);
        
        pts.push_back(ofxIlda::Point(mainLine.drawPos, color));
    }
    
    float oldBrightness = mainLine.brightness;
    float newBrightness = totalBrightness / (float)mainLine.sampleWidth;
    float diff = oldBrightness-newBrightness;
    mainLine.brightness = newBrightness;
    mainLine.brightnessVelocity = diff / ofGetLastFrameTime();
    
    buzz.setVolume( mainLine.brightness );
    
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
void ofApp::startRun() {
    if(bRunning) return;
    
    clap.play();
    ofLogNotice() << "startRun";
    startTime = ofGetElapsedTimef();
    
    if(bForward) {
        //dmx.setLevel(1, 100);
    } else {
        //dmx.setLevel(1, 0);
    }
    
    //camera.pressShutterButton();
    bRunning=true;
}

//--------------------------------------------------------------
void ofApp::endRun() {
    if(!bRunning) return;
    
    clap.play();
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
    source.image.clear();
    source.dir.listDir(source.dirPath);
    if(source.dir.size()==0) return;
    
    if(increment) {
        source.index++;
        
        // Have we reached the end of the frames?
        if(source.index > source.dir.size()-1) {
            processFrames();
            makeNewName();
            source.index=0;
        }
        
        persist["sourceImageIndex"] = source.index;
        persist.save(PERSIST_JSON_FILE, true);
    }
    
    source.imageName = source.dir.getName(source.index);
    source.image.loadImage(source.dir.getPath(source.index));
    laserPreview.allocate(source.image.getWidth(), source.image.getHeight(), GL_RGB);
    source.image.mirror(true, true);
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
    else if(name=="PENDULUM WIDTH")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulum.stripeWidth = slider->getValue();
    }
    else if(name == "SAMPLE WIDTH")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        mainLine.sampleWidth = slider->getValue();
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
    gui->saveSettings(GUI_SETTINGS_XML);
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
