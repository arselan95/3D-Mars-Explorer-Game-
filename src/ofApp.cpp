
//--------------------------------------------------------------
//  Student Name: Nathan Hencky
//  Date: 5/10/2018


#include "ofApp.h"
#include "Util.h"

//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
	mainCam.setDistance(10); //cam changed to mainCam
	mainCam.setNearClip(.1);
	mainCam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();
	ofEnableLighting();

	thrusters.loadSound("sounds/thrusters.wav");	//load up the firing sound		


	//CAMERA SETUP
	emitterDown.setNearClip(.1);
	emitterDown.setFov(65.5);

	roverSide.setNearClip(.1);
	roverSide.setFov(65.5);

	landingSpace.setNearClip(.1);
	landingSpace.setFov(65.5);
	landingSpace.setPosition(ofVec3f(10, 10, 100)); //Change values to fit the map

	midAir.setNearClip(.1);
	midAir.setFov(65.5);
	midAir.setPosition(ofVec3f(-10, -10, 10)); //Change values to fit the map



	// Setup 3 - Light System
	// 
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(-4.59, 4.59, 7.65);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(15);
	fillLight.setAttenuation(2, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-11, 0.84, 13.47);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(3.37, 2.76, -14.1);


	ofDisableArbTex();     // disable rectangular textures

	 // Load textures
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	// load the shader
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/mars-low-v2.obj");
	mars.setScaleNormalization(false);

	points = mars.getMesh(0).getVertices();
	boundingBox = meshBounds(mars.getMesh(0));
	//roverBox = meshBounds(rover.getMesh(0));

	if (rover.loadModel("geo/Rover3.obj"))
	{
	
		rover.setScaleNormalization(false);
		rover.setScale(0.003, 0.003, 0.003);
		rover.setPosition(0, 0, 0);
	}
	
	gui.setup("Nipples", "settings.xml", 0.0, 0.0);
	gui.add(keyPos.setup("KeyPosition", ofVec3f(5, 5, 5), ofVec3f(-30, -30, -30), ofVec3f(30, 30, 30)));
	gui.add(fillPos.setup("FillPosition", ofVec3f(-5, 5, 5), ofVec3f(-30, -30, -30), ofVec3f(30, 30, 30)));
	gui.add(rimPos.setup("RimPosition", ofVec3f(0, 5, -7), ofVec3f(-30, -30, -30), ofVec3f(30, 30, 30)));

	engine.setOneShot(false);
	engine.setGroupSize(100);
	engine.visible = false;
	engine.setLifespan(1);
	engine.setVelocity(ofVec3f(0, -20, 0));
	engine.setRate(15);
	engine.setParticleRadius(5);

	// create our one lonely particle
	ship.lifespan = 10000;
	ship.position.set(0, 4, 0);
	sys.add(ship);
	sys.addForce(&thruster);
	sys.addForce(&impulse);
	GravityForce* g = new GravityForce(ofVec3f(0, -.1, 0));
	sys.addForce(g);


	//Camera position setup
	mainCam.setPosition(0, 0, 0);
	mainCam.setOrientation(ofVec3f(-45, 0, 0));
	mainCam.setPosition(ofVec3f(-1.0, 12.0, 16.5));

	emitterDown.setPosition(0, 0, 0);
	emitterDown.setOrientation(ofVec3f(270, 0, 0));
	//Need to know how to look down?

	//roverSide.setPosition(ofVec3f(rover.getPosition().x, rover.getPosition().y - 16, rover.getPosition().z));
	roverSide.setPosition(0, 0, 0);
	roverSide.setOrientation(ofVec3f(0, 45, 0));

	//landingSpace.setPosition(); //set position at where we land
	landingSpace.setPosition(0, 0, 0);
	landingSpace.setOrientation(ofVec3f(-270, 0, 0));
	landingSpace.setPosition(ofVec3f(mars.getSceneCenter().x, mars.getSceneCenter().y - .5, mars.getSceneCenter().z));

	//midAir.setPosition(); //Fill in position where the rover can fly by
	midAir.setPosition(0, 0, 0);
	midAir.setOrientation(ofVec3f(15, 0, 0));
	midAir.lookAt(ofVec3f(rover.getPosition().x, rover.getPosition().y, rover.getPosition().z));


	camera = &mainCam;


	camGui.setup();
	camGui.setName("F1: Ground Camera\n  F2: Window Camera\n  F3: Landing Spot\n  F4: Mid Air\n  F5: Easy Cam\n  F6: Reset\n  F7: Focus on Rover");


	//Store the indices in this node
	for (int i = 0; i < points.size(); i++)
	{
		tree.root->indices.push_back(i);
	}
	tree.root->boundary = boundingBox;
	populateTree(tree.root, points);

	shipRay.direction = Vector3(0, -1, 0);


	clicked = false;
	released = true;

	music.loadSound("sounds/music.mp3");
	music.play();
}

