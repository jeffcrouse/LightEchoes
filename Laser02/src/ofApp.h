#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"
#include "ofxJSON.h"
#include "ofxDmx.h"
#include "SourceMaterial.h"

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
    
        void incrementSource();
        void makeNewName();
        string getSavePath();
        void processFrames();
        void startRun();
        void endRun();
        void toggleDirection();
        void updatePreviewFBO();
        void drawPendulum();
        void drawMainLine();
    
        ofxEtherdream etherdream;
        ofxEdsdk::Camera camera;
        ofxUISuperCanvas *gui;
        ofxDmx dmx;
        ofTrueTypeFont font;
        ofxIlda::Frame calibPattern;
        string currentName; // The name of the video we are working on
    
        float trackPos;
        float startTime;
        bool bForward;
        bool bRunning;
        ofxUISlider* trackTimeSlider;
        ofxUISlider* autoRunDelaySlider;
        ofxUIToggle* directionToggle;
        ofxUILabelToggle* drawCalibPatternToggle;
        ofxUILabelToggle* autoRunToggle;
    
        ofSoundPlayer endClap;
        ofSoundPlayer startClap;
        ofSoundPlayer arpPad;
        ofSoundPlayer kick;
        ofSoundPlayer snare;
        ofSoundPlayer hihat;
        ofSoundPlayer bed;
    
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
            ofxUILabelToggle* draw;
            float vel;
        } pendulum;
    
        struct MainLine {
            vector<ofxIlda::Point> points;
            int endCount;
            int blankCount;
            int lastSampleY;
       
            ofPoint drawPos;    // Where in the etherdream frame are we drawing the sampled color?
        } mainLine;
    
        float brightness;
        float brightnessVelocity;
        ofxUISlider* bingThreshold;
};
