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

#include <glm/gtc/matrix_transform.hpp>

// handles basic movement in the scene.
Avatar::Avatar()
{
	m_oculus = NULL;
}

void Avatar::Start(const Oculus* oculus)
{
	m_oculus = oculus;
	m_speed = 10.0f;
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
		
	////////////////////////////////////////////////////////////////
	const float aspect = float(params.VP.w) / float(params.VP.h);

	OVR::Matrix4f proj((params.ViewAdjust * params.Projection));//.Transposed());
	glm::mat4 ProjectionMatrix;
	for(int i=0; i<4; ++i) {
		for(int j=0; j<4; ++j) {
			ProjectionMatrix[i][j] = proj.M[j][i];
		}
	}

	ViewMatrix	= GetEyeView(eye);
	ModelMatrix	= glm::mat4(1.0);
	MVP			= ProjectionMatrix * ViewMatrix * ModelMatrix;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

glm::mat4 Avatar::GetEyeView(OVR::Util::Render::StereoEye eye)
{
	static const glm::vec4 UpVector(0.0f, 1.0f, 0.0f, 1.0f);
	static const glm::vec4 ForwardVector(0.0f, 0.0f, -1.0f, 1.0f);
	static const glm::vec4 RightVector(1.0f, 0.0f, 0.0f, 1.0f);

	float yaw, pitch, roll;
	m_oculus->GetSensorOrientation(yaw, pitch, roll);
	glm::mat4 eye_rpy;
	
	eye_rpy = glm::rotate(eye_rpy, OVR::RadToDegree(yaw),	glm::vec3(0.0f, 1.0f, 0.0f));
	eye_rpy = glm::rotate(eye_rpy, OVR::RadToDegree(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	eye_rpy = glm::rotate(eye_rpy, OVR::RadToDegree(roll),	glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 eye_pos		= glm::vec3(m_position.x, m_position.y, m_position.z);
	glm::vec3 eye_forward	= glm::vec3(eye_rpy * ForwardVector);
	glm::vec3 eye_up		= glm::vec3(eye_rpy * UpVector);
	glm::vec3 eye_right		= glm::vec3(eye_rpy * RightVector);
	glm::mat4 eye_view		= glm::lookAt(eye_pos, eye_pos + eye_forward, eye_up); 
	return eye_view;
}
