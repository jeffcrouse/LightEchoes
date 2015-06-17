#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"
#include "ofxJSON.h"
#include "ofxDmx.h"



class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void guiEvent(ofxUIEventArgs &e);
    
        void loadSourceImage(bool increment=true);
        void makeNewName();
        string getSavePath();
        void processFrames();
        void startRun();
        void endRun();
        void toggleDirection();
        void drawPendulum();
        void drawMainLine();
    
        ofxEtherdream etherdream;
        ofxEdsdk::Camera camera;
        ofxUISuperCanvas *gui;
        ofxDmx dmx;
    
    
        float trackPos;
        float startTime;
        bool bForward;
        bool bRunning;
        ofxUISlider* trackTimeSlider;
        ofxUISlider* autoRunDelaySlider;
        ofxUIToggle* directionToggle;
        ofxUILabelToggle* drawCalibPatternToggle;
        ofxUILabelToggle* autoRunToggle;
    
        ofSoundPlayer clap;
        ofSoundPlayer foundation;
        ofSoundPlayer woosh;
        ofSoundPlayer buzz;
    
        ofTrueTypeFont font;
        ofxIlda::Frame calibPattern;
        string currentName; // The name of the video we are working on

        ofxJSONElement persist;
        ofFbo laserPreview;
    
        struct Source {
            ofDirectory dir;
            string dirPath;
            int index;
            string imageName;
            ofImage image;
            
        } source;
    
        struct Pendulum {
            ofxIlda::Frame frame;
            float stripeWidth;
            vector<ofxUISlider*> stripeX;
            ofFloatColor color;
            float height;
            float speed;
            float offset;
            int blankCount;
            float sin;
            ofxUILabelToggle* draw;
        } pendulum;
    
        struct MainLine {
            vector<ofxIlda::Point> points;
            int endCount;
            int blankCount;
            ofPoint samplePos;
            ofPoint drawPos;
            int sampleWidth;    // How many samples should we take from the image?
            float brightness;
        } mainLine;
    
};
