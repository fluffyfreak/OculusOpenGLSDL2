//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
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

class BoxApp : public BaseApp
{
public:
	BoxApp(int argc, char * argv[]) : BaseApp() {}
	virtual ~BoxApp() {}

	virtual bool Init() { return true; }
	virtual void Update() {};
	virtual void HandleEvent(const SDL_Event &sdl_event) {}
	virtual void DrawScene() {
		// draw left viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Left);
		//DrawSpheres(m_avatar.GetPosition());

		// draw right viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Right);
		//DrawSpheres(m_avatar.GetPosition());
	}; 

private:
};

int SDL_main(int argc, char * argv[])
{	
	BoxApp theApp(argc, argv);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}
