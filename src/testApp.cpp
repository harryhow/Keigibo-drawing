#include "testApp.h"
#include <sys/timeb.h>
#include <string.h>




bool    gotFirstDataByte = false;
bool    gotFirstA = false;
bool    gotFirstQ = false;
bool    gotFirstCloud = false;
int     aligned = 0;
int     serialCount = 0;
char*   teapotPacket;
float*  q;
float*  q_tmp;

float*  gravity;
float*  prev_gravity;
float*  diff;
float*  euler;
float*  ypr;
int     interval;
ofQuaternion    quaternion;
float           angleQua;
ofVec3f         vecQua;
ofPoint p;
double  x, y, z = 0; //
float   param1, param2, param3, paramAngle;
float   p_prev1, p_prev2, p_prev3;
int     x_rboundry, x_lboundry, y_rboundry, y_lboundry;
int     width_to_translate, height_to_translate;
float   zDiff;
float   yDiff;
float   xDiff;
bool    isShock = false;
bool    debug_trigger = false;
MyThread *thread;
float r_now = 0.8;
float g_now = 0.0;
float b_now = 0.6;
int isShockCount = 0;


////////////////////////////////////////
// helper function
int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int request_id_gravity_x = 0;
int request_id_gravity_y = 0;
int request_id_gravity_z = 0;


void testApp::urlResponse(ofHttpResponse & r) {
    cout <<  "GOT urlResponse"<< " " << r.status <<" id: " << r.request.getID() << endl;
    if (r.status==200 && r.request.getID() == request_id_gravity_x) {
//        jsonResponse.save("tmpresponse");
//        jsonResponse.openLocal("tmpresponse");
        jsonResponse.openBuffer(r.data);
        gravity_x_cloud[g_index] = jsonResponse["result"].asDouble();
        cout <<  "GOT X="<< " " << gravity_x_cloud[g_index] << endl;
        gotXResponse = true;
    } else {
        cout << r.status << " " << r.error << endl;
        if (r.status != -1) gotXResponse = false;
    }
    
    if (r.status==200 && r.request.getID() == request_id_gravity_y) {
        //        jsonResponse.save("tmpresponse");
        //        jsonResponse.openLocal("tmpresponse");
        jsonResponse.openBuffer(r.data);
        gravity_y_cloud[g_index] = jsonResponse["result"].asDouble();
        cout <<  "GOT Y="<< " " << gravity_y_cloud[g_index] << endl;
        gotYResponse = true;
    } else {
        cout << r.status << " " << r.error << endl;
        if (r.status != -1) gotYResponse = false;
    }

    
    if (r.status==200 && r.request.getID() == request_id_gravity_z) {
        //        jsonResponse.save("tmpresponse");
        //        jsonResponse.openLocal("tmpresponse");
        jsonResponse.openBuffer(r.data);
        gravity_z_cloud[g_index] = jsonResponse["result"].asDouble();
        cout <<  "GOT Z="<< " " << gravity_z_cloud[g_index] << endl;
        gotZResponse = true;
    } else {
        cout << r.status << " " << r.error << endl;
        if (r.status != -1) gotZResponse = false;
    }
    
//    g_index++;
//    if (g_index < 1024)
//        g_index++;
//    else
//        g_index = 0;
}



void testApp::checkCloudAsync() {
    // gravity x
    if (gotXResponse){
        std::string url = sparkURIDevice + "/" + coreID + "/" + "gravity_x" + "?" + "access_token=" + token;
        request_id_gravity_x = ofLoadURLAsync(url);
        cout << "return X ID: " << request_id_gravity_x << endl;
        gotXResponse = false;
    }
    
    if (gotYResponse){
        std::string url = sparkURIDevice + "/" + coreID + "/" + "gravity_y" + "?" + "access_token=" + token;
        request_id_gravity_y = ofLoadURLAsync(url);
        cout << "return Y ID: " << request_id_gravity_x << endl;
        gotYResponse = false;
    }
    
    if (gotZResponse){
        std::string url = sparkURIDevice + "/" + coreID + "/" + "gravity_z" + "?" + "access_token=" + token;
        request_id_gravity_z = ofLoadURLAsync(url);
        cout << "return Z ID: " << request_id_gravity_x<< endl;
        gotZResponse = false;
    }
    
}


