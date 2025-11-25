//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"
#include "iomanip"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	glEnable(GL_PROGRAM_POINT_SIZE);
    bWireframe = false;
    bDisplayPoints = false;
    bAltKeyDown = false;
    bCtrlKeyDown = false;
    bLanderLoaded = false;
    bTerrainSelected = true;


    cam.setDistance(20);
    cam.setNearClip(.1);
    cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
    ofSetVerticalSync(true);
    cam.disableMouseInput();
    ofEnableSmoothing();
    ofEnableDepthTest();

	//setting particle size 
	glEnable(GL_PROGRAM_POINT_SIZE);

    // setup rudimentary lighting 
    initLightingAndMaterials();

    backgroundImage.load("geo/starfield.jpg");

    terrain.loadModel("geo/134final_v12.obj");
    terrain.setScaleNormalization(false);
	//terrain.setPosition(0, -30, 0);
	//terrain.setRotation(0, 180, 0, 1, 0);  

    
    lander.model.loadModel("geo/134Final_lander.obj");
	lander.model.setPosition(0, 40, 0);
	//lander.model.setRotation(0, 180, 0, 1, 0);
    bLanderLoaded = true;
    lander.model.setScaleNormalization(false);

    cout << "number of meshes: " << lander.model.getNumMeshes() << endl;
    bboxList.clear();
    for (int i = 0; i < lander.model.getMeshCount(); i++) {
        bboxList.push_back(Octree::meshBounds(lander.model.getMesh(i)));
    }

    // create sliders for testing
    gui.setup();
    bHide = false;

    //  Create Octree for testing.
    float startTime = ofGetElapsedTimeMillis();
    // octree.create(terrain.getMesh(0), 20);
	for(int i = 0; i<terrain.getNumMeshes(); i++){
		Octree temp;
		temp.create(terrain.getMesh(i),20);
		octrees.push_back(temp);
	}
	float endTime = ofGetElapsedTimeMillis();
    
    cout << "Number of Verts: " << terrain.getMesh(0).getNumVertices() << endl;
    cout << "Time to build the tree (ms): " << endTime - startTime << endl;

    testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

    // setup particle exhaust
    particleShader.load("particle.vert", "particle.frag");
    glm::vec3 landerPos = lander.model.getPosition();
    exhaustEmitter.position = landerPos - glm::vec3(0, 1, 0);
    exhaustEmitter.rate = 100;
    exhaustEmitter.velocity = glm::vec3(0, -5, 0);
    exhaustEmitter.oneShot = false;

	// setup explosion emitter
	explosionShader.load("explosionParticle.vert", "explosionParticle.frag");
	explosionEmitter.position = landerPos;
	explosionEmitter.rate = 100;
	explosionEmitter.velocity = glm::vec3(0, -1, 0);
	explosionEmitter.oneShot = false;

	//setup sounds
	thrustSound.load("sounds/thrustSound_v2.mp3");       
	thrustSound.setLoop(true);  
	thrustSound.setVolume(0.75);                 

	backgroundMusic.load("sounds/backgroundMusic.mp3");  
	backgroundMusic.setLoop(true);
	backgroundMusic.play();
	backgroundMusic.setVolume(0.3);

	explosionSound.load("sounds/explosionSound.wav"); 
	explosionSound.setLoop(false); 

	//Setting up lights
	ofSetGlobalAmbientColor(ofFloatColor(0.25, 0.25, 0.25, 1.0));
	// KEY LIGHT
	keyLight.setup();
	keyLight.enable();
	keyLight.setPointLight();
	keyLight.setPosition(40, 60, 40);
	keyLight.setDiffuseColor(ofFloatColor(1.0, 1.0, 1.0));      // normal brightness
	keyLight.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
	keyLight.setAmbientColor(ofFloatColor(0.25, 0.25, 0.25));
	keyLight.setAttenuation(0.2, 0.002, 0.0);
	// FILL LIGHT
	fillLight.setup();
	fillLight.enable();
	fillLight.setPointLight();
	fillLight.setPosition(-50, 35, 45);
	fillLight.setDiffuseColor(ofFloatColor(0.6, 0.6, 0.6));
	fillLight.setSpecularColor(ofFloatColor(0.4, 0.4, 0.4));
	fillLight.setAmbientColor(ofFloatColor(0.15, 0.15, 0.15));
	fillLight.setAttenuation(0.2, 0.002, 0.0);

	// RIM LIGHT
	rimLight.setup();
	rimLight.enable();
	rimLight.setPointLight();
	rimLight.setPosition(0, 70, -70);
	rimLight.setDiffuseColor(ofFloatColor(0.8, 0.8, 0.8));
	rimLight.setSpecularColor(ofFloatColor(0.6, 0.6, 0.6));
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setAttenuation(0.2, 0.002, 0.0);


	// SHIP LIGHT
	shipLight.setup();
	shipLight.setSpotlight();
	shipLight.setDiffuseColor(ofFloatColor(1,1,1));
	shipLight.setSpecularColor(ofFloatColor(1,1,1));
	shipLight.setAmbientColor(ofFloatColor(0.0,0.0,0.0));
	shipLight.setSpotlightCutOff(25);
	shipLight.setAttenuation(1.0, 0.01, 0.001);


	//chase cam sits in the middle above everything
	chaseCam.setNearClip(0.1);
	chaseCam.setFarClip(5000);
	chaseCam.setPosition(glm::vec3(0,200,0));
	//always looks at the lander
	chaseCam.lookAt(lander.model.getPosition());

	downCam.setNearClip(0.1);
	downCam.setFarClip(5000);
	//set it slightly below the model to not be obstructed
	downCam.setPosition(lander.model.getPosition() - glm::vec3(1,1,0));
	//always looks directly down
	downCam.lookAt(lander.model.getPosition() + glm::vec3(0,-1,0) * 10.0f);
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	lander.forces.set(0, 0, 0);
	// forward/backward
	//need to get the transform matrix from the ofxAssimpModelLoader
	glm::mat4 mat = lander.model.getModelMatrix();
	//we then multiply that matrix by (0,0,-1) which is the forward vector to get the heading
	glm::vec3 worldHeading = glm::normalize(glm::vec3(mat * glm::vec4(0,0,-1,0)));
	ofVec3f thrustVector(0, 0, 0);
	// Horizontal thrust (XZ plane)
	if (upPressed) {
		thrustVector += ofVec3f(worldHeading.x, 0, worldHeading.z) * 50;
	}
	if (downPressed) {
		thrustVector += ofVec3f(-worldHeading.x, 0, -worldHeading.z) * 50;
	}

	// Vertical thrust (Y axis)
	if (shiftPressed) {
		thrustVector += ofVec3f(0, 1, 0) * 50;
	}
	if (ctrlPressed) {
		thrustVector += ofVec3f(0, -1, 0) * 50;
	}
	ThrustShapeForce thrust(thrustVector);
	thrust.updateForce(&lander);

	// rotate clockwise
	if (rightPressed) {
		RotationalShapeForce rotation(-100);
		rotation.updateForce(&lander);
	}
	// rotate counterclockwise
	if (leftPressed) {
		RotationalShapeForce rotation(100);
		rotation.updateForce(&lander);
	}
	
	//Updating sounds
	if(thrustVector.length() > 0 || rightPressed || leftPressed){
		if (!thrustPlaying) {
        	thrustSound.play();
        	thrustPlaying = true;
   		}
	} else {
    	if (thrustPlaying) {
        	thrustSound.stop();
        	thrustPlaying = false;
    	}
	}

	
    // recheck collision
    ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
    ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();
    Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

    colBoxList.clear();
	for(int i = 0; i<octrees.size(); i++){
		octrees[i].intersect(bounds, octrees[i].root, colBoxList);
		//we have a collision
		if (!colBoxList.empty()) {
			vector<int> collisionPoints;
			//find the points of each box
			for (auto & box : colBoxList) {
				vector<int> points;
				octrees[i].getMeshPointsInBox(octrees[i].mesh, octrees[i].root.points, box, points);
				for (int p : points) {
					collisionPoints.push_back(p);
				}
			}
			//get the average normal vector of all the points
			glm::vec3 averageNormal(0, 0, 0);
			for (auto & index : collisionPoints) {
				averageNormal += octrees[i].mesh.getNormal(index);
			}
			averageNormal = glm::normalize(averageNormal);
			
			if(glm::length(lander.velocity)>20){
				 cout << "game over due to collision check" << endl;
				 glm::vec3 thrustVector = 100000 * averageNormal;
				 ThrustShapeForce thrust(ofVec3f(thrustVector.x,thrustVector.y,thrustVector.z));
				 thrust.updateForce(&lander);
				 explosionSound.play(); 
				 gameover = true;
				
				 break;
			}
			else{
				//move lander ouside of terrain if stuck inside
				glm::vec3 newPosition = lander.model.getPosition() + averageNormal * 0.01;
				lander.model.setPosition(newPosition.x, newPosition.y, newPosition.z);
				//impulse force equation
				glm::vec3 thrustVector = (1.00000001) * glm::dot(-lander.velocity,averageNormal) * averageNormal * ofGetFrameRate()*2;
				ThrustShapeForce thrust(ofVec3f(thrustVector.x,thrustVector.y,thrustVector.z));
				thrust.updateForce(&lander);
				break;
			}
		}
	}

    lander.integrate();

    // ALTITUDE AGL DETECTION
    if(bShowAGL){
        glm::vec3 landerPos = lander.model.getPosition();  
        glm::vec3 rayDir = glm::vec3(0, -1, 0);
        Ray downRay(Vector3(landerPos.x, landerPos.y, landerPos.z), Vector3(rayDir.x, rayDir.y, rayDir.z));
        TreeNode hitNode; 
		bRayHit = false;
		altitudeAGL = 1000000000;
		for(int i = 0; i<octrees.size(); i++){
			TreeNode* hit = octrees[i].intersect(downRay, octrees[i].root, hitNode);
			
			rayStart = landerPos; 
			rayEnd = landerPos + rayDir * 5000.0f; 

			if(hit != nullptr && !hit->points.empty()){
				ofVec3f hitPoint = octrees[i].mesh.getVertex(hit->points[0]);
				if(altitudeAGL > landerPos.y - hitPoint.y){
					altitudeAGL = landerPos.y - hitPoint.y;  
					rayEnd = glm::vec3(hitPoint.x, hitPoint.y, hitPoint.z);
					bRayHit = true; 
				}
			}
		}
		if(!bRayHit){
			altitudeAGL = -1;
		}
    }

    // update particle exhaust
    if (upPressed || downPressed || leftPressed || rightPressed || shiftPressed || ctrlPressed) {
		glm::vec3 landerPos = lander.model.getPosition();
		//need to get the transform matrix from the ofxAssimpModelLoader
		glm::mat4 mat = lander.model.getModelMatrix();
		//we then multiply that matrix by (0,0,-1) which is the forward vector to get the heading
		glm::vec3 landerDir = glm::normalize(glm::vec3(mat * glm::vec4(0, 0, -1, 0)));
    	exhaustEmitter.position = lander.model.getPosition() - lander.heading * 0.05f; 
    	exhaustEmitter.update();
		explosionEmitter.position = lander.model.getPosition();
		explosionEmitter.update();
	} else {
		exhaustEmitter.particles.clear(); 
	}

	//toggle ship light
	if (bShipLightOn) {
    	shipLight.setPosition(lander.model.getPosition() + glm::vec3(0,1,0)); 
    	shipLight.lookAt(lander.model.getPosition() + lander.heading * 2.0f);
    	shipLight.enable();
	} else {
    	shipLight.disable();
	}

	//camera position update
	downCam.setPosition(lander.model.getPosition() - glm::vec3(1,1,0));
	downCam.lookAt(lander.model.getPosition() + glm::vec3(0,-1,0) * 10.0f);
	chaseCam.lookAt(lander.model.getPosition());
	// distance from chase cam to lander
    float d = glm::distance(chaseCam.getPosition(), lander.model.getPosition());

    // map distance to FOV (zoom)
    float fov = ofMap(d,
                      0,   500,    // distances
                      35,  80,     // FOV range: small # = zoomed in
                      true);

    chaseCam.setFov(fov);
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);
    backgroundImage.draw(0,0, ofGetWidth(), ofGetHeight());
    glClear(GL_DEPTH_BUFFER_BIT);

    glDepthMask(false);
    if (!bHide) gui.draw();
    glDepthMask(true);

    // cam.begin();
	if(useChase){
		chaseCam.begin();
	}
	else if(useDown){
		downCam.begin();
	}
	else{
		cam.begin();
	}
    ofPushMatrix();
    if (bWireframe) {
        ofDisableLighting();
        ofSetColor(ofColor::slateGray);
        terrain.drawWireframe();
        if (bLanderLoaded) {
            lander.model.drawWireframe();
            if (!bTerrainSelected) drawAxis(lander.model.getPosition());
        }
        if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
    } else {
        ofEnableLighting();
        terrain.drawFaces();
        if (bLanderLoaded) {
            lander.draw();
            if (!bTerrainSelected) drawAxis(lander.model.getPosition());
            if (bDisplayBBoxes) {
                ofNoFill();
                ofSetColor(ofColor::white);
                for (int i = 0; i < lander.model.getNumMeshes(); i++) {
                    ofPushMatrix();
                    ofMultMatrix(lander.model.getModelMatrix());
                    ofRotate(-90, 1, 0, 0);
                    Octree::drawBox(bboxList[i]);
                    ofPopMatrix();
                }
            }

            if (bLanderSelected) {
				ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
				ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();
				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
                ofSetColor(ofColor::white);
                ofNoFill();
                Octree::drawBox(bounds);

                ofSetColor(ofColor::lightBlue);
                for (int i = 0; i < colBoxList.size(); i++) {
                    Octree::drawBox(colBoxList[i]);
                }
            }
        }
    }
    if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

    if (bDisplayPoints) {
        glPointSize(3);
        ofSetColor(ofColor::green);
        terrain.drawVertices();
    }

    if (bPointSelected) {
        ofSetColor(ofColor::blue);
        ofDrawSphere(selectedPoint, .1);
    }

    ofDisableLighting();
    if (bDisplayLeafNodes) {
		// octree.drawLeafNodes(octree.root);
		for(int i = 0; i<octrees.size(); i++){
        	octrees[i].drawLeafNodes(octrees[i].root);
		}
	}
    else if (bDisplayOctree) {
        ofNoFill();
        ofSetColor(ofColor::white);
        // octree.draw(numLevels, 0);
		for(int i = 0; i<octrees.size(); i++){
        	octrees[i].draw(5, 0);
		}
    }

    if (pointSelected) {
        ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
        ofVec3f d = p - cam.getPosition();
        ofSetColor(ofColor::lightGreen);
        ofDrawSphere(p, .02 * d.length());
    }

    // AGL Ray Intersection
    if(bShowAGL){
        ofSetColor(ofColor::green);
        ofSetLineWidth(2.0f); 
        ofDrawLine(rayStart, rayEnd);
        if(bRayHit){
            ofSetColor(ofColor::red);
            ofDrawSphere(rayEnd, 2.0f);
        }
    }

	
	if(useChase){
		if (gameover) {
			explosionEmitter.draw(explosionShader, chaseCam);
		}
		else{
			//draw particle exhaust
			exhaustEmitter.draw(particleShader, chaseCam);
		}
	}
	else if(useDown){
		if (gameover) {
			explosionEmitter.draw(explosionShader, downCam);
		}
		else{
			//draw particle exhaust
			exhaustEmitter.draw(particleShader, downCam);
		}
	}
	else{
		if (gameover) {
			explosionEmitter.draw(explosionShader, cam);
		}
		else{
			//draw particle exhaust
			exhaustEmitter.draw(particleShader, cam);
		}
	}

    ofPopMatrix();
    // cam.end();
	if(useChase){
		chaseCam.end();
	}
	else if(useDown){
		downCam.end();
	}
	else{
		cam.end();
	}

	//AGL stats 
	if(bShowAGL){
    	ofSetColor(ofColor::green);
    	if(altitudeAGL >= 0){
        	ofDrawBitmapString("AGL: " + ofToString(altitudeAGL, 2) + " units", ofGetWidth() - 200, 40);
    	} else {
        	ofDrawBitmapString("AGL: N/A", ofGetWidth() - 200, 40);
    	}
	}
}

