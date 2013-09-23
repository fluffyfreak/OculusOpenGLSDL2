//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _EYEPATCH_H_
#define _EYEPATCH_H_

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "OVR.h"

// render frame buffer to a portion of the screen, 
// so we can apply the distortion shader.
class EyePatch
{
public:
	// various patches for eyes.
	void Setup(OVR::Util::Render::StereoEye eye);

	void Render();

	void Cleanup();

private:
	static const float g_vertex_buffer_data[3][4][3];
	static const float g_uv_buffer_data[3][4][2];

	GLuint m_vertex_arrays;
	GLuint m_vertex_buffer;
	GLuint m_uv_buffer;
};



#endif // _EYEPATCH_H_