////////////////////////////////////////////////////////////////////////
#ifndef USE_THREAD_SPARK_CLOUD
void testApp::checkCloud() {

    // setup Spark Cloud
    // core id: 48ff70065067555049362287
    // token: 63b087f7d2b2e7d07bac1c75b656bd9e8d9f024c
    // https://api.spark.io/v1/devices/48ff70065067555049362287/quat?access_token=63b087f7d2b2e7d07bac1c75b656bd9e8d9f024c
    
    
    // gravity x
    std::string url = sparkURIDevice + "/" + coreID + "/" + "gravity_x" + "?" + "access_token=" + token;
    int id = ofLoadURLAsync(url, "gravity_x");

#if 0
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
    //cout << "gravity from cloud x,y,z: " << gravity_x_cloud[g_index] << ", " << gravity_y_cloud[g_index] << ", " << gravity_z_cloud[g_index] << ", " << gravity_z_cloud[g_index] << "\n";
    
    ///////////////////////////////////////////////////////////////////////////////////
    // quat w
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

    //cout << "quat w,x,y,z from cloud: " << quat_w_cloud[g_index] << ", " << quat_x_cloud[g_index] << ", " << quat_y_cloud[g_index] << ", " << quat_z_cloud[g_index] << "\n";
#endif
    
    gotFirstCloud = true;
    
//    if (g_index < 1024)
//        g_index++;
//    else
//        g_index = 0;
    
}
#endif



//--------------------------------------------------------------
void testApp::setup(){

    ofSetVerticalSync(true);
    ofBackground(255,255,255);
    ofSetFrameRate(60);
    
    // init data
    p_prev1, p_prev2, p_prev3 = 0;
    quaternion = ofQuaternion(1.0, 1.0, 1.0, 1.0);
    interval = getMilliCount();
    gotXResponse = true;
    gotYResponse = true;
    gotZResponse = true;
    
    gravity = new float[3];
    prev_gravity = new float[3];
    diff = new float[3];
    ypr = new float[3];
    q = new float[4];
    
    //memset(prev_gravity, 0 , sizeof(prev_gravity));
    
    x = ofGetWidth()/2;
    y = ofGetHeight()/2;
    z = 0.0;

    
    //ofSetFrameRate(32);
    cout << "frame rate: " << ofGetFrameRate() << "\r";
    
    //setup texture
    //we need to call this for textures to work on models
    ofDisableArbTex();
	
	//this makes sure that the back of the model doesn't show through the front
	ofEnableDepthTest();
    
#ifdef USE_SERIAL_DATA_INPUT
    //// setup serial port
    
    serial.setup("/dev/tty.usbmodem1421", 115200); //because I setup 115200 at driver
    //ofAddListener(serial.NEW_MESSAGE,this,&testApp::onNewMessageMPU6050);
	message = "";
    requestRead = true;
    
    // init token for hardware, might modify driver to elimilate this trigger
    serial.writeByte('r');
    serial.writeByte('r');
    serial.writeByte('r');
    serial.startContinuousRead();
#endif
    
#ifdef USE_OSC_CONTROL
    // listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);
	current_msg_string = 0;
#endif
    
    rgbaFbo.allocate(400, 400, GL_RGBA);
    rgbaFbo.begin();
	ofClear(255,255,255, 0);
    rgbaFbo.end();
  
#ifdef USE_THREAD_SPARK_CLOUD
    thread = new MyThread();
    // start the thread
//    thread->startThread(false, true);    // non-blocking, verbose
#endif
    
    ofRegisterURLNotification(this);
    

    // setup dot points effect
    ofLoadImage(texture, "dot.png");
	
	// set the camera distance
	camDist  = 1605;
	camera.setDistance(camDist);

#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/shader");
#else
    shader.load("shaders/shader");
#endif
    //skybox
    skybox.load();
    
}

