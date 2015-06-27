#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp, ofThread {

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
        void threadedFunction();
        void makeVideo();
        void cleanup();
    
        bool bMakingVideo;  // THREADED VAR!
        bool bDebug;
        ofVideoPlayer* video;
        ofRectangle bounds;
        ofTrueTypeFont font;
        float nextVideoCheck;
        string currentVideo;
    
        Poco::Path framesPath;
        Poco::Path desktop;
        Poco::Timestamp videoModified;
};
