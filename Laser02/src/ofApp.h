#pragma once

#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"
#include "ofxJSON.h"
#include "ofxDmx.h"
#include "SourceMaterial.h"
#include "SoundEngine.h"

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
        void startRun();
        void endRun();
        void motorRelease();
        void motorReturn();
        void motorStopSignal();
        void lightOn();
        void lightOff();
        void lightToggle();
        void updatePreviewFBO();
        void drawMainLine();
        void pause();
        void unpause();
        void togglePaused();
        //void drawPendulum();
        //void toggleDirection();
        //void makeNewName();
        //string getSavePath();
        //void processFrames();
        

        ofxEtherdream etherdream;
        ofxEdsdk::Camera camera;
        ofxDmx dmx;
        ofxUISuperCanvas *gui;
        ofTrueTypeFont font;
        SoundEngine sound;
        ofxIlda::Frame calibPattern;
        SourceMaterial source;
        ofFbo preview;
        //ofxJSONElement persist;
    
    
        float elapsedTime;
        string savePathBig, savePathSmall;
        bool bPaused;
        bool bReturnClap;
        bool bStartClap;
        bool bLightOn;
        bool bRunning;
        float trackPos;
        float startTime,endTime;
        float brightness;
        float brightnessVelocity;

        //string currentName; // The name of the video we are working on
        //bool bForward;
        //ofxUIRangeSlider* cutout;
        //ofxUISlider* trackTimeSlider;
        //ofxUISlider* autoRunDelaySlider;
        //ofxUIToggle* directionToggle;
        ofxUISlider* colorAdjust[3];
        ofxUILabelToggle* drawCalibPatternToggle;
        ofxUILabelToggle* autoRunToggle;
        ofxUILabelToggle* forceOnToggle;
        ofxUISlider* trackPosSlider;
        ofxUILabelToggle* motorReturnToggle;
        ofxUILabelToggle* motorReleaseToggle;
        ofxUISlider* lightLevelSlider;
        ofxUISlider* briChangeThresh;
        //ofxUIIntSlider* lightDimmerSlider;
        //ofxUIIntSlider* lightStrobeSlider;
    
    
        /*
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
         */
    
        struct MainLine {
            vector<ofxIlda::Point> points;
            int endCount;
            int blankCount;
            //int lastSampleY;
        } mainLine;
    

};
