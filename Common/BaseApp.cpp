//----------------------------------------------
// Oculus + OpenGL + SDL 2 BaseApp class.
// Derive from this and overload virtual methods to create custom apps.
// 
// Author : Andrew Copland.
//----------------------------------------------

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <stdio.h>
#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "sdl.h"
#include "OVR.h"

#include "Logging.h"
#include "Oculus.h"
#include "EyePatch.h"

#include "BaseApp.h"

void APIENTRY gl_debug_callback(	GLenum source,
									GLenum type,
			                        GLuint id,
						            GLenum severity,
									GLsizei length,
			                        const GLchar* message,
						            void* userParam)
{
	TRACE("[OPENGL ERROR] source(%s) type(%s) id(%d) severity(%s) '%s'", glewGetString(source), glewGetString(type), id, glewGetString(severity), message);
	if(severity == GL_DEBUG_SEVERITY_HIGH)
		debug_break();
}

static const char* gl_vertex_shader_code = 
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 Position;\n"
"layout(location = 1) in vec2 TexCoord;\n"
"out vec2 oTexCoord;\n"
"\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(Position, 1);\n"
"   oTexCoord = TexCoord;\n"
"};\n";

static const char* gl_fragment_shader_code = 
"#version 330\n"
"\n"
"uniform vec2 LensCenter;\n"
"uniform vec2 ScreenCenter;\n"
"uniform vec2 Scale;\n"
"uniform vec2 ScaleIn;\n"
"uniform vec4 HmdWarpParam;\n"
"uniform sampler2D texture0;\n"
"varying vec2 oTexCoord;\n"
"out vec4 outcolor;\n"
"\n"
"vec2 HmdWarp(vec2 in01)\n"
"{\n"
"   vec2  theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]\n"
"   float rSq = theta.x * theta.x + theta.y * theta.y;\n"
"   vec2  theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + \n"
"                           HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
"   return LensCenter + Scale * theta1;\n"
"}\n"
"void main()\n"
"{\n"
"   vec2 tc = HmdWarp(oTexCoord);\n"
"   if (!all(equal(clamp(tc, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tc)))\n"
"       outcolor = vec4(0);\n"
"   else\n"
"	   outcolor = texture2D(texture0, tc);\n"
"};\n";



// main object encapsulation.
/*Avatar m_avatar;
Scene m_scene;*/

BaseApp::BaseApp() : 
	sdl_use_vertex_shaders(true),
	sdl_debug(false),
	sdl_quit(false),
	sdl_window(NULL),
	sdl_opengl_context(0),
	sdl_frame_timestamp(0),
	sdl_frame_timestep(16),
	gl_frame_buffer(0),
	gl_frame_buffer_texture(0),
	gl_frame_buffer_depth(0),
	gl_fragment_shader_program(0)
{
	SetupOculus();
	SetupSdl();
	SetupOpenGL();
	SetupShaders();
	SetupFramebuffer();
}

//virtual 
BaseApp::~BaseApp() {
	CleanupFramebuffer();
	CleanupOpenGL();
	CleanupSDL();
}

void BaseApp::CleanupFramebuffer()
{
	// [TODO] stop being lazy.
}

void BaseApp::CleanupOpenGL()
{
	// [TODO] stop being lazy.
}

void BaseApp::CleanupSDL()
{
	SDL_GL_DeleteContext(sdl_opengl_context);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

// create frame buffer where we render the scene.
void BaseApp::SetupFramebuffer()
{
	// setup frame buffer. Resolution can be set to anything, preferably higher than oculus resolution. 
	GLsizei width = m_oculus.GetHMDInfo().HResolution;
	GLsizei height = m_oculus.GetHMDInfo().VResolution;
	
	glGenFramebuffers(1, &gl_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer);

	// The texture we're going to render to
	glGenTextures(1, &gl_frame_buffer_texture);
	 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, gl_frame_buffer_texture);
	 
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
	 
	// linear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// The depth buffer
	glGenRenderbuffers(1, &gl_frame_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, gl_frame_buffer_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gl_frame_buffer_depth);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gl_frame_buffer_texture, 0);
	 
	// Set the list of draw buffers.
	GLenum gl_draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, gl_draw_buffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ASSERT("[FRAMEBUFFER] error.");
	}
}

void BaseApp::SetupOculus()
{
	m_oculus.Start();
	m_avatar.Start(&m_oculus);
}

