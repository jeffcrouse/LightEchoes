#pragma once

#include "ofMain.h"

#define NUM_FRAMES 45
#define PLAYBACK_FRAMERATE 24.0
#define PAUSE_ON_NEW_FRAME 30


#define STATE_LOOKING_FOR_DIR 1
#define STATE_RUNNING 2

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
		
        void loadContent();
        void checkForNewFrame();
        bool onNewFrame(string path);
    
        ofTrueTypeFont font;
    
        ofImage loader;
        //Poco::Timestamp newest;
        //string mountCommand;
        int newest;
        string contentPath;
        ofDirectory dir;
        //string lockfile;

    
        vector<ofTexture> frames;
        vector<ofTexture>::iterator it;
        float nextFrameAt;
        ofRectangle bounds;
    
        bool bDebug;
        int state;
        float nextContentCheck;
};
