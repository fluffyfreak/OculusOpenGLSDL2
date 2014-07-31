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
#include "VertexArray.h"
#include "ShaderHelper.h"

namespace {
	const std::string vertstr("PosCol.vert");
	const std::string fragstr("PosCol.frag");
};

class ShapesApp : public BaseApp
{
public:
	ShapesApp(int argc, char * argv[]) : 
		BaseApp(),
		mVAO(UINT_MAX),
		mVertId(UINT_MAX),
		mColourId(UINT_MAX),
		mIndicesId(UINT_MAX),
		mNumIndices(0)
	{
	}

	virtual ~ShapesApp() {
		Cleanup();
	}

	virtual bool Init() { 
		InitShapes();
		LoadShader(mShaderProg, vertstr, fragstr);
		return true; 
	}

	virtual void Update() {};
	virtual void HandleEvent(const SDL_Event &sdl_event) {}
	virtual void DrawScene() {
		glDisable(GL_CULL_FACE); // hack!!!

		// draw left viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Left);
		DrawShapes();

		// draw right viewpoint
		m_avatar.SetupCamera(OVR::Util::Render::StereoEye_Right);
		DrawShapes();
	}; 

private:
	void InitShapes() {/*
		// generate geometry
		MeshData *pMesh = CreateGrid(160.0f, 160.0f, 50, 50);
		mNumIndices = pMesh->mIndices.size();

		// modify the heights and generate colours from the heights
		VertexArray *pv = pMesh->mVerts;
		for(Uint32 i = 0; i<pv->position.size(); ++i) {
			glm::vec3 &pos = pv->position[i];
			pos.y = GetHeight(pos.x, pos.z);
			glm::vec4 color;

			if (pos.y < -10.0f) {
				color = glm::vec4(1.0f, 1.0f, 0.96f, 0.62f);
			} else if (pos.y < 5.0f) {
				color = glm::vec4(1.0f, 0.48f, 0.77f, 0.46f);
			} else if (pos.y < 12.0f) {
				color = glm::vec4(1.0f, 0.1f, 0.48f, 0.19f);
			} else if (pos.y < 20.0f) {
				color = glm::vec4(1.0f, 0.45f, 0.39f, 0.34f);
			} else {
				color = glm::vec4(1,1,1,1);
			}
			pv->diffuse[i] = color;
		}

		// now generate our rendering buffers
		Cleanup();
		const Uint32 numPos = pv->position.size();
		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		glGenBuffers(1, &mVertId);
		glBindBuffer(GL_ARRAY_BUFFER, mVertId);
		glBufferData(GL_ARRAY_BUFFER, numPos * sizeof(glm::vec3), &pv->position[0], GL_STATIC_DRAW);

		glGenBuffers(1, &mColourId);
		glBindBuffer(GL_ARRAY_BUFFER, mColourId);
		glBufferData(GL_ARRAY_BUFFER, numPos * sizeof(glm::vec4), &pv->diffuse[0], GL_STATIC_DRAW);

		glGenBuffers(1, &mIndicesId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(GLuint), &pMesh->mIndices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// cleanup our local mesh geometry once we no longer need it
		delete pMesh;*/
	}

	void DrawShapes() {
		// bind shader program
		glUseProgram(mShaderProg);

		// bind buffers
		GLuint attributeIndex = 0;
		glBindVertexArray(mVAO);
		if(mVertId < UINT_MAX)
		{
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(attributeIndex);
			glBindBuffer(GL_ARRAY_BUFFER, mVertId);
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
		if(mColourId < UINT_MAX)
		{
			// 2nd attribute buffer : colours
			glEnableVertexAttribArray(attributeIndex);
			glBindBuffer(GL_ARRAY_BUFFER, mColourId);
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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesId);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,		// mode
			mNumIndices,		// count
			GL_UNSIGNED_INT,	// type
			nullptr				// element array buffer offset
		);

		// release buffers
		attributeIndex = 0;
		if(mVertId != UINT_MAX) {
			glDisableVertexAttribArray(attributeIndex);
			++attributeIndex;
		}
		if(mColourId != UINT_MAX) {
			glDisableVertexAttribArray(attributeIndex);
			++attributeIndex;
		}
		glBindVertexArray(0);

		glUseProgram(0);
	}

	void Cleanup()
	{
		GLboolean glbIsBuffer = glIsBuffer(mVertId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mVertId);
			mVertId = UINT_MAX;
		}
		glbIsBuffer = glIsBuffer(mColourId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mColourId);
			mColourId = UINT_MAX;
		}
		glbIsBuffer = glIsBuffer(mIndicesId);
		if(glbIsBuffer==GL_TRUE) {
			glDeleteBuffers(1, &mIndicesId);
			mIndicesId = UINT_MAX;
		}
		const GLboolean glbIsVA = glIsVertexArray(mVAO);
		if(glbIsVA==GL_TRUE) {
			glDeleteVertexArrays(1, &mVAO);
			mVAO = UINT_MAX;
		}
	}

	float GetHeight(const float x, const float z) const {
		return 0.3f*(z*sin(0.1f*x) + x*cos(0.1f*z));
	}

	GLuint mShaderProg;
	GLuint mVAO;
	GLuint mVertId;
	GLuint mColourId;
	GLuint mIndicesId;

	Uint32 mNumIndices;
};

int SDL_main(int argc, char * argv[])
{	
	ShapesApp theApp(argc, argv);

	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}