//--------------------------------------------------------------
// Draw an XYZ axis in RGB
//
void ofApp::drawAxis(ofVec3f location) {
    ofPushMatrix();
    ofTranslate(location);
    ofSetLineWidth(1.0);
    ofSetColor(ofColor(255, 0, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
    ofSetColor(ofColor(0, 255, 0));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));
    ofSetColor(ofColor(0, 0, 255));
    ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));
    ofPopMatrix();
}



void ofApp::keyPressed(int key) {

	//lets us handle multiple keys pressed at the same time
	if (key == 'd' || key == 'D') {
		rightPressed = true;
	}
	if (key == 'a' || key == 'A') {
		leftPressed = true;
	}
	if (key == 'w' || key == 'W') {
		upPressed = true;
	}
	if (key == 's' || key == 'S') {
		downPressed = true;
	}
	if (key == OF_KEY_CONTROL) {
		//bCtrlKeyDown = true;
		ctrlPressed = true;
	}
	if (key == OF_KEY_SHIFT) {
		//bCollision = true;
		shiftPressed = true;
	}
	if (key == '1'){
		useChase = false;
		useDown = false;
	}
	if (key == '2'){
		useChase = true;
		useDown = false;
	}
	if (key == '3'){
		useChase = false;
		useDown = true;
	}

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'g':
	case 'G':
    	bShowAGL = !bShowAGL;
    	break;
	case 'H':
	case 'h':
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'P':  
	case 'p':
    	bShipLightOn = !bShipLightOn;
    	break;	
	case 'r':
		cam.reset();
		break;
	//case 's':
	//	savePicture();
	//	break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_DEL:
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {
	//lets us handle multiple keys pressed at the same time
	if (key == 'd' || key == 'D') {
		rightPressed = false;
	}
	if (key == 'a' || key == 'A') {
		leftPressed = false;
	}
	if (key == 'w' || key == 'W') {
		upPressed = false;
	}
	if (key == 's' || key == 'S') {
		downPressed = false;
	}
	if (key == OF_KEY_CONTROL) {
		//bCtrlKeyDown = true;
		ctrlPressed = false;
	}
	if (key == OF_KEY_SHIFT) {
		//bCollision = true;
		shiftPressed = false;
	}

	switch (key) {
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	default:
		break;
	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
		ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.model.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		float startTime = ofGetElapsedTimeMicros();
		raySelectWithOctree(p);
		float endTime = ofGetElapsedTimeMicros();
		if (bTimingInfo) {
			cout << std::fixed << std::setprecision(4) << "Ray intersection search time (ms): " << (endTime - startTime) / 1000 << endl;
		}
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	//cout << "Returned node with " << selectedNode.points.size() << " points" << endl;
	//cout << "Returned has " << selectedNode.children.size() << " children" << endl;
	if (pointSelected != nullptr) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.model.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.model.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
		ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);
	

		// if (bounds.overlap(testBox)) {
		// 	cout << "overlap" << endl;
		// }
		// else {
		// 	cout << "OK" << endl;
		// }


	}
	else {
		ofVec3f p;
		float startTime = ofGetElapsedTimeMicros();
		raySelectWithOctree(p);
		float endTime = ofGetElapsedTimeMicros();
		if (bTimingInfo) {
			cout << std::fixed << std::setprecision(4) << "Ray intersection search time (ms): " << (endTime - startTime)/1000 << endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

//	ofVec3f point;
//	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
//	if (lander.model.loadModel(dragInfo.files[0])) {
//		lander.model.setScaleNormalization(false);
////		lander.model.setScale(.1, .1, .1);
//	//	lander.model.setPosition(point.x, point.y, point.z);
//		lander.model.setPosition(1, 1, 0);
//
//		bLanderLoaded = true;
//		for (int i = 0; i < lander.model.getMeshCount(); i++) {
//			bboxList.push_back(Octree::meshBounds(lander.model.getMesh(i)));
//		}
//
//		cout << "Mesh Count: " << lander.model.getMeshCount() << endl;
//	}
//	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	//if (lander.model.loadModel(dragInfo.files[0])) {
	//	bLanderLoaded = true;
	//	lander.model.setScaleNormalization(false);
	//	lander.model.setPosition(0, 0, 0);
	//	cout << "number of meshes: " << lander.model.getNumMeshes() << endl;
	//	bboxList.clear();
	//	for (int i = 0; i < lander.model.getMeshCount(); i++) {
	//		bboxList.push_back(Octree::meshBounds(lander.model.getMesh(i)));
	//	}

	//	//		lander.model.setRotation(1, 180, 1, 0, 0);

	//			// We want to drag and drop a 3D object in space so that the model appears 
	//			// under the mouse pointer where you drop it !
	//			//
	//			// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
	//			// once we find the point of intersection, we can position the lander/lander
	//			// at that location.
	//			//

	//			// Setup our rays
	//			//
	//	glm::vec3 origin = cam.getPosition();
	//	glm::vec3 camAxis = cam.getZAxis();
	//	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	//	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	//	float distance;

	//	bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
	//	if (hit) {
	//		// find the point of intersection on the plane using the distance 
	//		// We use the parameteric line or vector representation of a line to compute
	//		//
	//		// p' = p + s * dir;
	//		//
	//		glm::vec3 intersectPoint = origin + distance * mouseDir;

	//		// Now position the lander's origin at that intersection point
	//		//
	//		glm::vec3 min = lander.model.getSceneMin();
	//		glm::vec3 max = lander.model.getSceneMax();
	//		float offset = (max.y - min.y) / 2.0;
	//		lander.model.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

	//		// set up bounding box for lander while we are at it
	//		//
	//		landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
	//	}
	//}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}
