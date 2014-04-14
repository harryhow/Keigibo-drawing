#include "testApp.h"
//
//#include <GLkit/GLKMath.h>
//#include <GLKit/GLKQuaternion.h>
//#include <GLKit/GLKMathTypes.h>




bool gotFirstDataByte = false;
int aligned = 0;
int serialCount = 0;
char* teapotPacket;
float* q;
ofQuaternion quaternion;
float angleQua, xQua, yQua, zQua;



//--------------------------------------------------------------
void testApp::setup(){

    ofSetVerticalSync(true);
    ofBackground(0,0,0);
    
    //setup texture
    //we need to call this for textures to work on models
    ofDisableArbTex();
	
	//this makes sure that the back of the model doesn't show through the front
	ofEnableDepthTest();
    
//	//now we load our model
//	if (!model.loadModel("dog/dog.3ds"))
//        printf("failed to load model");
    // center(.5, .5)
	model.setPosition(ofGetWidth()* .5, ofGetHeight() *.5, 0);
	
	light.enable();
    
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
    
    teapotPacket = new char[14];
    q = new float[4];
    quaternion = ofQuaternion(1.0, 1.0, 1.0, 1.0);

}

void testApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
    
    char ch;
    
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
            cout << (char)ch << " " << aligned << " " << serialCount << "\n";
            serialCount++;
            if (serialCount == 14) serialCount = 0;
        } else {
            if (serialCount > 0 || ch == '$') {
                
                 teapotPacket[serialCount++] = (char)ch;
                
                if (serialCount == 14) {
                    serialCount = 0; // restart packet byte position
                    cout << "CALCULATE quaternion" << "\n";
                    
                    q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
                    q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
                    q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
                    q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
                    for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
                    
                    // set our toxilibs quaternion to new data
                    //quat.set(q[0], q[1], q[2], q[3]);
                    quaternion = ofQuaternion(q[0], q[1], q[2], q[3]);
                
                }
            }
        }

    }
    
    
   
	
//	red = (message == "r");
//	green = (message == "g");
//	blue = (message == "b");
}



//--------------------------------------------------------------
void testApp::update(){
    

//    if(requestRead)
//    {
//		serial.sendRequest();
//		requestRead = false;
//    }
    
    
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
    
    
    //ofMatrix4x4 rotateMatrix;
    //quaternion.getRotate(angleQua, xQua, yQua, zQua);
    //quaternion.get(rotateMatrix);
    //cout << "ANGLE:" <<  angleQua << " x:" << xQua << " y:" << yQua << " z:" << zQua << "\r";
    
    
    //ofPushMatrix();
    //ofRotate (angleQua, xQua, yQua, zQua);
    quaternion.getRotate(angleQua, xQua, yQua, zQua);
    line.addVertex(ofPoint(xQua,yQua));
    ofClamp(zQua, -1, 1);
    
    rotateX = xQua;
    rotateY = yQua;
    rotateZ = zQua;
    
    for (int i = 0; i < line.getVertices().size(); i++){
        ofPoint fromCenter = line.getVertices()[i] - ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        ofMatrix4x4 rotateMatrix;
        //quaternion.get(rotateMatrix);
        rotateMatrix.makeRotationMatrix(1, rotateX, rotateY, rotateZ);
        ofPoint rot = fromCenter * rotateMatrix + ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        line.getVertices()[i] = rot;
    }
    //ofPopMatrix();

}

//--------------------------------------------------------------
void testApp::draw(){

    float widthSmooth = 10;
    float angleSmooth;
    
    vector < float > transparency;
    vector < ofMesh > meshes;
    
    
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
                meshes[j-1].addColor(ofFloatColor(0.8, 0.7, 0.0, transparency[j]*100 * (1-pctAwayFromCenter)));
                meshes[j-2].addColor(ofFloatColor(0.2, 0.2, 0.0, transparency[j]*100 * (1-pctAwayFromCenter)));
            }
            
        }
    }
    
    for (int i = 0; i < meshes.size(); i++){
        meshes[i].addColor(ofFloatColor(1.0, 0.0, 0.0));
        meshes[i].draw();
    }
    

/*
    //////////////////////
    ofPushMatrix();
    ofMaterial material = model.getMaterialForMesh(0);

    ofTexture texture;
    ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
    bool bHasTexture = meshHelper.hasTexture();
//    printf("has texture %d", bHasTexture);
    if( bHasTexture ) {
        texture = model.getTextureForMesh(0);
    }

    
    //draw the model manually
    texture.bind();
    material.begin();

    
    //mesh.drawWireframe(); //you can draw wireframe too
	//mesh.drawFaces();
    for (int i = 0; i < meshes.size(); i++){
        meshes[i].draw();
    }

	material.end();
//	if(bHasTexture) texture.unbind();
                                    
    ofPopMatrix();
*/
}

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