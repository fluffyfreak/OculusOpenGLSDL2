//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _AVATAR_H_
#define _AVATAR_H_

#include "OVR.h"

// forward declarations
class Oculus;

// handles basic movement in the scene.
class Avatar
{
public:
	Avatar();

	void start(const Oculus* oculus);

	void on_key_down(int key);

	void on_key_up(int key);

	const OVR::Vector3f& get_position() const;

	void set_position(const OVR::Vector3f& position);

	void update(float dt);

	void setup_camera(OVR::Util::Render::StereoEye eye);

	OVR::Matrix4f get_eye_view(OVR::Util::Render::StereoEye eye);

private:
	float m_speed;
	OVR::Vector3f m_position;
	OVR::Vector3f m_velocity;
	const Oculus* m_oculus;
};

#endif // _AVATAR_H_