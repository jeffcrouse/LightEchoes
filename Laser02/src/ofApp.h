#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"
#include "ofxJSON.h"
#include "ofxDmx.h"
#include "SourceMaterial.h"
#include "SoundEngine.h"

#define NUM_DMX_CHANNELS 5

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
    
        ofFloatColor mapColor(ofFloatColor c);
        void drawSafetyPattern();
        void incrementSource();
        //void makeNewName();
        //string getSavePath();
        //void processFrames();
        void startRun();
        void endRun();
        void laserRelease();
        void laserReturn();
        void laserStopSignal();
        //void toggleDirection();
        void updatePreviewFBO();
        void drawPendulum();
        void drawMainLine();
    
        ofxEtherdream etherdream;
        ofxEdsdk::Camera camera;
        ofxUISuperCanvas *gui;
        ofxDmx dmx;
        ofTrueTypeFont font;
        ofxIlda::Frame calibPattern;
        ofxUISlider* colorAdjust[3];
        //ofxUIRangeSlider* cutout;
        string savePathRaw, savePathSmall;
        bool bPaused;

    
        //string currentName; // The name of the video we are working on
        float trackPos;
        float startTime;
        //bool bForward;
        bool bRunning;
        //ofxUISlider* trackTimeSlider;
        ofxUISlider* autoRunDelaySlider;
        //ofxUIToggle* directionToggle;
        ofxUILabelToggle* drawCalibPatternToggle;
        ofxUILabelToggle* autoRunToggle;
        ofxUILabelToggle* forceOnToggle;
        ofxUISlider* trackPosSlider;
    
        ofxUISlider* dmxLevel[NUM_DMX_CHANNELS];
    
        SoundEngine sound;
    
        ofxJSONElement persist;
        SourceMaterial source;
        ofFbo preview;

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
            bool bDraw;
            float vel;
        } pendulum;
    
        struct MainLine {
            vector<ofxIlda::Point> points;
            int endCount;
            int blankCount;
            //int lastSampleY;
            ofPoint drawPos;    // Where in the etherdream frame are we drawing the sampled color?
        } mainLine;
    
        float brightness;
        float brightnessVelocity;
        ofxUISlider* briChangeThresh;
};