#ifdef USE_SERIAL_DATA_INPUT
void testApp::onNewMessageMPU6050(string & message)
{
    
    // for getting OUTPUT_READABLE_QUATERNION and OUTPUT_READABLE_WORLDACCEL from MPU6050
    //cout << "++message: " << message << "\n" << "--message: "<< "\n";
    
    char ch;
    
    vector <string> aworldResult;
    vector <string> qResult;

    
    if (ofIsStringInString(message, "aworld")){
        aworldResult = ofSplitString(message, "\t");
        if (!aworldResult[0].compare("aworld")){
            if (aworldResult.size() == 4){
                param1 = ofToFloat(aworldResult[1]);
                param2 = ofToFloat(aworldResult[2]);
                param3 = ofToFloat(aworldResult[3]);
                gotFirstA = true;
                cout << "aworld: " << aworldResult[1] << ", " << aworldResult[2] << ", " << aworldResult[3] << "\n";
            }
            
        }
    }
    
    if (ofIsStringInString(message, "quat")){
        qResult = ofSplitString(message, "\t");
        if (!qResult[0].compare("quat")){
            if (qResult.size() == 5){
                cout << "Q: " << qResult[1] << ", " << qResult[2] << ", " << qResult[3] << ", " << qResult[4] << "\n";
                //quaternion.set(ofQuaternion(ofToFloat(qResult[2]), ofToFloat(qResult[3]), ofToFloat(qResult[3]), ofToFloat(qResult[1]))); // x,y,z,w
                q = new float[4];
                //memset(q, 0 , sizeof(q));
                q[0] = ofToFloat(qResult[1]); //w
                q[1] = ofToFloat(qResult[2]); //x
                q[2] = ofToFloat(qResult[3]); //y
                q[3] = ofToFloat(qResult[4]); //z
                gotFirstQ = true;
                cout << "\n\n" << "UPDATE quaternion" << "\n";

            }
        }
    }
}
#endif


