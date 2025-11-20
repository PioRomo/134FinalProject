
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
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	backgroundImage.load("geo/starfield.jpg");

	terrain.loadModel("geo/134FinalScene.obj");
	//terrain.loadModel("geo/supermario_final.obj");
	terrain.setScaleNormalization(false);

	lander.position = glm::vec3(0, 0, 0);
	lander.model.setPosition(0, 0, 0);
	lander.model.loadModel("geo/134Final_lander.obj");
	//lander.model.loadModel("geo/mushroom_lander.obj");
	bLanderLoaded = true;
	lander.model.setScaleNormalization(false);
	cout << "number of meshes: " << lander.model.getNumMeshes() << endl;
	bboxList.clear();
	for (int i = 0; i < lander.model.getMeshCount(); i++) {
		bboxList.push_back(Octree::meshBounds(lander.model.getMesh(i)));
	}

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	gui.add(bTimingInfo.setup("Enable Timing Info", false));
	bHide = false;

	//  Create Octree for testing.
	//
	float startTime = ofGetElapsedTimeMillis();
	octree.create(terrain.getMesh(0), 20);
	float endTime = ofGetElapsedTimeMillis();
	
	cout << "Number of Verts: " << terrain.getMesh(0).getNumVertices() << endl;
	cout << "Time to build the tree (ms): " << endTime - startTime << endl;

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));


}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	//collided
	//move the lander normal to the collision face and recheck collission
	//if (bCollision && colBoxList.size() >= 10) {
	//	vector<int> collisionPoints;
	//	//find the points of each box
	//	for (auto & box : colBoxList) {
	//		vector<int> points;
	//		octree.getMeshPointsInBox(octree.mesh, octree.root.points, box, points);
	//		collisionPoints.push_back(points[0]);
	//	}
	//	//get the average normal vector of all the points
	//	glm::vec3 averageNormal(0,0,0);
	//	for (auto & index : collisionPoints) {
	//		averageNormal += octree.mesh.getNormal(index);
	//	}
	//	averageNormal /= collisionPoints.size();
	//	averageNormal = glm::normalize(averageNormal);
	//	cout << averageNormal << endl;
	//	//move lander along that vector
	//	lander.model.setPosition(lander.model.getPosition().x + averageNormal.x/4, lander.model.getPosition().y + averageNormal.y/4, lander.model.getPosition().z + averageNormal.z/4);

	//	//recheck collision
	// 	ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
	// 	ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

	// 	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	// 	colBoxList.clear();
	// 	octree.intersect(bounds, octree.root, colBoxList);
	// }

	//rotate clockwise
	if (rightPressed) {
		RotationalShapeForce rotation(100);
		rotation.updateForce(&lander);
	}
	//rotate counterclockwise
	if (leftPressed) {
		RotationalShapeForce rotation(-100);
		rotation.updateForce(&lander);
	}
	if (upPressed) {
		//multiply heading vector by the transform matrix to get the heading in the visible plane
		glm::vec3 worldHeading = glm::normalize(glm::vec3(lander.getTransform() * glm::vec4(lander.heading, 0.0)));
		//give the triangle a force in the heading direction
		ofVec3f thrustVector = ofVec3f(worldHeading.x, 0, worldHeading.z);
		thrustVector *= 50;
		ThrustShapeForce thrust(thrustVector);
		thrust.updateForce(&lander);
	}
	if (downPressed) {
		//multiply heading vector by the transform matrix to get the heading in the visible plane
		glm::vec3 worldHeading = glm::normalize(glm::vec3(lander.getTransform() * glm::vec4(lander.heading, 0.0)));
		//give the triangle a force in the heading direction
		ofVec3f thrustVector = ofVec3f(-worldHeading.x, 0, -worldHeading.z);
		thrustVector *= 50;
		ThrustShapeForce thrust(thrustVector);
		thrust.updateForce(&lander);
	}
	if (shiftPressed) {
		//multiply heading vector by the transform matrix to get the heading in the visible plane
		//glm::vec3 worldHeading = glm::normalize(glm::vec3(lander.getTransform() * glm::vec4(lander.heading, 0.0)));
		//give the triangle a force in the heading direction
		ofVec3f thrustVector = ofVec3f(0, 1, 0);
		thrustVector *= 50;
		ThrustShapeForce thrust(thrustVector);
		thrust.updateForce(&lander);
	}
	if (ctrlPressed) {
		//multiply heading vector by the transform matrix to get the heading in the visible plane
		//glm::vec3 worldHeading = glm::normalize(glm::vec3(lander.getTransform() * glm::vec4(lander.heading, 0.0)));
		//give the triangle a force in the heading direction
		ofVec3f thrustVector = ofVec3f(0, -1, 0);
		thrustVector *= 50;
		ThrustShapeForce thrust(thrustVector);
		thrust.updateForce(&lander);
	}

	lander.integrate();

	//recheck collision
	ofVec3f min = lander.model.getSceneMin() + lander.model.getPosition();
	ofVec3f max = lander.model.getSceneMax() + lander.model.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);

	//ALTITUDE AGL DETECTION WITH TELEMETRY (REQ #2)
	if(bShowAGL){
		glm::vec3 landerPos = lander.model.getPosition();  
		//setting ray down
		glm::vec3 rayDir = glm::vec3(0, -1, 0);
		Ray downRay(
			Vector3(landerPos.x, landerPos.y, landerPos.z),
			Vector3(rayDir.x, rayDir.y, rayDir.z)
		);

		//intersected node
		TreeNode hitNode; 
		bool hit = octree.intersect(downRay, octree.root, hitNode);
		
		//save ray start and end 
		rayStart = landerPos; 
		rayEnd = landerPos + rayDir * 5000.0f; 

		//terrain hit or no ground found
		if(hit && !hitNode.points.empty()){
			ofVec3f hitPoint = octree.mesh.getVertex(hitNode.points[0]);
			altitudeAGL = landerPos.y - hitPoint.y;  
			rayEnd = glm::vec3(hitPoint.x, hitPoint.y, hitPoint.z);
			bRayHit = true; 
		} else {
			altitudeAGL = -1; 
			bRayHit = false; 
		}
	}
}
//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255);
	backgroundImage.draw(0,0, ofGetWidth(), ofGetHeight());
	//Fixes issues with models not loading in 
	glClear(GL_DEPTH_BUFFER_BIT);

	glDepthMask(false);
	if (!bHide) gui.draw();
	glDepthMask(true);

	//drawing AGL stats (2D, so outside cam)
	if(bShowAGL){
		ofSetColor(ofColor::green);
	
		if(altitudeAGL >= 0){
			ofDrawBitmapString("AGL: " + ofToString(altitudeAGL, 2) + " units", ofGetWidth() - 200, 40);
		} else {
			ofDrawBitmapString("AGL: N/A", ofGetWidth() - 200, 40);
		}
	}

	cam.begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		terrain.drawWireframe();
		if (bLanderLoaded) {
			lander.model.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.model.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		terrain.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			//lander.model.drawFaces();
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

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		terrain.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		// cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}

	//AGL 3D Ray Intersection
	if(bShowAGL){
    	ofSetColor(ofColor::green);
    	ofSetLineWidth(2.0f); 
    	ofDrawLine(rayStart, rayEnd);

    	if(bRayHit){
        	ofSetColor(ofColor::red);
        	ofDrawSphere(rayEnd, 2.0f);
    	}
	}

	ofPopMatrix();
	cam.end();
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

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
	case 'w':
		//toggleWireframeMode();
		upPressed = true;
		break;
	case 'a':
		leftPressed = true;
		break;
	case 's':
		downPressed = true;
		break;
	case 'd':
		rightPressed = true;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		//bCtrlKeyDown = true;
		ctrlPressed = true;
		break;
	case OF_KEY_SHIFT:
		//bCollision = true;
		shiftPressed = true;
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

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case 'w':
		upPressed = false;
		break;
	case 'a':
		leftPressed = false;
		break;
	case 's':
		downPressed = false;
		break;
	case 'd':
		rightPressed = false;
		break;
	case OF_KEY_CONTROL:
		//bCtrlKeyDown = false;
		ctrlPressed = false;
		break;
	case OF_KEY_SHIFT:
		//bCollision = false;
		shiftPressed = false;
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
	if (pointSelected) {
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
