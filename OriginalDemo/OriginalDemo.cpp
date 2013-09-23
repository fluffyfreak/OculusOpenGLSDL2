//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// Originally by: obzen (aka: Olivier Renault.)
// https://developer.oculusvr.com/forums/viewtopic.php?f=30&t=4249&p=55882&hilit=oculus+sdl#p55882
// http://pastebin.com/2VEasjbu
// http://pastebin.com/2qdG5kRj
// 
// Author : Andrew Copland.
//----------------------------------------------
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "sdl.h"
#include "OVR.h"

#include "BaseApp.h"
#include "Avatar.h"

class OriginalApp : public BaseApp
{
public:
	OriginalApp(int argc, char * argv[]) : BaseApp(), m_sphereList(-1) {
		if(!glIsList(m_sphereList))
		{
			m_sphereList = glGenLists(1);
			glNewList(m_sphereList,GL_COMPILE);
			GLUquadricObj* sphere=gluNewQuadric();
			gluQuadricNormals(sphere, GLU_SMOOTH);
			gluQuadricTexture(sphere, GL_TRUE);
			gluSphere(sphere,0.5,20,20);
			gluDeleteQuadric(sphere);	   
			glEndList();
		}
	}
	virtual ~OriginalApp() {
		if(glIsList(m_sphereList))
		{
			glDeleteLists(m_sphereList, 1);
		}
	}

	virtual bool Init() { 
		return true; 
	}
	virtual void Update() {
	};
	virtual void HandleEvent(const SDL_Event &sdl_event) {
	}
	virtual void DrawScene() {
		// draw left viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Left);
		DrawSpheres(m_avatar.GetPosition());

		// draw right viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Right);
		DrawSpheres(m_avatar.GetPosition());
	}; 

private:
	// draw a bunch of spheres
	void DrawSpheres(const OVR::Vector3f position)
	{
		if(!glIsList(m_sphereList)) {
			return;
		}

		float scale = 10.0f;
		float step = scale * 2.0f;
		int nx = 10;
		int ny = 1;
		int nz = 10;
		
		float cx = (int)(position.x / step) * step;
		float cy = (int)(position.y / step) * step;
		float cz = (int)(position.z / step) * step;
		
		for(int ix = 0; ix < nx; ix++)
		{
			for(int iz = 0; iz < nz; iz++)
			{
				for(int iy = 0; iy < ny; iy++)
				{
					glPushMatrix();
					float x = cx + (ix - nx * 0.5f)* (step);
					float y = cy + (iy - ny * 0.5f)* (step);
					float z = cz + (iz - nz * 0.5f)* (step);
					glTranslatef(x, y, z);
					glScalef(scale, scale, scale);
					glCallList(m_sphereList);
					glPopMatrix();
				}
			}
		}
	}

	GLuint m_sphereList;
};

int SDL_main(int argc, char * argv[])
{	
	OriginalApp theApp(argc, argv);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}