#if 0
void testApp::onNewMessage(string & message)
{
//	cout << "onNewMessage, message: " << msg << "\n";
//
      char ch;
//    message+=msg;
    
    cout << "++message: " << message << "\n" << "--message: "<< "\n";
  
    
    for (int i=0; i < message.size(); i++){
        //cout << message[i] << "\n";
        ch = message[i];
        if(!gotFirstDataByte) {
            if (ch == '$') {
                cout << "GOT" << message[i] << "\n";
                gotFirstDataByte = true;
            } else {
                return;
            }
        }
        
        
        if (aligned < 4) {
            // make sure we are properly aligned on a 14-byte packet
            if (serialCount == 0) {
                if (ch == '$') aligned++; else aligned = 0;
            } else if (serialCount == 1) {
                if (ch == 2) aligned++; else aligned = 0;
            } else if (serialCount == 12) {
                if (ch == '\r') aligned++; else aligned = 0;
            } else if (serialCount == 13) {
                if (ch == '\n') aligned++; else aligned = 0;
            }
            //println((char)ch + " " + aligned + " " + serialCount);
            cout << "ALIGNED: " << (char)ch << " " << aligned << " " << serialCount << "\n";
            serialCount++;
            if (serialCount == 14) serialCount = 0;
        } else {
            if (serialCount > 0 || ch == '$') {
                
                if (!teapotPacket){
                    cout << "NEW PACKET" << "\r";
                    teapotPacket = new char[14];
                    //memset(teapotPacket, 0 , sizeof(teapotPacket));

                }
                
                teapotPacket[serialCount++] = (char)ch;
               
                if (serialCount == 14) {
                    serialCount = 0; // restart packet byte position
                    
//                    if (q){
//                        delete[] q;
//                    }
//                    
//                    q = new float[4];
                    memset(q, 0 , sizeof(q));
                    cout << "teapotpacket=" << (teapotPacket[4] << 8) << "\n";
                    q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0;
                    q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0;
                    q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0;
                    q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0;
                    
                    
//                    q = new float[4];
//                    memset(q, 0 , sizeof(q));
//                    // fit format as x,y,z,w
//                    q[3] = q_tmp[0];
//                    q[0] = q_tmp[1];
//                    q[1] = q_tmp[2];
//                    q[2] = q_tmp[3];
//                    
                    
                    // swap 0 and 3 to fit format
//                    float* q_tmp = new float[1];
//                    q_tmp[0] = q[0];
//                    q[0] = q[3];
//                    q[3] = q_tmp[0];
                    
                    //  q[3] <- q[0]
                    //  q[0] <- q[1]
                    //  q[1] <- q[2]
                    //  q[2] <- q[3]
                    
                    for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
                    
                    // set our toxilibs quaternion to new data
                    //quat.set(q[0], q[1], q[2], q[3]);
                    //quaternion = ofQuaternion(q[1], q[2], q[3], q[0]);
                    
                    cout << "q:\t" << (q[0]*100.0)/100.0 << " " << (q[1]*100.0)/100.0 << " " << (q[2]*100.0)/100.0 << " " << (q[2]*100.0)/100.0 << "\r";
                    
                    quaternion.set(q[1], q[2], q[3], q[0]); // x,y,z,w
                    cout << "\n\n" << "UPDATE quaternion" << "\n";
                    gotFirstQ = true;
                    
                    
                    /*
                    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
                    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
                    // different coordinate system orientation assumptions between Processing
                    // and InvenSense DMP)
                    float[] axis = quat.toAxisAngle();
                    rotate(axis[0], -axis[1], axis[3], axis[2]);
                    */
                    
                    
                    // update angle and x,y,z vector from quaternion
                    
                    quaternion.getRotate(angleQua, vecQua);
                    
                    cout << "UPDATE rotation: angle=" << angleQua <<" "<< "x,y,z= " << vecQua.x <<" " << vecQua.y <<" " << vecQua.z << "\n";
                    
                    //gravity = new float[3];
                    // calculate gravity vector
                    
                    
                    gravity[0] = 2 * (q[1]*q[3] - q[0]*q[2]);
                    gravity[1] = 2 * (q[0]*q[1] + q[2]*q[3]);
                    gravity[2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
                    cout << "UPDATE gravity: " << gravity[0] <<" " << gravity[1] <<" "<< gravity[2] << "\n";
                    
                    //ypr = new float[3];
                    // calculate yaw/pitch/roll angles
                    ypr[0] = atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1);
                    ypr[1] = atan(gravity[0] / sqrt(gravity[1]*gravity[1] + gravity[2]*gravity[2]));
                    ypr[2] = atan(gravity[1] / sqrt(gravity[0]*gravity[0] + gravity[2]*gravity[2]));
                    cout << "UPDATE ypr: "<< ypr[0] <<" " << ypr[1] <<" "<< ypr[2] << "\n";
                    
                    if (teapotPacket){
                        cout << "delete ...." << "\n";
                        //delete[] teapotPacket;
                        memset(teapotPacket, 0 , sizeof(teapotPacket));
                    }
                    
                }
            }
        }
    }
}
#endif



