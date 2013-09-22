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
	void apply_gl_matrix(const OVR::Matrix4f& matrix)
	{
		glMultMatrixf(&(matrix.Transposed().M[0][0]));
	}
};

// handles basic movement in the scene.
Avatar::Avatar()
{
	m_oculus = NULL;
}

void Avatar::start(const Oculus* oculus)
{
	m_oculus = oculus;
	m_speed = 10.0f;
}

void Avatar::on_key_down(int key)
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

void Avatar::on_key_up(int key)
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

const OVR::Vector3f& Avatar::get_position() const 
{ 
	return m_position; 
}

void Avatar::set_position(const OVR::Vector3f& position)
{
	m_position = position;
}

void Avatar::update(float dt)
{
	m_position += m_velocity * dt;
}

void Avatar::setup_camera(OVR::Util::Render::StereoEye eye)
{
	const OVR::HMDInfo& hmd = m_oculus->get_HMD_info();
	const OVR::Util::Render::StereoEyeParams& params = m_oculus->get_stereo_config().GetEyeRenderParams(eye);
	glViewport(params.VP.x, params.VP.y, params.VP.w, params.VP.h);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	apply_gl_matrix(params.ViewAdjust);
	apply_gl_matrix(params.Projection);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	OVR::Matrix4f eye_view = get_eye_view(eye);
	apply_gl_matrix(eye_view);
}

OVR::Matrix4f Avatar::get_eye_view(OVR::Util::Render::StereoEye eye)
{
	static const OVR::Vector3f UpVector(0.0f, 1.0f, 0.0f);
	static const OVR::Vector3f ForwardVector(0.0f, 0.0f, -1.0f);
	static const OVR::Vector3f RightVector(1.0f, 0.0f, 0.0f);

	float yaw, pitch, roll;
	m_oculus->get_sensor_orientation(yaw, pitch, roll);
	OVR::Matrix4f eye_rpy = OVR::Matrix4f::RotationY(yaw) * OVR::Matrix4f::RotationX(pitch) * OVR::Matrix4f::RotationZ(roll);

	OVR::Vector3f eye_pos = m_position;
	OVR::Vector3f eye_forward = eye_rpy.Transform(ForwardVector);
	OVR::Vector3f eye_up = eye_rpy.Transform(UpVector);
	OVR::Vector3f eye_right = eye_rpy.Transform(RightVector);
	OVR::Matrix4f eye_view = OVR::Matrix4f::LookAtRH(eye_pos, eye_pos + eye_forward, eye_up); 
	return eye_view;
}
