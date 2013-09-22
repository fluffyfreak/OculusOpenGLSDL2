//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _OCULUS_H_
#define _OCULUS_H_

#include "OVR.h"

#pragma comment(lib, "libovr.lib")

class Oculus : public OVR::MessageHandler
{
public:
	Oculus();
	~Oculus();

	void start();

	bool get_sensor_position(float& x, float& y, float& z) const;

	bool get_sensor_orientation(float& yaw, float& pitch, float& roll) const;

	const OVR::HMDInfo& get_HMD_info() const
	{
		return m_HMD_info;
	}

	OVR::Util::Render::StereoConfig get_stereo_config() const 
	{ 
		return m_stereo_config; 
	}

	virtual void OnMessage(const OVR::Message& msg);

private:
	OVR::Ptr<OVR::DeviceManager>			m_device_manager;
    OVR::Ptr<OVR::SensorDevice>				m_sensor;
    OVR::Ptr<OVR::HMDDevice>				m_HMD;
    OVR::HMDInfo							m_HMD_info;
	OVR::SensorFusion						m_sensor_fusion;
    OVR::Util::Render::StereoConfig			m_stereo_config;
};

#endif // _OCULUS_H_