int tj_i = 0;
void testApp::drawTrajectory(){
    
   
    //gotFirstCloud = true;
    
    ///////////////////////////////////////////////////////////////
    // draw lines here
    ///////////////////////////////////////////////////////////////
    
    
    float rotateX = 0;
    float rotateY = 0;
    float rotateZ = 0;
    
    
    
    //harry: control by your accelerometer
    if (ofGetKeyPressed('a')) rotateX = 1;
    if (ofGetKeyPressed('s')) rotateX = -1;
    if (ofGetKeyPressed('z')) rotateY = 1;
    if (ofGetKeyPressed('x')) rotateY = -1;
    if (ofGetKeyPressed('q')) rotateZ = 1;
    if (ofGetKeyPressed('w')) rotateZ = -1;
    if (ofGetKeyPressed('C')) line.clear();

    
    
    //cout << "1111111111111"<< "\r";
    
    gotFirstQ = true;
    
    if (!gotFirstQ){
        x = ofGetWidth()/2;
        y = ofGetHeight()/2;
        z = 0.0;
        
    }
    else{
        
        gravity[0] = 2 * (q[1]*q[3] - q[0]*q[2]);
        gravity[1] = 2 * (q[0]*q[1] + q[2]*q[3]);
        gravity[2] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
        //cout << "UPDATE gravity: " << gravity[0] <<" " << gravity[1] <<" "<< gravity[2] << "\n";
        

        //
//        param1 = gravity[0];
//        param2 = gravity[1];
//        param3 = gravity[2];

#ifdef USE_THREAD_SPARK_CLOUD

        // from the cloud
        tj_i = 0; // because of thread starts every time
        param1 = thread->gravity_x_cloud[tj_i];
        param2 = thread->gravity_y_cloud[tj_i];
        param3 = thread->gravity_z_cloud[tj_i];
#else
        
        // from the cloud
        tj_i = 0;
        param1 = gravity_x_cloud[tj_i];
        param2 = gravity_y_cloud[tj_i];
        param3 = gravity_z_cloud[tj_i];
#endif
        
        cout << "gravity cloud: " << param1 <<", " << param2 << ", " << param3 << "\r";

        
//        if (!gotFirstA)
//            return;
//        
        xDiff = param1 + p_prev1;
        yDiff = param2 + p_prev2;
        zDiff = param3 + p_prev3;
        
        cout << "Diff: " << xDiff <<", " << yDiff << ", " << zDiff << "\r";
        
        if (xDiff > 2)
        {
            rotateX = 1;
            cout << "SHOCK" << "\r";
            //lets draw some graphics into our two fbos
//            rgbaFbo.begin();
//            drawFboTest();
//            rgbaFbo.end();
            isShock = true;
        }
        
        if (yDiff > 2)
        {
            rotateY = 1;
            cout << "SHOCK" << "\r";
            isShock = true;
        }
        
        if (zDiff > 2)
        {
            rotateZ = 1;
            cout << "SHOCK" << "\r";
            isShock = true;
        }
        
        
        x += xDiff*10;
        y += yDiff*10;
        z += zDiff*10;
        
    
        
        p_prev1 = param1;
        p_prev2 = param2;
        p_prev3 = param3;
        
        //cout << "XYZ: " << x <<", " << y << ", " << z << "\r";
        
        
    if (gotFirstQ || gotFirstA || gotFirstCloud/*thread->gotFirstCloud*/){
        cout << "add vertex: " << x <<", " << y << ", " << z << "\r";
        line.addVertex(x,y);
    }
    
    for (int i = 0; i < line.getVertices().size(); i++){
        ofPoint fromCenter = line.getVertices()[i] - ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        ofMatrix4x4 rotateMatrix;
        rotateMatrix.makeRotationMatrix(1, rotateX, rotateY, rotateZ);
        ofPoint rot = fromCenter * rotateMatrix + ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        line.getVertices()[i] = rot;
    }
    
    }
    if (tj_i < 1024){
        tj_i++;
    } else {
        tj_i = 0;
    }
    
#ifdef USE_THREAD_SPARK_CORE
    thread->gotFirstCloud = false;
#else
    gotFirstCloud = false;
#endif
}

