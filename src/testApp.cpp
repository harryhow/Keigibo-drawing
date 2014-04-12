#include "testApp.h"




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

}

//--------------------------------------------------------------
void testApp::update(){
    
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
    
    for (int i = 0; i < line.getVertices().size(); i++){
        ofPoint fromCenter = line.getVertices()[i] - ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        ofMatrix4x4 rotateMatrix;
        rotateMatrix.makeRotationMatrix(1, rotateX, rotateY, rotateZ);
        ofPoint rot = fromCenter * rotateMatrix + ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        line.getVertices()[i] = rot;
    }

}

//--------------------------------------------------------------
void testApp::draw(){

    float widthSmooth = 10;
    float angleSmooth;
    
    vector < float > transparency;
    vector < ofMesh > meshes;
    
    
    for (int i = 0; i < 50; i++){
        ofMesh line;
        line.setMode(OF_PRIMITIVE_LINE_STRIP);
        meshes.push_back(line);
        transparency.push_back(  abs(ofSignedNoise(i / 10.0)) );
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
    line.addVertex(ofPoint(x,y));
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