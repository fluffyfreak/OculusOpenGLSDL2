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

	void Start(const Oculus* oculus);

	void OnKeyDown(int key);

	void OnKeyUp(int key);

	const OVR::Vector3f& GetPosition() const;

	void SetPosition(const OVR::Vector3f& position);

	void Update(float dt);

	void SetupCamera(OVR::Util::Render::StereoEye eye);

	OVR::Matrix4f GetEyeView(OVR::Util::Render::StereoEye eye);

private:
	float m_speed;
	OVR::Vector3f m_position;
	OVR::Vector3f m_velocity;
	const Oculus* m_oculus;
};

#endif // _AVATAR_H_