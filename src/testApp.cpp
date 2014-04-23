#include "testApp.h"
//
//#include <GLkit/GLKMath.h>
//#include <GLKit/GLKQuaternion.h>
//#include <GLKit/GLKMathTypes.h>
#include <sys/timeb.h>



bool gotFirstDataByte = false;
bool gotFirstQ = false;
int aligned = 0;
int serialCount = 0;
char* teapotPacket;
float* q;
float* q_tmp;


ofQuaternion quaternion;
float angleQua;
ofVec3f vecQua;

float* gravity;
float* prev_gravity;
float* diff;
float* euler;
float* ypr;
int interval;
ofPoint p;
float x, y, z; //
float param1, param2, param3;
float p_prev1, p_prev2, p_prev3;
int x_rboundry, x_lboundry, y_rboundry, y_lboundry;
int width_to_translate, height_to_translate;




////////////////////////////////////////
// helper function
int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}
/*
void tilt() {
    // translate from origin to center:
    ofTranslate( ofGetWidth()/2,  ofGetWidth()/2,  ofGetWidth()/2);
    // X is front-to-back:
    rotateX(radians(roll + 90));
    // Y is left-to-right:
    rotateY(radians(pitch) );
    // set the disc fill color:
    fill(#79BF3D);
    // draw the disc:
    ellipse(0, 0, width/4, width/4);
    // set the text fill color:
    fill(#20542E);
    // Draw some text so you can tell front from back:
    text(pitch + "," + roll, -40, 10, 1);
}
*/
//////////////////////////////////////


//--------------------------------------------------------------
void testApp::setup(){

    ofSetVerticalSync(true);
    ofBackground(0,0,0);
    
    ofSetFrameRate(30);
    cout << "frame rate: " << ofGetFrameRate() << "\r";
    
    //setup texture
    //we need to call this for textures to work on models
    ofDisableArbTex();
	
	//this makes sure that the back of the model doesn't show through the front
	ofEnableDepthTest();
    
//	//now we load our model
//	if (!model.loadModel("dog/dog.3ds"))
//        printf("failed to load model");
    // center(.5, .5)
	//model.setPosition(ofGetWidth()* .5, ofGetHeight() *.5, 0);
	
	//light.enable();
    
/*
//// setup serial port
    
    serial.setup("/dev/tty.usbmodem1421", 38400); //be ware of device path
    
    ofAddListener(serial.NEW_MESSAGE,this,&testApp::onNewMessage);
	message = "";
    requestRead = true;

    
    // init token for hardware, maybe not necessary?
    serial.writeByte('r');
    serial.writeByte('r');
    serial.writeByte('r');
    

	serial.startContinuousRead();

 */
    quaternion = ofQuaternion(1.0, 1.0, 1.0, 1.0);
    interval = getMilliCount();
    
    gravity = new float[3];
    prev_gravity = new float[3];
    diff = new float[3];
    ypr = new float[3];
    
    //memset(prev_gravity, 0 , sizeof(prev_gravity));
    
    x = ofGetWidth()/2;
    y = ofGetHeight()/2;
    z = 0.0;
    
    // listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);
    
	current_msg_string = 0;
    p_prev1, p_prev2, p_prev3 = 0;

}
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
                    
                    q = new float[4];
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



//--------------------------------------------------------------
void testApp::update(){
    
    
    //    if(requestRead)
    //    {
    //		serial.sendRequest();
    //		requestRead = false;
    //    }
    
    
    
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
void testApp::draw(){
    float widthSmooth = 10;
    float angleSmooth;
    
    vector < float > transparency;
    vector < ofMesh > meshes;
    
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
    
    for (int i = 0; i < 40; i++){
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
        
        float w = ofMap(dist, 0, 20, 1, 80, true);
        
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
            meshes[j].addVertex(newPt);
            if (j>1){
                meshes[j-1].addColor(ofFloatColor(0.8, 0.7, 0.6, transparency[j]*100 * (1-pctAwayFromCenter)));
                meshes[j-2].addColor(ofFloatColor(0.2, 0.2, 0.8, transparency[j]*100 * (1-pctAwayFromCenter)));
            }
            
        }
    }
    
    
    ofPushMatrix();
    
    
    ofTranslate(-x+ ofGetWidth()/2, -y + ofGetHeight()/2);

    for (int i = 0; i < meshes.size(); i++){
        meshes[i].addColor(ofFloatColor(1.0, 0.0, 0.0));
        meshes[i].draw();
    }
    
    ofPopMatrix();
}

/*
void::testApp::draw(){
    ///////////////////////////////////////////////
    ofBackground(0, 0, 0);
    ///ofQuaternion qt;    //  total quaternion
    
    ofPushMatrix();
    {
        ofSetLineWidth(10.0);
        
        //  x
        ofSetColor(255, 0, 0);
        ofLine(0.0, 0.0, ofGetWidth(), 0);
        
        //  y
        ofSetColor(0, 255, 0);
        ofLine(0.0, 0.0, 0.0, ofGetHeight());
        
        //  z
        ofSetColor(0, 0, 255);
        ofLine(0, 0, -30.0, 0, 0, 30.0);
    }
    ofPopMatrix();
    
    ofVec3f Znormal(0,0,1);
    ofVec3f Xnormal(1,0,0);
    ofVec3f Ynormal(0,1,0);
    
    if (!gotFirstQ)
        return;
    
    ofQuaternion qr = ofQuaternion(ypr[2], Znormal);
    ofQuaternion qp = ofQuaternion(ypr[1], Xnormal);
    ofQuaternion qy = ofQuaternion(ypr[0], Ynormal);
    ofQuaternion qt;    //  total quaternion
    
    qt = qr * qp * qy;
    
    ofSetLineWidth(1.0);
    ofSetColor(255);
    ofPushMatrix();
    {
        ofNoFill();
        
        //  トータルquaternionよりトータルな回転軸と回転量を取得
        ofVec3f qAxsis;
        float   angle;
        qt.getRotate(angle, qAxsis);
        
        //  そして回転
        ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
        ofDrawSphere(ofGetWidth()/2, ofGetHeight()/2, 0.0,  100.0);
        
    }
    ofPopMatrix();
    //////////////////////////////////////////////
}
*/
//--------------------------------------------------------------
void testApp::keyPressed(int key){

    if (key == 'f') ofToggleFullscreen();
    

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