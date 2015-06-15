#pragma once

#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxUI.h"
#include "ofxEdsdk.h"


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
	
        float now, endTime;
        
        void drawPendulum();
        void drawImage();
    
        ofTrueTypeFont font;
  
        void incrementSourceImage();
        void toggleDirection();
        void startCapture();
        void endCapture();
        void updateSourceImagePreview();
    
        //ofFloatColor map(ofFloatColor c);
        bool bIsRecordingMovie;
        bool bDrawingInProgress;
        bool bForward;
        bool bDisableLaser;
        float brightness;
    
        // CAPTURE MODE STUFF
        float startTime;
        ofDirectory sourceImageDir;
        string sourceImageName;
        string outputDir;
        ofxUITextInput* outputDirInput;
    
    
        // SOURCE IMAGE STUFF
        ofFbo sourceImagePreview;
        ofImage sourceImage;
        int sampleWidth;
    
        ofxEtherdream etherdream;
        int sourceImageIndex;
        
        int lineEndCount;
        int lineBlankCount;

        ofxIlda::Frame calibration;
        ofxUISuperCanvas *gui0;
        //ofxUISlider *samplePosSlider;
    
        ofxUISlider* drawTimeSlider;
        ofxUIToggle* forwardToggle;
    
        bool bDrawPendulum;
        bool bDrawCalibration;
        void guiEvent(ofxUIEventArgs &e);
        float drawY;            // Y Position for the laser to draw at
        float sampleY;         // Y position to sample from
    
    

    
    
        //float scanSpeed;
        ofxEdsdk::Camera camera;

        float pendulumWidth;
        float pendulumY;
        vector<ofxUISlider*> stripeX;
        ofFloatColor pendulumColor;
        ofxIlda::Frame pendulum;
        float swingSpeed;
        float swingSize;
        float swingOffset;

    
        ofPoint pos;
        ofFloatColor color;
};


