//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------
#include <GL/glew.h>
#include <GL/gl.h>

#include "sdl.h"
#include "Oculus.h"
#include "Avatar.h"
#include "ShaderHelper.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {
	void ApplyGLMatrix(const OVR::Matrix4f& matrix)
	{
		glMultMatrixf(&(matrix.Transposed().M[0][0]));
	}
};

// handles basic movement in the scene.
Avatar::Avatar()
{
	m_oculus = NULL;
}

void Avatar::Start(const Oculus* oculus)
{
	m_oculus = oculus;
	m_speed = 10.0f;

	////////////////////////////////////////////////////////////////
	// load the patch terrain shader
	LoadShader(prog, "patch.vert", "patch.frag");

	// Get a handle for our "MVP" uniform(s)
	MatrixID		= glGetUniformLocation(prog, "MVP");
	ViewMatrixID	= glGetUniformLocation(prog, "V");
	ModelMatrixID	= glGetUniformLocation(prog, "M");
}

void Avatar::OnKeyDown(int key)
{
	switch(key)
	{
	case SDLK_UP:
		{
			m_velocity.z = -m_speed;
			break;
		}
	case SDLK_DOWN:
		{
			m_velocity.z = m_speed;
			break;
		}
	case SDLK_LEFT:
		{
			m_velocity.x = -m_speed;
			break;
		}
	case SDLK_RIGHT:
		{
			m_velocity.x = m_speed;
			break;
		}
	}
}

void Avatar::OnKeyUp(int key)
{
	switch(key)
	{
	case SDLK_UP:
		{
			m_velocity.z = 0.0f;
			break;
		}
	case SDLK_DOWN:
		{
			m_velocity.z = 0.0f;
			break;
		}
	case SDLK_LEFT:
		{
			m_velocity.x = 0.0f;
			break;
		}
	case SDLK_RIGHT:
		{
			m_velocity.x = 0.0f;
			break;
		}
	}
}

const OVR::Vector3f& Avatar::GetPosition() const 
{ 
	return m_position; 
}

void Avatar::SetPosition(const OVR::Vector3f& position)
{
	m_position = position;
}

void Avatar::Update(float dt)
{
	m_position += m_velocity * dt;
}

void Avatar::SetupCamera(OVR::Util::Render::StereoEye eye)
{
	const OVR::HMDInfo& hmd = m_oculus->GetHMDInfo();
	const OVR::Util::Render::StereoEyeParams& params = m_oculus->GetStereoConfig().GetEyeRenderParams(eye);
	glViewport(params.VP.x, params.VP.y, params.VP.w, params.VP.h);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	////////////////////////////////////////////////////////////////
	const float aspect = float(params.VP.w) / float(params.VP.h);

	OVR::Matrix4f proj((params.ViewAdjust * params.Projection).Transposed());
	glm::mat4 ProjectionMatrix;
	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			ProjectionMatrix[i][j] = proj.M[i][j];
		}
	}

	glm::mat4 ViewMatrix	= GetEyeView(eye);
	glm::mat4 ModelMatrix	= glm::mat4(1.0);
	glm::mat4 MVP			= ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(prog);

	// Send our transformation to the currently bound shader in the "MVP" uniform
	glUniformMatrix4fv(MatrixID,		1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID,	1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID,	1, GL_FALSE, &ViewMatrix[0][0]);
}

glm::mat4 Avatar::GetEyeView(OVR::Util::Render::StereoEye eye)
{
	static const glm::vec4 UpVector(0.0f, 1.0f, 0.0f, 1.0f);
	static const glm::vec4 ForwardVector(0.0f, 0.0f, -1.0f, 1.0f);
	static const glm::vec4 RightVector(1.0f, 0.0f, 0.0f, 1.0f);

	float yaw, pitch, roll;
	m_oculus->GetSensorOrientation(yaw, pitch, roll);
	glm::mat4 eye_rpy;
	glm::rotate(eye_rpy, yaw,	glm::vec3(1.0f, 0.0f, 0.0f));
	glm::rotate(eye_rpy, pitch, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::rotate(eye_rpy, roll,	glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 eye_pos		= glm::vec3(m_position.x, m_position.y, m_position.z);
	glm::vec3 eye_forward	= glm::vec3(eye_rpy * ForwardVector);
	glm::vec3 eye_up		= glm::vec3(eye_rpy * UpVector);
	glm::vec3 eye_right		= glm::vec3(eye_rpy * RightVector);
	glm::mat4 eye_view		= glm::lookAt(eye_pos, eye_pos + eye_forward, eye_up); 
	return eye_view;
}