//--------------------------------------------------------------
void testApp::update(){
    
#ifdef USE_THREAD_SPARK_CLOUD
    
    
    // harry: thread handling!!!!ah...
    
    if (thread->isThreadRunning()){
        if (thread->gotFirstCloud){
            thread->waitForThread(true);
            return;

        }
    }
    else{
        while(thread->isThreadRunning() == false){
            thread = new MyThread;
            thread->startThread(true, true);
            return;
        }
        
    }
    
//    
//    if (!thread->isThreadRunning()){
//        // start the thread
//        
//        thread->startThread(true, true);    // blocking, verbose
//    } else{
////        if (thread->gotFirstCloud){
//            thread->stopThread();
////            delete thread;
////            thread = NULL;
////        }
//    }
    
//    if (!thread->isThreadRunning()){
//        thread->startThread(true, true);
//    }
//    else
//        thread->waitForThread(true);

#else
    //checkCloud();
    checkCloudAsync();
#endif
    
    drawTrajectory();
    return;
    

#ifdef USE_OSC_CONTROL
    // hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}
    
    // check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
		// check for mouse moved message
		if(m.getAddress() == "/gravity"){
			// both the arguments are int32's
			param1 = m.getArgAsFloat(0);
			param2 = m.getArgAsFloat(1); // make sure data type
            param3 = m.getArgAsFloat(2);
            gotFirstQ = true;
            
            cout << "get param: " << param1 << "," << param2 << "," << param3 << "\r";
           
		}
        else if(m.getAddress() == "/angle"){
            paramAngle = m.getArgAsFloat(0);
            cout << "get angle: " << paramAngle << "\r";
        }
		// check for mouse button message
		else if(m.getAddress() == "/test"){
			// the single argument is a string
			param1 = m.getArgAsFloat(0);
            cout << "get test param: " << param1 << "\r";

		}
		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

        ///////////////////////////////////////////////////////////////
        
        
        float rotateX = 0;
        float rotateY = 0;
        float rotateZ = 0;
        
        
        //harry: control by your accelerometer
        if (ofGetKeyPressed('a')) rotateX = 1;
        if (ofGetKeyPressed('s')) rotateX = -1;
        if (ofGetKeyPressed('z')) rotateY = 1;
        if (ofGetKeyPressed('x')) rotateY = -1;
        if (ofGetKeyPressed('q')) rotateZ = 1;
        if (ofGetKeyPressed('w')) rotateZ = -1;
        if (ofGetKeyPressed('C')) line.clear();
        
        //(axis order [1, 3, 2] and inversion [-1, +1, +1] is
        //////////////////////////////////////////////////////
        // draw lines here
        
        if (!gotFirstQ){
            x = ofGetWidth()/2;
            y = ofGetHeight()/2;
            z = 0.0;
            
        }
        else{
            
            //        diff[0] = prev_gravity[0] - gravity[0];
            //        diff[1] = prev_gravity[1] - gravity[1];
            //        diff[2] = prev_gravity[2] - gravity[2];
            //
            
            float xDiff = param1 + p_prev1;
            float yDiff = param2 + p_prev2;
            float zDiff = param3 + p_prev3;
            
            cout << "Diff: " << xDiff <<", " << yDiff << ", " << zDiff << "\r";
            
            x += xDiff*10;
            y += yDiff*10;
            z += zDiff*10;
            
            p_prev1 = param1;
            p_prev2 = param2;
            p_prev3 = param3;
            
            cout << "XYZ: " << x <<", " << y << ", " << z << "\r";
            
            //
            //        prev_gravity[0] = gravity[0];
            //        prev_gravity[1] = gravity[1];
            //        prev_gravity[2] = gravity[2];
        }
        
//        if (x > ofGetWidth() || x < 0) {
//            if (y > ofGetHeight() || y < 0)
//                return;
//        }
        
        if (gotFirstQ){
            cout << "add vertex: " << x <<", " << y << ", " << z << "\r";
            line.addVertex(x,y);
        }
        
        for (int i = 0; i < line.getVertices().size(); i++){
            ofPoint fromCenter = line.getVertices()[i] - ofPoint(ofGetWidth()/2, ofGetHeight()/2);
            ofMatrix4x4 rotateMatrix;
            rotateMatrix.makeRotationMatrix(1, rotateX, rotateY, rotateZ);
            ofPoint rot = fromCenter * rotateMatrix + ofPoint(ofGetWidth()/2, ofGetHeight()/2);
            line.getVertices()[i] = rot;
        }
        
	}
#endif
    
    
// pick what value for rotation?
//    rotateX = ofMap(vecQua.x, -1.0, 1.0, -1.0, 1.0);
//    rotateY = ofMap(vecQua.y, -1.0, 1.0, -1.0, 1.0);
//    rotateZ = ofMap(vecQua.z, -1.0, 1.0, -1.0, 1.0);
    
    
//    rotateX = ofMap(ypr[0], -1.0, 1.0, -1.0, 1.0);
//    rotateY = ofMap(ypr[1], -1.0, 1.0, -1.0, 1.0);
//    rotateZ = ofMap(ypr[2], -1.0, 1.0, -1.0, 1.0);
    
