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
	
    
        ofTrueTypeFont font;
  
        void incrementSourceImage();
        void toggleDirection();
        void startCapture();
        void endCapture();
        void updateSourceImagePreview();
    
        ofFloatColor map(ofFloatColor c);
        bool bIsRecordingMovie;
        bool bIsRunning;
        bool bForward;
    
        // PREVIEW MODE
        bool bPreviewMode;
        float previewAnimSpeed;
        float previewNextFrame;
    
        vector<ofImage> previewFrames;
        vector<ofImage>::iterator previewIterator;
    
    
        // CAPTURE MODE STUFF
        float startTime;
        ofDirectory sourceImageDir;
        string sourceImageName;
        string outputDir;
        ofxUITextInput* outputDirInput;
    
    
        // SOURCE IMAGE STUFF
        ofFbo sourceImagePreview;
        ofImage sourceImage;
        ofShader adjust;
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
    
        bool bDrawCalibration;
        void guiEvent(ofxUIEventArgs &e);
        float drawY;            // Y Position for the laser to draw at
        float sampleY;         // Y position to sample from
    
        float swingY;
        float swingSpeed;
    
        float saturation;
        float brightness;
    
    
        float scanSpeed;
        float redMin, redMax;
        float greenMin, greenMax;
        float blueMin, blueMax;
        ofxEdsdk::Camera camera;

        ofFloatColor stripeColor;
        float stripeWidth;
        float stripeGap;
        ofxIlda::Frame stripes;
    
        ofPoint pos;
        ofFloatColor color;
};


