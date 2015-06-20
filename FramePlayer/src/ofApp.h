#pragma once

#include "ofMain.h"

#define NUM_FRAMES 45
#define FRAMERATE 24.0


class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    
        void checkForNewFrame();
        void onNewFrame(string path);
    
    
        ofImage loader;
        Poco::Timestamp newest;
        string videoFolder;
        ofDirectory dir;
    
        vector<ofTexture> frames;
        vector<ofTexture>::iterator it;
        float nextFrameAt;
        ofRectangle bounds;
};