//    rotateX = ypr[0];
//    rotateY = ypr[1];
//    rotateZ = ypr[2];
    
   // cout << "point add: " << vecQua.x*-1000 << " " << vecQua.z*1000 << "\r";
    //cout << "rotate: " << rotateX << " " << rotateY << " " << rotateZ << "\r";
    

}
//--------------------------------------------------------------
void testApp::addPoint(float x, float y, float z) {
	ofVec3f p(x, y, z);
	points.push_back(p);
	
	// we are passing the size in as a normal x position
	float size = ofRandom(0, yDiff*10);
	sizes.push_back(ofVec3f(size));
}

void testApp::drawDot() {
    
    glDepthMask(GL_FALSE);
	
	ofSetColor(255, 83, 13, 100);
	
	// this makes everything look glowy :)
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();
	
	// bind the shader and camera
	// everything inside this function
	// will be effected by the shader/camera
	shader.begin();
	camera.begin();
	
	// bind the texture so that when all the points
	// are drawn they are replace with our dot image
	texture.bind();
	vbo.draw(GL_POINTS, 0, (int)points.size());
	texture.unbind();
	
	camera.end();
	shader.end();
	
	ofDisablePointSprites();
	ofDisableBlendMode();
	
	// check to see if the points are
	// sizing to the right size
	ofEnableAlphaBlending();
	camera.begin();
	for (unsigned int i=0; i<points.size(); i++) {
		ofSetColor(255, 80);
		ofVec3f mid = points[i];
		mid.normalize();
		mid *= 300;
		ofLine(points[i], mid);
        ofEllipse(points[i], yDiff*10, yDiff*10);
        
	}
	camera.end();
	
	glDepthMask(GL_TRUE);
    isShock = false;


}


