#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSimpleSerial.h"
#include "ofxOsc.h"

// listen on port 5001
#define PORT 12000
#define NUM_MSG_STRINGS 20


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
        //ofPolyline line2;
        vector < ofPolyline > lines;

    
        //this is our model we'll draw
        ofxAssimpModelLoader model;
        ofLight light;
    
        // serial
        //ofxSimpleSerial	serial;
        string	message;
       
        bool		remember;
        bool		requestRead;
        char*       teapotPacket;
   		//char[] teapotPacket = new char[14];
    
        ofxOscReceiver receiver;
        int current_msg_string;
        string msg_strings[NUM_MSG_STRINGS];
        float timers[NUM_MSG_STRINGS];

};
