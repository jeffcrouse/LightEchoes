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
    bPreviewMode = false;
    camera.setup();
    
    font.loadFont("fonts/verdana.ttf", 24);
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Desktop");
    path.pushDirectory(ofGetTimestampString("%m-%d-%H-%M-%S-%i"));
    outputDir = path.toString();
    
    
    //camera.lockUI();
    
    sourceImageDir.listDir("images/Text_03");
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
    gui0->addLabel("SETTINGS");
    gui0->addIntSlider("PPS", 10000, 60000, 30000);
    gui0->addToggle("CALIBRATION", &bDrawCalibration);
    forwardToggle = gui0->addToggle("FORWARD", &bForward);
    scanSpeedSlider = gui0->addSlider("SCAN SPEED", 40, 150, 114.30);
    
    gui0->addIntSlider("SAMPLE WIDTH", 400, 2000, 800);
    gui0->addSlider("SWING SPEED", 0, 5, 2);
    gui0->addSlider("BRIGHTNESS", 0.0, 1.0, 0.5);
    gui0->addSlider("SATURATION", 0.0, 1.0, 0.5);
    gui0->addRangeSlider("RED", 0.0, 1.0, &redMin, &redMax);
    gui0->addRangeSlider("GREEN", 0.0, 1.0, &greenMin, &greenMax);
    gui0->addRangeSlider("BLUE", 0.0, 1.0, &blueMin, &blueMax);
    gui0->addSpacer();
    outputDirInput = gui0->addTextInput("SAVE LOCATION", outputDir);
    gui0->addSpacer();
    gui0->addSlider("PREVIEW SPEED", 1, 10, 3);
    gui0->addSpacer();
    gui0->addSlider("DRAWPOS", 0.0, 1.0, 0.5);
    //gui0->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    //samplePosSlider = gui0->addSlider("SAMPLEPOS", 1.0, 0.0, 0.5, 17, 160);
    //gui0->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui0->addSpacer();
    gui0->addIntSlider("LINE END COUNT", 0, 50, 10);
    gui0->addIntSlider("LINE BLANK COUNT", 0, 50, 10);
    
    
    gui0->addSlider("STRIPE WIDTH", 0.0001, 0.1, 0.001);
    gui0->addSlider("STRIPE GAP", 0.0001, 0.2, 0.001);
    gui0->addIntSlider("STRIPE END COUNT", 0, 50, 10);
    gui0->addIntSlider("STRIPE BLANK COUNT", 0, 50, 10);
    
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
ofFloatColor ofApp::map(ofFloatColor inColor) {
    
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
}