//--------------------------------------------------------------
void testApp::draw(){
    float widthSmooth = 10;
    float angleSmooth;
    
    vector < float > transparency;
    vector < ofMesh > meshes;
    
    
//    camera.begin();
//    sphereCenter = ofVec3f(0, 0, 0);
//    camera.setPosition(ofVec3f(0, 0, 0));
//    skybox.draw();
//    
//    camera.lookAt(ofVec3f(x,  y, z), ofVec3f(0, 1, 0));
//    camera.end();

    
    // trigger DMP init/start in case the MPS is halted or rest while running
    // doesn't work?
/*
    if (getMilliCount() - interval > 5000) {
        cout << "wakeup trigger ... "<< "\r";
        // resend single character to trigger DMP init/start
        // in case the MPU is halted/reset while applet is running
        serial.writeByte('r');
        serial.writeByte('r');
        serial.writeByte('r');
        interval = getMilliCount();
    }
*/
    
    //ofRotate(angleQua, vecQua.x, vecQua.y, vecQua.z);
    
    for (int i = 0; i < 80; i++){
        ofMesh line;
        line.setMode(OF_PRIMITIVE_LINE_STRIP);
        meshes.push_back(line);
        transparency.push_back(abs(ofSignedNoise(i / 10.0)));
    }
    
    
    ofEnableAlphaBlending();
    
    for (int i = 0;  i < line.getVertices().size(); i++){
        
        
        int me_m_one = i-1;
        int me_p_one = i+1;
        if (me_m_one < 0) me_m_one = 0;
        if (me_p_one ==  line.getVertices().size()) me_p_one =  line.getVertices().size()-1;
        
        ofPoint diff = line.getVertices()[me_p_one] - line.getVertices()[me_m_one];
        

        float angle = atan2(diff.y, diff.x);
        
        if (i == 0) angleSmooth = angle;
        else {
            
            float diffAngle = angle - angleSmooth;
            while (diffAngle < -PI) diffAngle += TWO_PI ;
            while (diffAngle > PI) diffAngle -= TWO_PI ;
            
            angleSmooth += 0.1f * diffAngle;
            
        }
        
        float dist = diff.length();
        
        float w = ofMap(dist, 0, 20, 1, 80+100, true);
        
        widthSmooth = 0.9f * widthSmooth + 0.1f * w;
        
        ofPoint offset;
        offset.x = cos(angleSmooth + PI/2) * widthSmooth;
        offset.y = sin(angleSmooth + PI/2) * widthSmooth;
        
        ofSetColor(0,0,0);
        
        // ofLine (line.getVertices()[i] +  offset , line.getVertices()[i] -  offset );
        
        for (int j = 0; j < meshes.size(); j++){
            
            float pctAwayFromCenter = abs(j - meshes.size() / 2.0) / (meshes.size()/2);
            pctAwayFromCenter = powf(pctAwayFromCenter, 3.0);
            float pct = (float) j / (float) meshes.size();
            
            ofPoint newPt = (pct) * (line.getVertices()[i] +  offset) + (1 - pct) * (line.getVertices()[i] -  offset);
            
            meshes[j].enableColors();
            meshes[j].enableIndices();
            // harry: add color to mesh to let OF automatically generating color?
            // http://openframeworks.cc/tutorials/graphics/generativemesh.html
            
            // purple: ofFloatColor(0.8, 0.0, 0.6, 0)
            //
            
            
            
            if (isShock){
                
                r_now = ofRandom(0.0, 0.0);
                g_now = ofRandom(0.0, 0.0);
                b_now = ofRandom(0.0, 0.8);
                isShock = false;
                isShockCount ++;
            }
            
         
            
            meshes[j].addVertex(newPt);
            if (j>1){
//                meshes[j-1].enableColors();
//                meshes[j-1].enableIndices();
                meshes[j-1].addColor(ofFloatColor(r_now, g_now, b_now, transparency[j-1]*100 * (1-pctAwayFromCenter)));
                
//                meshes[j-2].enableColors();
//                meshes[j-2].enableIndices();
                meshes[j-2].addColor(ofFloatColor(r_now-0.2, g_now+0.7, b_now+0.2, transparency[j-2]*100 * (1-pctAwayFromCenter)));
                
            }
        }
    }
    
    //rgbaFbo.draw(0,0);
   
    
    //////////////////
    ofPushMatrix();
    
    ofTranslate(-x+ ofGetWidth()/2, -y + ofGetHeight()/2);
    
    // load the texure
	ofDisableArbTex();
	ofLoadImage(texture, "dot.png");
	
	// set the camera distance
	camDist  = 0;
	camera.setDistance(camDist);
//    
    

    for (int i = 0; i < meshes.size(); i++){
        meshes[i].addColor(ofFloatColor(1.0, 0.0, 0.0));
        meshes[i].draw();
    }


    if (isShock){
        
        // draw a random dot
//        float theta1 = ofRandom(0, TWO_PI);
//		float theta2 = ofRandom(0, TWO_PI);
//		ofVec3f p;
//		p.x = cos( theta1 ) * cos( theta2 );
//		p.y = sin( theta1 );
//		p.z = cos( theta1 ) * sin( theta2 );
//		p  *= 800;
//		addPoint(p.x, p.y, p.z);
//		int total = (int)points.size();
//		vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
//		vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);

        //drawDot();
//        camera.begin();
//        skybox.draw();
//        
//        camera.lookAt(ofVec3f(ofRandom(ofGetWidth()),  0,ofRandom(ofGetHeight())), ofVec3f(0, 1, 0));
//        camera.end();
    }

    ofPopMatrix();
    
    ///////////////
    if (debug_trigger){
        

        
        ofSetColor(255, 100);
        ofRect(0, 0, 250, 90);
        ofSetColor(0);
        string info = "FPS "+ofToString(ofGetFrameRate(), 0) + "\n";
        info += "Shock num "+ofToString(isShockCount)+"\n";
        ofDrawBitmapString(info, 20, 20);
    }
    
}

void testApp::exit(){
#ifdef USE_THREAD_SPARK_CLOUD
    thread->stopThread();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    if (key == 'f') ofToggleFullscreen();
    if (key == 'd') debug_trigger = !debug_trigger;

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    // suppy vertex by acceleration data
    //cout << "dragged point: " << x << "," << y << "\n";
    //line.addVertex(ofPoint(x,y));
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    //line.clear();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}