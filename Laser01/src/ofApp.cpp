#include "ofApp.h"
#include "ofxModifierKeys.h"

// 48.0/576
#define DRAW_HEIGHT_RATIO (107.25/636.0)


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(100);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    bIsRecordingMovie=false;
    bDisableLaser = false;
    camera.setup();
    
    font.loadFont("fonts/verdana.ttf", 24);
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory(ofGetTimestampString("%m-%d-%H-%M-%S-%i"));
    outputDir = path.toString();
    
    
    //camera.lockUI();
    
    sourceImageDir.listDir("images/Text_04");
    sourceImageIndex = -1;
    incrementSourceImage();
    
    adjust.load("shaders/adjust");
    
    etherdream.setup();
    //etherdream.setWaitBeforeSend(false);
    
    calibration.drawCalibration();
    
    gui0 = new ofxUISuperCanvas("SETTINGS");
    
    gui0->addSpacer();
    gui0->addFPSSlider("FPS SLIDER");
    gui0->addSpacer();
    gui0->addLabel("LASER");
    gui0->addIntSlider("PPS", 10000, 60000, 30000);
    gui0->addToggle("CALIBRATION", &bDrawCalibration);
    forwardToggle = gui0->addToggle("FORWARD", &bForward);
    drawTimeSlider = gui0->addSlider("DRAW TIME", 40, 150, 114.30);
    
    
    
//    gui0->addSpacer();
//    gui0->addSlider("BRIGHTNESS", 0.0, 1.0, 0.5);
//    gui0->addSlider("SATURATION", 0.0, 1.0, 0.5);
//    gui0->addRangeSlider("RED", 0.0, 1.0, &redMin, &redMax);
//    gui0->addRangeSlider("GREEN", 0.0, 1.0, &greenMin, &greenMax);
//    gui0->addRangeSlider("BLUE", 0.0, 1.0, &blueMin, &blueMax);
    gui0->addSpacer();
    outputDirInput = gui0->addTextInput("SAVE LOCATION", outputDir);
    
    gui0->addSpacer();
    gui0->addLabel("IMAGE LINE");
    gui0->addIntSlider("SAMPLE WIDTH", 400, 2000, 800);
    gui0->addIntSlider("LINE END COUNT", 0, 50, 10);
    gui0->addIntSlider("LINE BLANK COUNT", 0, 50, 10);
    gui0->addSpacer();
    
    

    gui0->addToggle("PENDULUM", &bDrawPendulum);
    gui0->addSlider("SWING SPEED", 0, 5, 2);
    gui0->addSlider("SWING SIZE", 0, 1, 0.6);
    gui0->addSlider("SWING OFFSET", -0.4, 0.4, 0.0);
    gui0->addSlider("PENDULUM WIDTH", 1, 1000, 2);
    gui0->addSlider("PENDULUM RED", 0, 1, 0.4);
    gui0->addSlider("PENDULUM GREEN", 0, 1, 0.4);
    gui0->addSlider("PENDULUM BLUE", 0, 1, 0.4);
    gui0->addIntSlider("PENDULUM END COUNT", 0, 50, 10);
    gui0->addIntSlider("PENDULUM BLANK COUNT", 0, 50, 10);
     gui0->addSpacer();
    
    
    float gap = 1/8.0;
    float x = 0;
    for(int i=0; i<9; i++) {
        stripeX.push_back(gui0->addSlider("STRIPE "+ofToString(i), 0, 1, x));
        x += gap;
    }
    
    gui0->autoSizeToFitWidgets();
    
    ofAddListener(gui0->newGUIEvent, this, &ofApp::guiEvent);
    gui0->loadSettings("gui0.xml");
    
}

//--------------------------------------------------------------
void ofApp::exit()
{
    camera.releaseShutterButton();
    camera.close();
    gui0->saveSettings("gui0.xml");
    delete gui0;
}

//--------------------------------------------------------------

