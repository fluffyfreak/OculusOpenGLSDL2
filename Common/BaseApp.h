//----------------------------------------------
// Oculus + OpenGL + SDL 2 BaseApp class.
// Derive from this and overload virtual methods to create custom apps.
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _BASEAPP_H_
#define _BASEAPP_H_

#include "sdl.h"
#include "Oculus.h"
#include "Avatar.h"
#include "EyePatch.h"

class BaseApp
{
public:
	BaseApp();
	virtual ~BaseApp();

	int Run();

	virtual bool Init()=0;
	virtual void Update()=0;
	virtual void HandleEvent(const SDL_Event &sdl_event)=0;
	virtual void DrawScene()=0; 
	
private:
	void SetupFramebuffer();
	void SetupOculus();
	void SetupSdl();
	void SetupMaterial();
	void SetupLighting();
	void SetupOpenGL();
	void SetupShaders();

	void UpdateTick();
	void UpdateEvents();

	void PreSceneDrawing();
	void PostSceneDrawing();
	void RenderEyePatch(OVR::Util::Render::StereoEye eye);
	void PostProcessFramebuffer();

	void gl_uniform_2f(const char* varname, float a, float b) { GLuint varid = glGetUniformLocation(gl_fragment_shader_program, varname); glUniform2f(varid, a, b); }
	void gl_uniform_4f(const char* varname, float a, float b, float c, float d) { GLuint varid = glGetUniformLocation(gl_fragment_shader_program, varname); glUniform4f(varid, a, b, c, d); }
	void gl_uniform_1i(const char* varname, int value) { GLuint varid = glGetUniformLocation(gl_fragment_shader_program, varname); glUniform1i(varid, value); }

	void CleanupFramebuffer();
	void CleanupOpenGL();
	void CleanupSDL();

protected:
	// SDL maintenance stuff
	bool sdl_use_vertex_shaders;
	bool sdl_debug;
	bool sdl_quit;
	SDL_Window *sdl_window;
	SDL_GLContext sdl_opengl_context;
	unsigned int sdl_frame_timestamp;
	unsigned int sdl_frame_timestep;

	// frame buffer maintenance.
	GLuint gl_frame_buffer;
	GLuint gl_frame_buffer_texture;
	GLuint gl_frame_buffer_depth;
	GLuint gl_draw_buffers[1];

	// shader maintenance.
	GLuint gl_fragment_shader_program;

	// main object encapsulation.
	Avatar m_avatar;
	Oculus m_oculus;
	EyePatch m_eye_patch[3];
};

#endif // _BASEAPP_H_