//--------------------------------------------------------------
void ofApp::update(){
    float now = ofGetElapsedTimef();
    float endTime = startTime + scanSpeedSlider->getValue();
    
    camera.update();
    
    if(bIsRunning) {
        if(now > endTime) {
            endCapture();
        } else {
            etherdream.clear();
            
            // Calculate the y coordinate to sample from, and to draw at
            drawY = 0.5;
            sampleY = bForward
                ? ofMap(now, startTime, endTime, 0, 1, true)
                : ofMap(now, startTime, endTime, 1, 0, true);

            vector<ofxIlda::Point> points;
 
            // x and y are the position to sample from in the image
            float y = sampleY * sourceImage.getHeight();
            
            for (int i=0; i<sampleWidth; i++) {
                int x = ofMap(i, 0, sampleWidth, 0, sourceImage.getWidth(), true);
                
                color = map( sourceImage.getColor(x, y) );

                float drawX = x / (float)sourceImage.getWidth();
                
                pos.set(ofClamp(drawX, 0, 1), drawY);
                
                ofxIlda::Point p;
                p.set(pos, color);
                points.push_back(p);
            }
            
            
            vector<ofxIlda::Point> finalPoints;
            ofxIlda::Point startPoint = points[0];
            ofxIlda::Point endPoint = points[1];
            
            // repeat at start
            for(int n=0; n<lineEndCount; n++) {
                finalPoints.push_back( ofxIlda::Point(startPoint.getPosition(), ofFloatColor(0, 0, 0, 0)) );
            }
            for(int n=0; n<lineBlankCount; n++) {
                finalPoints.push_back( startPoint );
            }
            for(int j=0; j<points.size(); j++) {
                finalPoints.push_back( points[j] );
            }
            for(int n=0; n<lineBlankCount; n++) {
                finalPoints.push_back( endPoint );
            }
            for(int n=0; n<lineEndCount; n++) {
                finalPoints.push_back( ofxIlda::Point(endPoint.getPosition(), ofFloatColor(0, 0, 0, 0) ));
            }
            etherdream.addPoints(finalPoints);
            

            
            
            
            // Do Stripes
            swingY = 0.4 * sin(ofGetElapsedTimef()*swingSpeed) + 0.4;

            stripes.clear();
            float x = 0;
            while(x < 1) {
                ofPolyline p;
                p.addVertex(x, swingY);
                p.addVertex(x+stripeWidth, swingY);
                stripes.addPoly(p);
                x += stripeGap;
            }
            stripes.params.output.color = map(ofFloatColor(0.4));

            stripes.update();
            etherdream.addPoints(stripes);
   
            
            
            
            //For each subsequent frame,
//            if(ofGetFrameNum() % 2) {
//                reverse(points.begin(), points.end());
//                for(int i=0; i<points.size(); i++) {
//                    points[i].x += reverseOffset;
//                }
//            }
        
        }
    }
    
    if(bDrawCalibration) {
        calibration.update();
        etherdream.addPoints(calibration);
    }
    
    if(bPreviewMode && now > previewNextFrame) {
        previewIterator++;
        if(previewIterator==previewFrames.end()) {
            previewIterator = previewFrames.begin();
        }
        
        float delay = 1/previewAnimSpeed;
        previewNextFrame = now + delay;
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
    

    if(etherdream.checkConnection()) {
        ofSetColor(ofColor::green);
    } else {
        ofSetColor(ofColor::red);
    }
    font.drawString("EtherDream", ofGetWidth()-200, 30);
    
    
    
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
    
    
    
    if(bPreviewMode) {
        float scale = ofGetHeight() / previewIterator->getHeight();
        int y = 0;
        int h = scale * previewIterator->getHeight();
        int w = scale * previewIterator->getWidth();
        int x = (ofGetWidth() / 2.0) - (w/2.0);
        previewIterator->draw(x, y, w, h);
    }

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
    if(key==OF_KEY_RETURN && !bIsRunning) {
        startCapture();
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
    if(key=='p') {
        bPreviewMode=!bPreviewMode;
        if(bPreviewMode) {
            ofDirectory dir;
            dir.listDir(outputDir);
            if( dir.size() ){
                previewFrames.assign(dir.size(), ofImage());
                
                for(int i = 0; i < (int)dir.size(); i++){
                    previewFrames[i].loadImage(dir.getPath(i));
                    int w = previewFrames[i].getWidth();
                    int h = previewFrames[i].getHeight();
                    previewFrames[i].resize(w/3.0, h/3.0);
                }
                previewNextFrame = ofGetElapsedTimef();
                previewIterator = previewFrames.begin();
            } else {
                bPreviewMode = false;
            }
        } else {
            previewFrames.clear();
        }
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
    bIsRunning=true;
}

//--------------------------------------------------------------
void ofApp::endCapture() {
    camera.releaseShutterButton();
    bIsRunning=false;
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
    else if(name == "STRIPE END COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        stripes.params.output.endCount = slider->getValue();
    }
    else if(name == "STRIPE BLANK COUNT")
    {
        ofxUIIntSlider *slider = (ofxUIIntSlider*)e.getSlider();
        stripes.params.output.blankCount = slider->getValue();
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
    else if(name=="DRAWY")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        //drawY = slider->getValue();
    }
    else if(name=="SWING SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        swingSpeed = slider->getValue();
    }
    else if(name=="SAMPLEPOS")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        //samplePos = slider->getValue();
    }
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
    else if(name=="STRIPE WIDTH")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        stripeWidth = slider->getValue();
    }
    else if(name=="STRIPE GAP")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        stripeGap = slider->getValue();
    }
    else if(name=="SCAN SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        scanSpeed = slider->getValue();
    }
    else if(name=="PREVIEW SPEED")
    {
        ofxUISlider *slider = (ofxUISlider *) e.getSlider();
        previewAnimSpeed = slider->getValue();
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
    else if(name=="SAVE LOCATION")
    {
        outputDir = outputDirInput->getTextString();
    }
    gui0->saveSettings("gui0.xml");
}
