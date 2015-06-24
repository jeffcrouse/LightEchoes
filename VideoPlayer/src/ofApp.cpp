#include "ofApp.h"

#define PAUSE_ON_NEW_FRAME 60

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Video Player");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("ofDirectory", OF_LOG_SILENT);
    ofSetDataPathRoot("../Resources/data/");
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    dropboxPath = path.toString();
    //ofDirectory::createDirectory(photosPath, false, true);
    
    
    stringstream vp;
    vp << dropboxPath << "Video.mp4";
    videoPath = vp.str();

    video.loadMovie(videoPath);
    
    stringstream pp;
    pp << dropboxPath << "_PhotosSmall";
    photosPath = pp.str();
    
    
    dir.allowExt("jpg");
    dir.listDir(photosPath);
    numPhotos = dir.size();
    
    
    bDebug=false;
    
    float ratio =  ofGetHeight() / video.getHeight();
    bounds.height = video.getHeight() * ratio;
    bounds.width = video.getWidth() * ratio;
    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
    bounds.y = 0;
    
    video.setLoopState(OF_LOOP_NONE);
    video.play();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if(video.getIsMovieDone()) {
        dir.allowExt("jpg");
        dir.listDir(photosPath);
        dir.sort();
        if (dir.size() > numPhotos) {
            
        } else {
            video.play();
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    video.draw(bounds);
    
    if(bDebug) {
        stringstream ss;
        ss << "framerate " << ofGetFrameRate() << endl;
        ss << "dropboxPath " << dropboxPath << endl;
        ss << "videoPath " << videoPath << endl;
        ss << "photosPath " << photosPath << endl;
        ss << "PAUSE_ON_NEW_FRAME " << PAUSE_ON_NEW_FRAME;
        ofDrawBitmapStringHighlight(ss.str(), 10, 20);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='f') {
        ofToggleFullscreen();
    }
    if(key=='d') {
        bDebug = !bDebug;
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