void ofApp::populateTree(OctalNode* node, vector<ofPoint>& points)
{

	if (node->indices.size() <= 1)
	{
		return;
	}
	
	vector<Box> childBoxes;	//Empty vector to hold children
	subDivideBox8(node->boundary, childBoxes);	//Fill children vector with the 8 subdivided boxes
	
	for (int i = 0; i < 8; i++)
	{
		node->children.push_back(new OctalNode());
		node->children[i]->boundary = childBoxes[i];
	}

	for (int j = 0; j < node->indices.size(); j++)
	{
		for (int k = 0; k < 8; k++)
		{
			if (childBoxes[k].contains(points[node->indices[j]]))
			{
				node->leafNode = false;
				node->children[k]->indices.push_back(node->indices[j]);
				//cout << childBoxes[k].volume() << endl;
			}
		}
	}

	for (int m = 0; m < 8; m++)
	{
		populateTree(node->children[m], points);
	}
	
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() 
{
	ofSeedRandom();
	keyLight.setPosition(keyPos);
	fillLight.setPosition(fillPos);
	rimLight.setPosition(rimPos);

	sys.update();
	engine.update();
	engine.setPosition(sys.particles[0].position);
	rover.setPosition(sys.particles[0].position.x, sys.particles[0].position.y, sys.particles[0].position.z);
	shipRay.origin = Vector3(sys.particles[0].position.x, sys.particles[0].position.y, sys.particles[0].position.z);
	vector<OctalNode*> intersecting;
	findBox(tree.root, shipRay, intersecting);
	float altitude = 1.1;
	if (intersecting.size() > 0)
	{
		OctalNode* hit = closestBox(intersecting);
		float altitude = abs(hit->boundary.max().y() - rover.getPosition().y);
		cout << "Altitude: " << abs(hit->boundary.max().y() - rover.getPosition().y) << endl;
		if (altitude < 1)
		{
			ofVec3f dingle = rover.getPosition();
			ofVec3f offset(0, rover.getSceneMin(true).y, 0);
			if (checkCollision(tree.root, dingle + offset))
			{
				ofVec3f v = sys.particles[0].velocity * -60;
				impulse.apply(v);
				cout << altitude << " COLLISION" << endl;
			}
		}
	}
	
	//cout << "Framerate: " << ofGetFrameRate() << endl;
	
	//Camera Updates for position for roverSide and emitterDown cameras only. Others are fixed position looking at rover
	roverSide.setPosition(ofVec3f(rover.getPosition().x, rover.getPosition().y, rover.getPosition().z));
	emitterDown.setPosition(ofVec3f(rover.getPosition().x, rover.getSceneMin().y + rover.getPosition().y, rover.getPosition().z));
	midAir.setPosition(ofVec3f(rover.getPosition().x, rover.getPosition().y + 2, rover.getPosition().z + 5));
	midAir.lookAt(rover.getPosition());
	
	
	if (released != true)
	{
		keyPressed(pressedKey);
	}

	else
	{
		engine.sys->reset();
		engine.start();
		thrusters.stop();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	loadVbo();
	ofBackground(ofColor::black);
	ofSetDepthTest(false);
	camGui.draw();
	ofSetDepthTest(true);
	camera->begin();
	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bRoverLoaded) {
			//rover.drawWireframe();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();

		if (bRoverLoaded) {
			//rover.drawFaces();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}


	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}
	

	//keyLight.draw();
	//fillLight.draw();
	//rimLight.draw();

	//sys.draw();
	//engine.draw();
	rover.drawFaces();
	
	//ofNoFill();
	//ofSetColor(ofColor::white);

	//drawBoxes(tree.root);
	ofPopMatrix();
	camera->end();
	glDepthMask(GL_FALSE);

	ofSetColor(255, 100, 90);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();


	// begin drawing in the camera
	//
	shader.begin();
	camera->begin();

	// draw particle emitter here..
	//
	//	emitter.draw();
	particleTex.bind();
	ofSetColor(255, 255, 255, 255);
	const ofFloatColor c(1, 0.5, 0.5, 0.5);
	vbo.draw(GL_POINTS, 0, (int)engine.sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	camera->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	//cout << ofGetFrameRate() << endl;
}

void ofApp::drawBoxes(const OctalNode* root)
{
	if (root == nullptr)
	{
		return;
	}
	if (root->indices.empty())
	{
		return;
	}
	if (root->isLeaf())
	{
		drawBox(root->boundary);
//cout << root->indices.size() << endl;
	}
	for (int i = 0; i < root->children.size(); i++)
	{
		drawBoxes(root->children[i]);
	}
}

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

void ofApp::keyPressed(int key) 
{
	ofVec3f xAxis;
	ofVec3f yAxis;
	switch (key) {
	case OF_KEY_UP:
		released = false;
		yAxis = cam.getYAxis();
		thruster.set(yAxis);
		thrusters.play();
		pressedKey = OF_KEY_UP;
		break;
	case OF_KEY_DOWN:
		break;
	case OF_KEY_LEFT:
		released = false;
		xAxis = -1 * cam.getXAxis();
		thruster.set(xAxis);
		thrusters.play();
		pressedKey = OF_KEY_LEFT;
		break;
	case OF_KEY_RIGHT:
		released = false;
		xAxis = cam.getXAxis();
		thruster.set(xAxis);
		thrusters.play();
		pressedKey = OF_KEY_RIGHT;
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
	case 'H':
	case 'h':
		break;
	case 'r':
		mainCam.setTarget(rover.getPosition()); //Resets easyCam view as PDF states
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		engine.stop();
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
		//Camera Controls
	case OF_KEY_F1:
		camera = &emitterDown;
		break;
	case OF_KEY_F2:
		camera = &roverSide;
		break;
	case OF_KEY_F3:
		camera = &landingSpace;
		break;
	case OF_KEY_F4:
		camera = &midAir;
		break;
	case OF_KEY_F5:
		camera = &mainCam;
		break;
	case OF_KEY_F6:
		mainCam.setPosition(0, 0, 0);
		mainCam.setOrientation(ofVec3f(-45, 0, 0));
		mainCam.setPosition(ofVec3f(-1.0, 12.0, 16.5));
		break;
	case OF_KEY_F7:
		mainCam.lookAt(rover.getPosition());
	default:
		break;
	}
}
void ofApp::keyReleased(int key) {
	switch (key) {
	case ' ':
		break;
	case OF_KEY_LEFT:
		thruster.set(ofVec3f(0, 0, 0));
		released = true;
		break;
	case OF_KEY_RIGHT:
		thruster.set(ofVec3f(0, 0, 0));
		released = true;
		break;
	case OF_KEY_UP:
		thruster.set(ofVec3f(0, 0, 0));
		released = true;
		break;
	case OF_KEY_DOWN:
		thruster.set(ofVec3f(0, 0, 0));
		released = true;
		break;;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
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

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	r = ray;
	OctalNode* temp = tree.root;
	clicked = true;
}

void ofApp::findBox(OctalNode* tree, Ray ray, vector<OctalNode*>& intersecting)
{
	OctalNode* temp = tree;
	OctalNode* dingle;
	if (temp == nullptr)
	{
		return;
	}
	if (temp->isLeaf())
	{
		return;
	}

	for (int i = 0; i < temp->children.size(); i++)
	{
		if (temp->children[i]->boundary.intersect(ray, -100, 100) && temp->children[i]->indices.size() > 0)
		{
			intersecting.push_back(temp);
			dingle = temp->children[i];
			findBox(dingle, ray, intersecting);
			break;
		}
	}
}


//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}


//
//  Select Target Point on Terrain by comparing distance of mouse to 
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//
bool ofApp::doPointSelection() {

	ofMesh mesh = mars.getMesh(0);
	int n = mesh.getNumVertices();
	float nearestDistance = 0;
	int nearestIndex = 0;

	bPointSelected = false;

	ofVec2f mouse(mouseX, mouseY);
	vector<ofVec3f> selection;

	// We check through the mesh vertices to see which ones
	// are "close" to the mouse point in screen space.  If we find 
	// points that are close, we store them in a vector (dynamic array)
	//
	for (int i = 0; i < n; i++) {
		ofVec3f vert = mesh.getVertex(i);
		ofVec3f posScreen = cam.worldToScreen(vert);
		float distance = posScreen.distance(mouse);
		if (distance < selectionRange) {
			selection.push_back(vert);
			bPointSelected = true;
		}
	}

	//  if we found selected points, we need to determine which
	//  one is closest to the eye (camera). That one is our selected target.
	//
	if (bPointSelected) {
		float distance = 0;
		for (int i = 0; i < selection.size(); i++) {
			ofVec3f point =  cam.worldToCamera(selection[i]);

			// In camera space, the camera is at (0,0,0), so distance from 
			// the camera is simply the length of the point vector
			//
			float curDist = point.length(); 

			if (i == 0 || curDist < distance) {
				distance = curDist;
				selectedPoint = selection[i];
			}
		}
	}
	return bPointSelected;
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
void ofApp::dragEvent(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);

	if (rover.loadModel(dragInfo.files[0])) {
		rover.setScaleNormalization(false);
		rover.setScale(.005, .005, .005);
		rover.setPosition(point.x, point.y, point.z);
		bRoverLoaded = true;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

OctalNode::OctalNode()
{
	leafNode = true;
}

bool OctalNode::isLeaf() const
{
	return leafNode;
}

void OctalNode::setBounds(const Box& boundary)
{
	this->boundary = boundary;
}

OctalTree::OctalTree()
{
	root = new OctalNode();
}

void OctalTree::setBounds(const Box& boundary)
{
	root->setBounds(boundary);
}

OctalNode* ofApp::closestBox(const vector<OctalNode*>& smallest)
{
	ofVec3f cameraPos = rover.getPosition();
	ofVec3f closest(smallest[0]->boundary.center().x(), smallest[0]->boundary.center().y(), smallest[0]->boundary.center().z());
	float dSquared = cameraPos.distanceSquared(closest);
	int index = 0;
	for (int i = 1; i < smallest.size(); i++)
	{
		ofVec3f temp(smallest[i]->boundary.center().x(), smallest[i]->boundary.center().y(), smallest[i]->boundary.center().z());
		float d = cameraPos.distanceSquared(temp);
		if (d < dSquared)
		{
			dSquared = d;
			index = i;
		}
	}
	return smallest[index];
}

bool ofApp::checkCollision(OctalNode* tree, ofVec3f& point)
{
	if (tree->indices.size() == 0)
	{
		return false;
	}

	if (tree->isLeaf() == false)
	{
		for (int i = 0; i < tree->children.size(); i++)
		{
			if (tree->children[i]->boundary.contains(point))
			{
				return checkCollision(tree->children[i], point);
			}
		}
	}
	else
	{
		if (tree->boundary.center() != boundingBox.center())
		{
			return tree->boundary.contains(point);
		}
		else
		{
			return false;
		}
	}
}

void ofApp::loadVbo() {
	if (engine.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < engine.sys->particles.size(); i++) {
		points.push_back(engine.sys->particles[i].position);
		sizes.push_back(ofVec3f(5));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
	
}