/*ofFloatColor ofApp::map(ofFloatColor inColor) {
    
    ofFloatColor outColor;
    
    float red = ofMap(inColor.r, 0, 1, redMin, redMax);
    float green = ofMap(inColor.g, 0, 1, greenMin, greenMax);
    float blue = ofMap(inColor.b, 0, 1, blueMin, blueMax);
    
    outColor.set(red, green, blue);
    
    float hue = outColor.getHue();
    float sat = outColor.getSaturation() * saturation;
    float bri = outColor.getBrightness() * brightness;
    
    outColor.setHsb(hue, sat, bri);

    return outColor;
}*/

//--------------------------------------------------------------
void ofApp::drawPendulum() {
    pendulum.clear();
    
    
    vector<ofxIlda::Point> points;

    float swingMin = 0.5 - swingSize;
    float swingMax = 0.5 + swingSize;
    pendulumY = ofMap(sin(ofGetElapsedTimef()*swingSpeed), -1, 1, swingMin, swingMax);
    pendulumY += swingOffset;
    
    // Add some blank points to the beginning
    for(int n=0; n<lineBlankCount; n++) {
        points.push_back( ofxIlda::Point(ofPoint(0, pendulumY), ofFloatColor(0, 0, 0, 0) ));
    }
    etherdream.addPoints(points);
    
    
    // Now add the actual pendulum points
    float w = pendulumWidth/1000.0;
    for(int i=0; i<stripeX.size(); i++) {
        float x1 = stripeX[i]->getValue();
        float x2 = (i==stripeX.size()-1) ? x1-w : x1+w;
        ofPolyline p;
        p.addVertex(x1, pendulumY);
        p.addVertex(x2, pendulumY);
        pendulum.addPoly(p);
    }
    
    pendulum.params.output.color = pendulumColor;
    pendulum.update();
    etherdream.addPoints(pendulum);
}

