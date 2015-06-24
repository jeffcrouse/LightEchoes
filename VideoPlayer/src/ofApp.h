#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

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
		
        bool bDebug;
    
        ofDirectory dir;
        ofImage frame;
        ofVideoPlayer video;
        ofRectangle bounds;
    
        int numPhotos;
        float nextFramecheck;
        float nextVideoReload;
    
        float frameStart,frameEnd;
        float frameAlpha;
    
        string dropboxPath;
        string videoPath;
        string photosDir;
        string lockfilePath;
};
