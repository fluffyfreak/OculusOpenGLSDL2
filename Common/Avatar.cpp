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
}

void Avatar::OnKeyDown(int key)
{
	switch(key)
	{
	case SDLK_w:
	case SDLK_UP:
		{
			m_velocity.z = -m_speed;
			break;
		}
	case SDLK_s:
	case SDLK_DOWN:
		{
			m_velocity.z = m_speed;
			break;
		}
	case SDLK_a:
	case SDLK_LEFT:
		{
			m_velocity.x = -m_speed;
			break;
		}
	case SDLK_d:
	case SDLK_RIGHT:
		{
			m_velocity.x = m_speed;
			break;
		}
	// vertical
	case SDLK_q: // down
		{
			m_velocity.y = -m_speed;
			break;
		}
	case SDLK_e: // up
		{
			m_velocity.y = m_speed;
			break;
		}
	}
}

void Avatar::OnKeyUp(int key)
{
	switch(key)
	{
	case SDLK_w:
	case SDLK_UP:
		{
			m_velocity.z = 0.0f;
			break;
		}
	case SDLK_s:
	case SDLK_DOWN:
		{
			m_velocity.z = 0.0f;
			break;
		}
	case SDLK_a:
	case SDLK_LEFT:
		{
			m_velocity.x = 0.0f;
			break;
		}
	case SDLK_d:
	case SDLK_RIGHT:
		{
			m_velocity.x = 0.0f;
			break;
		}
	// vertical
	case SDLK_q: // down
		{
			m_velocity.y = 0.0f;
			break;
		}
	case SDLK_e: // up
		{
			m_velocity.y = 0.0f;
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
	ApplyGLMatrix(params.ViewAdjust);
	ApplyGLMatrix(params.Projection);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	OVR::Matrix4f eye_view = GetEyeView(eye);
	ApplyGLMatrix(eye_view);
}

OVR::Matrix4f Avatar::GetEyeView(OVR::Util::Render::StereoEye eye)
{
	static const OVR::Vector3f UpVector(0.0f, 1.0f, 0.0f);
	static const OVR::Vector3f ForwardVector(0.0f, 0.0f, -1.0f);
	static const OVR::Vector3f RightVector(1.0f, 0.0f, 0.0f);

	float yaw, pitch, roll;
	m_oculus->GetSensorOrientation(yaw, pitch, roll);
	OVR::Matrix4f eye_rpy = OVR::Matrix4f::RotationY(yaw) * OVR::Matrix4f::RotationX(pitch) * OVR::Matrix4f::RotationZ(roll);

	OVR::Vector3f eye_pos = m_position;
	OVR::Vector3f eye_forward = eye_rpy.Transform(ForwardVector);
	OVR::Vector3f eye_up = eye_rpy.Transform(UpVector);
	OVR::Vector3f eye_right = eye_rpy.Transform(RightVector);
	OVR::Matrix4f eye_view = OVR::Matrix4f::LookAtRH(eye_pos, eye_pos + eye_forward, eye_up); 
	return eye_view;
}
