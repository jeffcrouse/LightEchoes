#include "ofApp.h"

#define PAUSE_ON_NEW_FRAME 60


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
void ofApp::setup(){
    ofSetWindowTitle("LightEchoes Video Player");
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofBackground(0);
    ofSetEscapeQuitsApp(false);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    ofSetLogLevel("ofDirectory", OF_LOG_SILENT);
    ofSetDataPathRoot("../Resources/data/");
    bDebug=false;
    
    
    Poco::Path path = Poco::Path::home();
    path.pushDirectory("Dropbox");
    path.pushDirectory("LE Shared");
    dropboxPath = path.toString();
    //ofDirectory::createDirectory(photosPath, false, true);
    
    
    stringstream path2;
    path2 << dropboxPath << "_PhotosSmall";
    
    dir.allowExt("jpg");
    dir.listDir(path2.str());
    numPhotos = dir.size();
    
    

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
        
        stringstream path;
        path << dropboxPath << "_PhotosSmall";

        dir.allowExt("jpg");
        dir.listDir(path.str());
        dir.sort();
        
        if (dir.size() > numPhotos) {
            onNewPhoto(dir.getPath(dir.size()-1));
        } else {
            video.play();
        }
    }
}

//--------------------------------------------------------------
void ofApp::onNewPhoto(string path) {
    frame.loadImage(path);
    
    
    stringstream cmd;
    cmd << "cd \"" << dropboxPath << "\";";
    cmd << "ffmpeg -f image2 ";
    cmd << "-pattern_type glob -i '_PhotosSmall/*.jpg' ";
    cmd << "-s 1920x1080 ";
    cmd << "-c:v libx264 ";
    cmd << "-an ";
    cmd << "-q:v 2 ";
    cmd << "-r 24 ";
    cmd << "-pix_fmt yuv420p ";
    cmd << "Video.mp4";
    
    ofSystem
}

//--------------------------------------------------------------
void ofApp::draw(){
    video.draw(bounds);
    
    if(bDebug) {
        stringstream ss;
        ss << "framerate " << ofGetFrameRate() << endl;
        ss << "dropboxPath " << dropboxPath << endl;
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
    float ratio =  ofGetHeight() / video.getHeight();
    bounds.height = video.getHeight() * ratio;
    bounds.width = video.getWidth() * ratio;
    bounds.x = (ofGetWidth()/2.0) - (bounds.width/2.0);
    bounds.y = 0;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
