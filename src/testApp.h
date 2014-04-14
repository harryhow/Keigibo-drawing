#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSimpleSerial.h"


using namespace std;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void onNewMessage(string & message);
    
    
        ofPolyline line;
        vector < ofPolyline > lines;

    
        //this is our model we'll draw
        ofxAssimpModelLoader model;
        ofLight light;
    
        // serial
        ofxSimpleSerial	serial;
        string		message;
        bool		remember;
        bool		requestRead;
		
};
