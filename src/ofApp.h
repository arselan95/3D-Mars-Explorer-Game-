#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "ofxGui.h"

/*Octal Node class*/
class OctalNode
{
public:
	//Data Members
	std::vector<int> indices;	//indices in mars grid
	std::vector<OctalNode*> children;
	Box boundary;
	bool leafNode;

	//Constructor
	OctalNode();

	//Methods
	bool isLeaf() const;
	void setBounds(const Box& boundary);
};

class OctalTree
{
public:
	//Data Members
	OctalNode* root;

	//Constructor
	OctalTree();

	//Methods
	void setBounds(const Box& boundary);
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool  doPointSelection();
		void drawBox(const Box &box);
		void drawBoxes(const OctalNode* root);
		void loadVbo();


		Box meshBounds(const ofMesh &);
		void subDivideBox8(const Box &b, vector<Box> & boxList);
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		
		void populateTree(OctalNode* tree, vector<ofPoint>& points);
		void findBox(OctalNode* tree, Ray ray, vector<OctalNode*>& intersecting);
		OctalNode* closestBox(const vector<OctalNode*>& smallest);
		bool checkCollision(OctalNode* tree, ofVec3f& point);

		ofSoundPlayer thrusters;
		ofSoundPlayer music;

		Ray r;
		Ray shipRay;
		ofEasyCam cam;
		ofxAssimpModelLoader mars, rover;
		ofLight light;
		Box boundingBox;
		Box roverBox;
		ofLight keyLight, fillLight, rimLight;
		std::vector<ofPoint> points;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;		
		bool bRoverLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		OctalTree tree;

		const float selectionRange = 4.0;

		Particle ship;
		ThrusterForce thruster;
		ParticleSystem sys;
		ParticleEmitter engine;
		ImpulseForce impulse;
		bool released;
		int pressedKey;
		bool clicked;
		ofxPanel gui;

		ofxPanel camGui;

		ofxVec3Slider keyPos;
		ofxVec3Slider fillPos;
		ofxVec3Slider rimPos;

		ofTexture  particleTex;
		ofVbo vbo;
		ofShader shader;


		//Camera Stuff
		ofCamera emitterDown, roverSide, landingSpace, midAir;
		ofEasyCam mainCam;

		ofCamera *camera; //use to point to everything else

};
