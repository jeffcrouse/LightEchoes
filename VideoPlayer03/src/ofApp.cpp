#include "ofApp.h"
#include "ofxModifierKeys.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("VideoPlayer03");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofBackground(0);
    ofEnableAlphaBlending();
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("ofDirectory", OF_LOG_SILENT);
    ofHideCursor();
    ofEnableNormalizedTexCoords();

    bDebug=false;
    
    videoPath = Poco::Path::home();
    videoPath.pushDirectory("Desktop");
    videoPath.setFileName("LightEchoes.avi");
    video.loadMovie(videoPath.toString());
    video.setLoopState(OF_LOOP_NORMAL);
    video.play();
    
//    float ratio =  ofGetWidth() / (float)video.getWidth();
//    bounds.height = video.getHeight() * ratio;
//    bounds.width = video.getWidth() * ratio;
//    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
//    bounds.y = 0;
    
    int w = video.getWidth();
    int h = video.getHeight();
    
    mesh = ofMesh::plane(w, h);
    vector<ofPoint>& verts = mesh.getVertices();
    verts[0].set(0, 0);
    verts[1].set(w, 0);
    verts[2].set(0, h);
    verts[3].set(w, h);
    
    
    playhead.set(20, ofGetHeight()-50, ofGetWidth()-40, 20);
    progress = playhead;
    
    
    savePath = Poco::Path::home();
    savePath.pushDirectory("Library");
    savePath.pushDirectory("Application Support");
    savePath.pushDirectory("LightEchoes");
    ofDirectory::createDirectory(savePath.toString(), false, true);
    savePath.setFileName("verts.json");
    
    
    load();
}

//--------------------------------------------------------------
void ofApp::save() {
    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        vertsJSON["verts"][i]["x"] = verts[i].x;
        vertsJSON["verts"][i]["y"] = verts[i].y;
    }
    vertsJSON.save(savePath.toString(), true);
}

//--------------------------------------------------------------
void ofApp::load() {
    if(!ofFile::doesFileExist(savePath.toString()) || !vertsJSON.open(savePath.toString())) return;

    vector<ofVec3f>& verts = mesh.getVertices();
    for(int i=0; i<verts.size(); i++) {
        verts[i].x = vertsJSON["verts"][i]["x"].asFloat();
        verts[i].y = vertsJSON["verts"][i]["y"].asFloat();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    
    video.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    //video.draw(bounds);
    
    video.getTextureReference().bind();
    mesh.draw();
    video.getTextureReference().unbind();
    
    
    
    ofSetColor(ofColor::black);
    ofFill();
    ofRect(0, ofGetHeight()-20, ofGetWidth(), 20);
    ofRect(0, 0, 29, ofGetHeight());
    
    if(bDebug) {
        ofPushStyle();
        stringstream debug;
        debug << std::setfill('0') << std::setw(2) << ofGetFrameRate() << endl;
        ofDrawBitmapStringHighlight(debug.str(), 10, 20);
        
        ofFill();
        ofSetColor(ofColor::gray);
        progress.width = ofMap(video.getPosition(), 0, 1, 0, playhead.width);
        ofRect(progress);
        
        ofNoFill();
        ofSetColor(ofColor::white);
        ofRect(playhead);
        
        ofNoFill();
        ofSetRectMode(OF_RECTMODE_CENTER);
        vector<ofPoint>& verts = mesh.getVertices();
        for(int i=0; i<verts.size(); i++) {
            ofSetColor(v==i?ofColor::green:ofColor::white);
            ofSetLineWidth(v==i?3:1);
            ofRect(verts[i].x, verts[i].y, 20, 20);
        }
        ofPopStyle();
    }
    //ofDrawBitmapStringHighlight(path.toString(), 10, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if(key=='d') {
        bDebug=!bDebug;
    }
    if(key=='f') {
        ofToggleFullscreen();
    }
    
    if(bDebug) {
        vector<ofPoint>& verts = mesh.getVertices();
    
        if(key==OF_KEY_TAB) {
            ++v %= verts.size();
        }
        int amt = ofGetModifierPressed(OF_KEY_SHIFT)?5:1;
        if (key==OF_KEY_LEFT) {
            verts[v].x -=amt;
        }
        if (key==OF_KEY_RIGHT) {
            verts[v].x +=amt;
        }
        if (key==OF_KEY_UP) {
            verts[v].y -=amt;
        }
        if (key==OF_KEY_DOWN) {
            verts[v].y +=amt;
        }
        save();
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
//    float ratio =  w / (float)video.getWidth();
//    bounds.height = video.getHeight() * ratio;
//    bounds.width = video.getWidth() * ratio;
//    bounds.x = (w/2.0) - (bounds.width/2.0);
//    bounds.y = 0;
    
//    vector<ofPoint>& verts = mesh.getVertices();
//    verts[0].set(0, 0);
//    verts[1].set(w, 0);
//    verts[2].set(0, h);
//    verts[3].set(w, h);

    playhead.set(20, h-50, w-40, 20);
    progress = playhead;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
