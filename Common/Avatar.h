//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _AVATAR_H_
#define _AVATAR_H_

#include "OVR.h"
#include <glm/glm.hpp>

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

	glm::mat4 GetEyeView(OVR::Util::Render::StereoEye eye);

	const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
	const glm::mat4& GetModelMatrix() const { return ModelMatrix; }
	const glm::mat4& GetMVP() const { return MVP; }

private:
	float m_speed;
	OVR::Vector3f m_position;
	OVR::Vector3f m_velocity;
	const Oculus* m_oculus;

	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 MVP;
};

#endif // _AVATAR_H_