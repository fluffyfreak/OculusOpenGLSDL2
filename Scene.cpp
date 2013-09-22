//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#include <GL/glew.h>
#include <GL/gl.h>
#include "Scene.h"

// A bunch of 3D-rendered stuff
Scene::Scene()
{
	spherelist = -1;
}

Scene::~Scene()
{
	if(glIsList(spherelist))
	{
		glDeleteLists(spherelist, 1);
	}
}

void Scene::draw_sphere()
{
	if(!glIsList(spherelist))
	{
		spherelist = glGenLists(1);
		glNewList(spherelist,GL_COMPILE);
		GLUquadricObj* sphere=gluNewQuadric();
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluQuadricTexture(sphere, GL_TRUE);
		gluSphere(sphere,0.5,20,20);
		gluDeleteQuadric(sphere);	   
		glEndList();
	}

	if(glIsList(spherelist))
	{
		glCallList(spherelist);
	}
}

// draw a bunch of spheres
void Scene::draw(const OVR::Vector3f position)
{
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
				draw_sphere();
				glPopMatrix();
			}
		}
	}
}
