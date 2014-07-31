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

// Include GLM
#include <glm/glm.hpp>

#include "BaseApp.h"
#include "ShaderHelper.h"

namespace {
	static const glm::vec4 WHITE  = glm::vec4(1.0f,1.0f,1.0f,1.0f);
	static const glm::vec4 BLACK  = glm::vec4(0.0f,0.0f,0.0f,1.0f);
	static const glm::vec4 RED    = glm::vec4(1.0f,0.0f,0.0f,1.0f);
	static const glm::vec4 GREEN  = glm::vec4(0.0f,1.0f,0.0f,1.0f);
	static const glm::vec4 BLUE   = glm::vec4(0.0f,0.0f,1.0f,1.0f);
	static const glm::vec4 YELLOW = glm::vec4(1.0f,1.0f,0.0f,1.0f);
	static const glm::vec4 CYAN   = glm::vec4(0.0f,1.0f,1.0f,1.0f);
	static const glm::vec4 MAGENTA= glm::vec4(1.0f,0.0f,0.5f,1.0f);

	const std::string vertstr("PosCol.vert");
	const std::string fragstr("PosCol.frag");
};

class BoxApp : public BaseApp
{
public:
	BoxApp(int argc, char * argv[]) : 
		BaseApp(),
		mVAO(UINT_MAX),
		mVertObjId(UINT_MAX),
		mColourObjId(UINT_MAX),
		mElementBufferId(UINT_MAX)
	{
	}

	virtual ~BoxApp() {
		Cleanup();
	}

	virtual bool Init() { 
		InitBox();
		LoadShader(mShaderProg, vertstr, fragstr); 

		// Get a handle for our "MVP" uniform(s)
		MatrixID		= glGetUniformLocation(mShaderProg, "MVP");
		ViewMatrixID	= glGetUniformLocation(mShaderProg, "V");
		ModelMatrixID	= glGetUniformLocation(mShaderProg, "M");
		
		return true;
	}

	virtual void Update() {};
	virtual void HandleEvent(const SDL_Event &sdl_event) {}

	virtual void DrawScene() {
		glDisable(GL_CULL_FACE); // hack!!!

		// draw left viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Left);
		DrawBox();
		
		// draw right viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Right);
		DrawBox();
	}; 

private:
	void InitBox()
	{
		Cleanup();

		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		glGenBuffers(1, &mVertObjId);
		glBindBuffer(GL_ARRAY_BUFFER, mVertObjId);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &mColourObjId);
		glBindBuffer(GL_ARRAY_BUFFER, mColourObjId);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec4), colours, GL_STATIC_DRAW);

		glGenBuffers(1, &mElementBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Cleanup()
	{
		GLboolean glbIsBuffer = glIsBuffer(mVertObjId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mVertObjId);
			mVertObjId = UINT_MAX;
		}
		glbIsBuffer = glIsBuffer(mColourObjId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mColourObjId);
			mColourObjId = UINT_MAX;
		}
		glbIsBuffer = glIsBuffer(mElementBufferId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mElementBufferId);
			mElementBufferId = UINT_MAX;
		}
		const GLboolean glbIsVA = glIsVertexArray(mVAO);
		if(glbIsVA==GL_TRUE) {
			glDeleteVertexArrays(1, &mVAO);
			mVAO = UINT_MAX;
		}
	}

	void DrawBox() {
		// bind shader program
		glUseProgram(mShaderProg);

		ViewMatrix = m_avatar.GetViewMatrix();
		ModelMatrix = m_avatar.GetModelMatrix();
		MVP = m_avatar.GetMVP();

		// Send our transformation to the currently bound shader in the "MVP" uniform
		glUniformMatrix4fv(MatrixID,		1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID,	1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID,	1, GL_FALSE, &ViewMatrix[0][0]);

		// bind buffers
		GLuint attributeIndex = 0;
		glBindVertexArray(mVAO);
		if(mVertObjId < UINT_MAX)
		{
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(attributeIndex);
			glBindBuffer(GL_ARRAY_BUFFER, mVertObjId);
			glVertexAttribPointer(
				attributeIndex,     // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				nullptr             // array buffer offset
			);
			++attributeIndex;
		}
		if(mColourObjId < UINT_MAX)
		{
			// 2nd attribute buffer : colours
			glEnableVertexAttribArray(attributeIndex);
			glBindBuffer(GL_ARRAY_BUFFER, mColourObjId);
			glVertexAttribPointer(
				attributeIndex,                   // attribute
				4,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				nullptr                           // array buffer offset
			);
			++attributeIndex;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferId);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,		// mode
			36,					// count
			GL_UNSIGNED_INT,	// type
			nullptr				// element array buffer offset
		);

		// release buffers
		attributeIndex = 0;
		if(mVertObjId != UINT_MAX) {
			glDisableVertexAttribArray(attributeIndex);
			++attributeIndex;
		}
		if(mColourObjId != UINT_MAX) {
			glDisableVertexAttribArray(attributeIndex);
			++attributeIndex;
		}
		glBindVertexArray(0);

		glUseProgram(0);
	}

	GLuint mShaderProg;
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;

	GLuint mVAO;
	GLuint mVertObjId;
	GLuint mColourObjId;
	GLuint mElementBufferId;

	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 MVP;

	static const glm::vec3 vertices[];
	static const glm::vec4 colours[];
	static const GLuint indices[];
};

// Create vertex buffer
const glm::vec3 BoxApp::vertices[] =
{
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, +1.0f, -1.0f),
	glm::vec3(+1.0f, +1.0f, -1.0f),
	glm::vec3(+1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, +1.0f),
	glm::vec3(-1.0f, +1.0f, +1.0f),
	glm::vec3(+1.0f, +1.0f, +1.0f),
	glm::vec3(+1.0f, -1.0f, +1.0f)
};


const glm::vec4 BoxApp::colours[] =
{
	WHITE,
	BLACK,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	MAGENTA
};

const GLuint BoxApp::indices[] = {
	// front face
	0, 1, 2,
	0, 2, 3,

	// back face
	4, 6, 5,
	4, 7, 6,

	// left face
	4, 5, 1,
	4, 1, 0,

	// right face
	3, 2, 6,
	3, 6, 7,

	// top face
	1, 5, 6,
	1, 6, 2,

	// bottom face
	4, 0, 3, 
	4, 3, 7
};

int SDL_main(int argc, char * argv[])
{	
	BoxApp theApp(argc, argv);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}