//--------------------------------------------------------------
void ofApp::drawImage() {
    
    // Calculate the y coordinate to sample from, and to draw at
    drawY = 0.5;
    sampleY = bForward
        ? ofMap(now, startTime, endTime, 0, 1, true)
        : ofMap(now, startTime, endTime, 1, 0, true);
    
    
    // Generate the first batch of points to draw the line
    vector<ofxIlda::Point> points;
    
    // x and y are the position to sample from in the image
    float y = sampleY * sourceImage.getHeight();
    
    for (int i=0; i<sampleWidth; i++) {
        
        int x = ofMap(i, 0, sampleWidth, 0, sourceImage.getWidth(), true);
        
        color = (i%10<9)
            ? (ofFloatColor)sourceImage.getColor(x, y)
            : ofFloatColor::black;
        
        //color =  sourceImage.getColor(x, y);
        
        float drawX = x / (float)sourceImage.getWidth();
        
        pos.set(ofClamp(drawX, 0, 1), drawY);
        
        ofxIlda::Point p;
        p.set(pos, color);
        points.push_back(p);
    }
    
    if(bDisableLaser) {
        for(int i=0; i<points.size(); i++) {
            points[i].r = 0;
            points[i].g = 0;
            points[i].b = 0;
        }
    }
    
    
    // Add safety points to the line
    vector<ofxIlda::Point> finalPoints;
    ofxIlda::Point startPoint = points[0];
    ofxIlda::Point endPoint = points[1];
    
    // repeat at start
    for(int n=0; n<lineBlankCount; n++) {
        finalPoints.push_back( ofxIlda::Point(startPoint.getPosition(), ofFloatColor(0, 0, 0, 0)) );
    }
    for(int n=0; n<lineEndCount; n++) {
        finalPoints.push_back( startPoint );
    }
    for(int j=0; j<points.size(); j++) {
        finalPoints.push_back( points[j] );
    }
    for(int n=0; n<lineEndCount; n++) {
        finalPoints.push_back( endPoint );
    }
    for(int n=0; n<lineBlankCount; n++) {
        finalPoints.push_back( ofxIlda::Point(endPoint.getPosition(), ofFloatColor(0, 0, 0, 0) ));
    }
    etherdream.addPoints(finalPoints);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    now = ofGetElapsedTimef();
    endTime = startTime + drawTimeSlider->getValue();
    
    camera.update();
    
    if(bDrawingInProgress) {
        if(now > endTime) {
            endCapture();
        } else {
            etherdream.clear();
            drawImage();
            if(bDrawPendulum) drawPendulum();
        }
    }
    
    if(bDrawCalibration) {
        calibration.update();
        etherdream.addPoints(calibration);
    }
    
    if(camera.isPhotoNew()) {
        ofDirectory::createDirectory(outputDir, false, true);
        stringstream path;
        path << outputDir << "/" << ofGetTimestampString("%m-%d-%H-%M-%S-%i") << ".jpg";
        ofLogNotice() << path.str();
        camera.savePhoto(path.str());
        
        toggleDirection();
        incrementSourceImage();
    }
    
    if(camera.isMovieNew()) {
        string path = ofToDataPath(ofGetTimestampString("%m-%d-%H-%M-%S-%i") + ".mov");
        ofLogNotice() << path;
        camera.savePhoto(path);
    }
    
    updateSourceImagePreview();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // UPPER RIGHT
    ofSetColor(ofColor::white);
    sourceImagePreview.draw(225, 10, 640, 480);
    ofSetColor(ofColor::purple);
    ofDrawBitmapStringHighlight(sourceImageName, 225, 20);
    //font.drawString(sourceImageName, 225, 40);

    // LOWER RIGHT
    ofSetColor(ofColor::white);
    adjust.begin();
    adjust.setUniform1f("brightness", brightness);
    adjust.setUniform1f("redMin", redMin);
    adjust.setUniform1f("redMax", redMax);
    adjust.setUniform1f("greenMin", greenMin);
    adjust.setUniform1f("greenMax", greenMax);
    adjust.setUniform1f("blueMin", blueMin);
    adjust.setUniform1f("blueMax", blueMax);
    sourceImage.draw(225, 500, 640, 480);
    adjust.end();
    
    
    // UPPER LEFT
    camera.draw(875, 10, 640, 480);
    
    // LOWER LEFT
    camera.drawPhoto(875, 500, 640, 480);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key==' ') {
        camera.pressShutterButton();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key==' ') {
        camera.releaseShutterButton();
    }
    if(key==OF_KEY_RETURN && !bDrawingInProgress) {
        startCapture();
    }
    if(key=='l'){
        bDisableLaser=!bDisableLaser;
    }
    if(key=='v') {
        bIsRecordingMovie=!bIsRecordingMovie;
        
        if(bIsRecordingMovie) {
            camera.beginMovieRecording();
            ofLogNotice() << "Recording...";
        } else {
            camera.endMovieRecording();
            ofLogNotice() << "Stopping...";
        }
    }
    if(key=='e') {
        etherdream.setup();
    }
    
    if(key=='s' && ofGetModifierPressed(OF_KEY_SPECIAL)) {
        ofFileDialogResult res = ofSystemLoadDialog("Choose a save directory", true, outputDir);
  
        outputDir = res.getPath();
        outputDirInput->setTextString(outputDir);
    }

    if(key==OF_KEY_ESC) {
        sampleY=0;
        endCapture();
        startTime = ofGetElapsedTimef()-1;
    }

    if(key==OF_KEY_TAB) {
        incrementSourceImage();
    }
    if(key=='f') {
        toggleDirection();
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

//--------------------------------------------------------------
void ofApp::incrementSourceImage() {
    sourceImageIndex++;
    if(sourceImageIndex>sourceImageDir.size()-1) sourceImageIndex=0;
    
    sourceImageName = sourceImageDir.getName(sourceImageIndex);
    sourceImage.loadImage(sourceImageDir.getPath(sourceImageIndex));
    
    sourceImage.mirror(true, true);
    sourceImagePreview.allocate(sourceImage.getWidth(), sourceImage.getHeight());
}

//--------------------------------------------------------------
void ofApp::toggleDirection() {
    bForward = !bForward;
    forwardToggle->setValue(bForward);
}

//--------------------------------------------------------------
void ofApp::startCapture() {
    sampleY = 0;
    sampleY = 0;
    startTime = ofGetElapsedTimef();
    camera.pressShutterButton();
    bDrawingInProgress=true;
}

//--------------------------------------------------------------
void ofApp::endCapture() {
    camera.releaseShutterButton();
    bDrawingInProgress=false;
}


//--------------------------------------------------------------
void ofApp::updateSourceImagePreview() {
    sourceImagePreview.begin();
    ofClear(0);
    sourceImage.draw(0, 0);
    for(int n=0; n<2; n++) {
        float y = sampleY * sourceImage.getHeight();
        ofPushStyle();
        ofNoFill();
        ofSetLineWidth(3);
        ofSetColor(255, 0, 0);
        ofRect(0, y-1, sourceImage.getWidth(), 3);
    }
    ofPopStyle();
    sourceImagePreview.end();
}


//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    int kind = e.getKind();
    //cout << "got event from: " << name << endl;
    
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
    else if(name == "SAMPLE WIDTH")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        sampleWidth = slider->getValue();
    }
    else if(name == "PENDULUM END COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        pendulum.params.output.endCount = slider->getValue();
    }
    else if(name == "PENDULUM BLANK COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        pendulum.params.output.blankCount = slider->getValue();
    }
    else if(name == "LINE END COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        lineEndCount = slider->getValue();
    }
    else if(name == "LINE BLANK COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        lineBlankCount = slider->getValue();
    }
    else if(name == "FORWARD")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        bForward = toggle->getValue();
    }
    else if(name == "PENDULUM")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
        bDrawPendulum = toggle->getValue();
    }
    else if(name=="SWING SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        swingSpeed = slider->getValue();
    }
    else if(name=="SWING OFFSET")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        swingOffset = slider->getValue();
    }
    else if(name=="SWING SIZE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        swingSize = slider->getValue();
    }
    else if(name=="PENDULUM RED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulumColor.r =  slider->getValue() ;
    }
    else if(name=="PENDULUM GREEN")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulumColor.g = slider->getValue();
    }
    else if(name=="PENDULUM BLUE")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulumColor.b = slider->getValue();
    }
    else if(name=="PENDULUM WIDTH")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        pendulumWidth = slider->getValue();
    }
    else if(name=="SCAN SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        scanSpeed = slider->getValue();
    }
    /*
    else if(name=="BRIGHTNESS")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        brightness = slider->getValue();
    }
    else if(name=="SATURATION")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        saturation = slider->getValue();
    }
    else if(name=="RED")
    {
        ofxUIRangeSlider* slider = (ofxUIRangeSlider*) e.getSlider();
        redMin = slider->getValueLow();
        redMax = slider->getValueHigh();
    }
    else if(name=="GREEN")
    {
        ofxUIRangeSlider* slider = (ofxUIRangeSlider*) e.getSlider();
        greenMin = slider->getValueLow();
        greenMax = slider->getValueHigh();
    }
    else if(name=="BLUE")
    {
        ofxUIRangeSlider* slider = (ofxUIRangeSlider*) e.getSlider();
        blueMin = slider->getValueLow();
        blueMax = slider->getValueHigh();
    }
     */
    else if(name=="SAVE LOCATION")
    {
        outputDir = outputDirInput->getTextString();
    }
    
//    for(int i=0; i<stripeX.size(); i++) {
//        if(name=="STRIPE "+ofToString(i))  {
//            ofxUISlider *slider = (ofxUISlider *) e.getSlider();
//            stripeX[i] = slider->getValue();
//        }
//    }
    
    
    gui0->saveSettings("gui0.xml");
}