void BaseApp::SetupSdl()
{
	// initialise out rendering context.
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return;    
	
	const OVR::HMDInfo& hmd_info = m_oculus.GetHMDInfo();
	sdl_window = SDL_CreateWindow("Oculus!", 100, 100, hmd_info.HResolution, hmd_info.VResolution, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED);
	sdl_opengl_context = SDL_GL_CreateContext(sdl_window);

	if(!sdl_debug)
	{
		// set window bounds into oculus display.
		SDL_SetWindowPosition(sdl_window, hmd_info.DesktopX, hmd_info.DesktopY);
		SDL_SetWindowSize(sdl_window, hmd_info.HResolution, hmd_info.VResolution);
		//SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN);
	}
}

void BaseApp::SetupMaterial()
{	
	// basic material for the scene.
	float mat_specular[4]={1.0f,1.0f,1.0f,1.0f};
	float mat_diffuse[4] ={0.5f,0.0f,0.0f,1.0f};
	float mat_ambient[4] ={0.2f,0.1f,0.1f,1.0f};
	float mat_shininess=50.0;

	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_ambient);
}

void BaseApp::SetupLighting()
{
	// basic lighting for the scene.
	GLfloat global_ambient[]={.3f,.3f,.3f,1.0f};
	GLfloat ambient[]={1.0f,0.0f,0.0f,1.0f};
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_LIGHT_MODEL_AMBIENT,global_ambient);

	GLfloat diffuse[]= { 0.5,0.5,0.5,1.0 };
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);

	GLfloat specular[] = { 1.0,1.0,1.0,1.0 };
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

	GLfloat light_position[]={ 100.0,1000.0,100.0,1.0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void BaseApp::SetupOpenGL()
{
	// debugging 
	if(glDebugMessageControl)	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, NULL, true);
	if(glDebugMessageCallback)	glDebugMessageCallback(gl_debug_callback, NULL);
	
	// shading
	glClearColor	(0.1f, 0.1f, 0.1f, 1.0f);
	glShadeModel	(GL_SMOOTH);
	glEnable		(GL_NORMALIZE);

	// culling
	glPolygonMode	(GL_FRONT_AND_BACK,GL_FILL);
	glEnable		(GL_CULL_FACE);
	glFrontFace		(GL_CW);
	glCullFace		(GL_BACK);
	glEnable		(GL_DEPTH_TEST);
	glDepthMask		(GL_TRUE);
	glDepthFunc		(GL_LEQUAL);
	
	// blending
	glEnable		(GL_BLEND);
	glBlendFunc		(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetupLighting();
	SetupMaterial();
}

void BaseApp::SetupShaders()
{
	// initialise glew API.
	glewInit();

	// setup eye quads.
	for(int i = 0; i < 3; ++i) {
		m_eye_patch[i].Setup((OVR::Util::Render::StereoEye) i);
	}
	
	// Now create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	
    // Compile Vertex Shader
    trace("Compiling vertex shader");
    glShaderSource(VertexShaderID, 1, &gl_vertex_shader_code , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
            std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            trace("%s", &VertexShaderErrorMessage[0]);
    }

	// Compile Fragment Shader
    trace("Compiling fragment shader");
    glShaderSource(FragmentShaderID, 1, &gl_fragment_shader_code , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            trace("%s", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    trace("Linking program");
    gl_fragment_shader_program = glCreateProgram();
    glAttachShader(gl_fragment_shader_program, VertexShaderID);
    glAttachShader(gl_fragment_shader_program, FragmentShaderID);
    glLinkProgram(gl_fragment_shader_program);

    // Check the program
    glGetProgramiv(gl_fragment_shader_program, GL_LINK_STATUS, &Result);
    glGetProgramiv(gl_fragment_shader_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(gl_fragment_shader_program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            trace("%s", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
}

int BaseApp::Run() {
	while(!sdl_quit)
	{
		UpdateTick();
		UpdateEvents();

		// move the viewpoint, update camera orientations.
		m_avatar.Update(sdl_frame_timestep / 1000.0f);

		Update();

		PreSceneDrawing();
		DrawScene();
		PostSceneDrawing();
		
		PostProcessFramebuffer();
	}
	return 0;
}
	
void BaseApp::UpdateTick()
{
	// limit framerate in a ham-fisted way.
	unsigned int sdl_frame_time = SDL_GetTicks() - sdl_frame_timestamp;
	if(sdl_frame_time < sdl_frame_timestep) SDL_Delay(sdl_frame_timestep - sdl_frame_time);
	sdl_frame_timestamp = SDL_GetTicks();
}

void BaseApp::UpdateEvents()
{
	SDL_Event event;	
	while( SDL_PollEvent( &event ) )
	{
		if( event.type == SDL_KEYDOWN ) 
		{
			if(event.key.keysym.sym == SDLK_ESCAPE) {
				sdl_quit = true;
			} else {
				m_avatar.OnKeyDown(event.key.keysym.sym);
			}
		}
		else if( event.type == SDL_KEYUP )
		{
			m_avatar.OnKeyUp(event.key.keysym.sym);
		}
		else if( event.type == SDL_QUIT )
		{
			sdl_quit = true;
		}
		HandleEvent(event);
	}
}

// render the 3D scene for each eye.
void BaseApp::PreSceneDrawing()
{
	glPushAttrib( GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_LIGHTING );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// render the 3D scene for each eye.
void BaseApp::PostSceneDrawing()
{
	glPopAttrib();
}

// output each viewpoint to the display screen.
void BaseApp::RenderEyePatch(OVR::Util::Render::StereoEye eye)
{
	// apply distortion for each eye.
	const OVR::Util::Render::StereoEyeParams& params = m_oculus.GetStereoConfig().GetEyeRenderParams(eye);
	if(params.pDistortion)
	{
		GLsizei width = m_oculus.GetHMDInfo().HResolution;
		GLsizei height = m_oculus.GetHMDInfo().VResolution;
		const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;

		float w  = float(params.VP.w) / float(width),
			  h  = float(params.VP.h) / float(height),
			  x  = float(params.VP.x) / float(width),
			  y  = float(params.VP.y) / float(height);
		float as = float(params.VP.w) / float(params.VP.h);

		// We are using 1/4 of DistortionCenter offset value here, since it is
		// relative to [-1,1] range that gets mapped to [0, 0.5].
		float HmdWarpParam[4]	= { distortion.K[0], distortion.K[1], distortion.K[2], distortion.K[3] };
		float ChromAbParam[4]	= { distortion.ChromaticAberration[0], distortion.ChromaticAberration[1], distortion.ChromaticAberration[2], distortion.ChromaticAberration[3] };
		float scaleFactor		= 1.0f / distortion.Scale;
		OVR::Vector2f LensCenter	(x + (w + distortion.XCenterOffset * 0.5f)*0.5f,	y + h*0.5f);
		OVR::Vector2f ScreenCenter	(x + w*0.5f,										y + h*0.5f);
		OVR::Vector2f Scale			((w/2) * scaleFactor,								(h/2) * scaleFactor * as);
		OVR::Vector2f ScaleIn		((2/w),												(2/h) / as);
		
		// fragment shader.
		gl_uniform_2f("LensCenter",		LensCenter.x,	 LensCenter.y);
		gl_uniform_2f("ScreenCenter",	ScreenCenter.x,	 ScreenCenter.y);
		gl_uniform_2f("Scale",			Scale.x,		 Scale.y);
		gl_uniform_2f("ScaleIn",		ScaleIn.x,		 ScaleIn.y);
		gl_uniform_4f("HmdWarpParam",	HmdWarpParam[0], HmdWarpParam[1], HmdWarpParam[2], HmdWarpParam[3]);
		gl_uniform_4f("ChromAbParam",	ChromAbParam[0], ChromAbParam[1], ChromAbParam[2], ChromAbParam[3]);
	}
	
	// render the quad on display.
	m_eye_patch[eye].Render();
}

// send 3D screen render to the display.
// apply the post process shaders.
void BaseApp::PostProcessFramebuffer()
{
	// dumb way to display the frame buffer as a full screen quad, but hey...
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_LIGHTING );
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
	// Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset the screen params.
	glViewport(0, 0, m_oculus.GetHMDInfo().HResolution, m_oculus.GetHMDInfo().VResolution); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// clear the screen.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the post process shader.
	glUseProgram(gl_fragment_shader_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_frame_buffer_texture);
	gl_uniform_1i("texture0", 0);

	// render left eye with distortion shader
	RenderEyePatch(OVR::Util::Render::StereoEye_Left);

	// render right eye with distortion shader
	RenderEyePatch(OVR::Util::Render::StereoEye_Right);

	// clean up.
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);	
	glPopAttrib();

	SDL_GL_SwapWindow(sdl_window);
}
