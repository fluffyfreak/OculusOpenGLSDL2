//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _SCENE_H_
#define _SCENE_H_

#include "OVR.h"

// A bunch of 3D-rendered stuff
class Scene
{
public:
	Scene();
	~Scene();

	void draw_sphere();

	// draw a bunch of spheres
	void draw(const OVR::Vector3f position);

private:
	int spherelist;
};

#endif // _SCENE_H_