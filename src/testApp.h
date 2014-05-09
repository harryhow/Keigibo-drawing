#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSimpleSerial.h"
#include "ofxOsc.h"
#include "ofxJSONElement.h"
#include "ofxSkyBox.h"

// listen on port 5001
#define PORT 12000

#define NUM_MSG_STRINGS 20
//#define USE_SERIAL_DATA_INPUT
//#define USE_OSC_CONTROL
//#define HARRYHOW_DEBUG
//#define USE_THREAD_SPARK_CLOUD


using namespace std;
class MyThread;



class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

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
        void onNewMessageMPU6050(string & message);
        void drawTrajectory();
        void checkCloud();
        void checkCloudAsync();
        //void drawFboTest();
        void drawDot();
        void addPoint(float x, float y, float z);
        void urlResponse(ofHttpResponse & response);
    
        ofPolyline line;
        //ofPolyline line2;
        vector < ofPolyline > lines;

    
        //this is our model we'll draw
        ofxAssimpModelLoader model;
        ofLight light;
    
        // serial
        ofxSimpleSerial	serial;
        string	message;
       
        bool		remember;
        bool		requestRead;
        char*       teapotPacket;
   		//char[] teapotPacket = new char[14];
    
        ofxOscReceiver receiver;
        int current_msg_string;
        string msg_strings[NUM_MSG_STRINGS];
        float timers[NUM_MSG_STRINGS];
    
    
        ofFbo rgbaFbo; // with alpha
        int fadeAmnt;
    
        // vector to store all values
        vector <ofVec3f> points;
        vector <ofVec3f> sizes;
        
        ofVbo vbo;
        ofShader shader;
        ofEasyCam camera;
        
        float camDist;
        
        ofTexture texture;
        ofxSkyBox   skybox;
        ofVec3f     sphereCenter;

    
#ifndef USE_THREAD_SPARK_CLOUD
    int g_index = 0;
    double gravity_x_cloud[1024];
    double gravity_y_cloud[1024];
    double gravity_z_cloud[1024];
    
    double quat_w_cloud[1024];
    double quat_x_cloud[1024];
    double quat_y_cloud[1024];
    double quat_z_cloud[1024];
    
    string token = "ceba2533e9c533ecd3e9f866fc1028cadf45b52e";
    string coreID = "48ff70065067555049362287";
    string sparkURIDevice = "https://api.spark.io/v1/devices";
    
    bool gotFirstCloud;
    ofxJSONElement  jsonResponse;
    bool gotXResponse;
    bool gotYResponse;
    bool gotZResponse;
    

#endif
};



#ifdef USE_THREAD_SPARK_CLOUD

class MyThread : public ofThread, ofBaseApp {
    
    // the shared resource
public:
    int g_index = 0;
    double gravity_x_cloud[1024];
    double gravity_y_cloud[1024];
    double gravity_z_cloud[1024];
    
    double quat_w_cloud[1024];
    double quat_x_cloud[1024];
    double quat_y_cloud[1024];
    double quat_z_cloud[1024];
    
    string token = "ceba2533e9c533ecd3e9f866fc1028cadf45b52e";
    string coreID = "48ff70065067555049362287";
    string sparkURIDevice = "https://api.spark.io/v1/devices";
    
    bool gotFirstCloud;
    ofxJSONElement  response;
    
    
    // the thread function
    void threadedFunction() {
        
        // start
        
        
        while(isThreadRunning()) {
            
            cout  << "RUNNNNNNNN\n" << endl;

            
            //lock();
            // setup Spark Cloud
            // core id: 48ff70065067555049362287
            // token: 63b087f7d2b2e7d07bac1c75b656bd9e8d9f024c
            // https://api.spark.io/v1/devices/48ff70065067555049362287/quat?access_token=63b087f7d2b2e7d07bac1c75b656bd9e8d9f024c
            
            // gravity x
            std::string url = sparkURIDevice + "/" + coreID + "/" + "gravity_x" + "?" + "access_token=" + token;
            if (response.open(url)) {
                cout  << "Failed to parse gravity x\n" << endl;
            }
            gravity_x_cloud[g_index] = response["result"].asDouble();
            
            // gravity y
            url = sparkURIDevice + "/" + coreID + "/" + "gravity_y" + "?" + "access_token=" + token;
            //cout << "url: " << url << "\n";
            if (!response.open(url)) {
                cout  << "Failed to parse gravity y\n" << endl;
            }
            gravity_y_cloud[g_index] = response["result"].asDouble();
            
            // gravity z
            url = sparkURIDevice + "/" + coreID + "/" + "gravity_z" + "?" + "access_token=" + token;
            if (!response.open(url)) {
                cout  << "Failed to parse gravity z\n" << endl;
            }
            gravity_z_cloud[g_index] = response["result"].asDouble();
            cout << "gravity from cloud x,y,z: " << gravity_x_cloud[g_index] << ", " << gravity_y_cloud[g_index] << ", " << gravity_z_cloud[g_index] << ", " << gravity_z_cloud[g_index] << "\n";
            
            ///////////////////////////////////////////////////////////////////////////////////
            // quat wtestApp::
            url = sparkURIDevice + "/" + coreID + "/" + "quat_w" + "?" + "access_token=" + token;
            if (!response.open(url)) {
                cout  << "Failed to parse q w JSON\n" << endl;
            }
            quat_w_cloud[g_index] = response["result"].asDouble();
            
            
            // quat x
            url = sparkURIDevice + "/" + coreID + "/" + "quat_x" + "?" + "access_token=" + token;
            if (!response.open(url)) {
                cout  << "Failed to parse q x JSON\n" << endl;
            }
            quat_x_cloud[g_index] = response["result"].asDouble();
            
            // quat y
            url = sparkURIDevice + "/" + coreID + "/" + "quat_y" + "?" + "access_token=" + token;
            if (!response.open(url)) {
                cout  << "Failed to parse q y JSON\n" << endl;
            }
            quat_y_cloud[g_index] = response["result"].asDouble();
            
            
            // quat z
            url = sparkURIDevice + "/" + coreID + "/" + "quat_z" + "?" + "access_token=" + token;
            if (!response.open(url)) {
                cout  << "Failed to parse q z JSON\n" << endl;
            }
            quat_z_cloud[g_index] = response["result"].asDouble();
            
            cout << "quat w,x,y,z from cloud: " << quat_w_cloud[g_index] << ", " << quat_x_cloud[g_index] << ", " << quat_y_cloud[g_index] << ", " << quat_z_cloud[g_index] << "\n";
            
            
            gotFirstCloud = true;
            
            if (g_index < 1024)
                g_index++;
            else
                g_index = 0;
            
            //unlock();
            
        }
        
        // done
    }
    
};

#endif


