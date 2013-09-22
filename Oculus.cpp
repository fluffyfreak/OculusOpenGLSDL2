//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------
#include "Oculus.h"
#include "Logging.h"

Oculus::Oculus() 
{}

Oculus::~Oculus()
{
	OVR::MessageHandler::RemoveHandlerFromDevices();
	m_sensor.Clear();
	m_HMD.Clear();
	m_device_manager.Clear();
		
	OVR::System::Destroy();
	OVR_DEBUG_STATEMENT(_CrtDumpMemoryLeaks());
}

void Oculus::start()
{
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
		
	m_device_manager = *OVR::DeviceManager::Create();
		
	// We'll handle it's messages in this case.
	m_device_manager->SetMessageHandler(this);

	// Release Sensor/HMD in case this is a retry.
	m_sensor.Clear();
	m_HMD.Clear();
		
	m_HMD = *m_device_manager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
	if (m_HMD)
	{
		m_sensor = *m_HMD->GetSensor();

		// This will initialize HMDInfo with information about configured IPD,
		// screen size and other variables needed for correct projection.
		// We pass HMD DisplayDeviceName into the renderer to select the
		// correct monitor in full-screen mode.
		m_HMD->GetDeviceInfo(&m_HMD_info);

		m_stereo_config.SetHMDInfo(m_HMD_info);
		m_stereo_config.SetFullViewport(OVR::Util::Render::Viewport(0,0, m_HMD_info.HResolution, m_HMD_info.VResolution));
		m_stereo_config.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
			
		if (m_HMD_info.HScreenSize > 0.140f) // 7"
			m_stereo_config.SetDistortionFitPointVP(-1.0f, 0.0f);
		else
			m_stereo_config.SetDistortionFitPointVP(0.0f, 1.0f);
	}
	else
	{            
		// If we didn't detect an HMD, try to create the sensor directly.
		// This is useful for debugging sensor interaction; it is not needed in
		// a shipping app.
		m_sensor = m_device_manager->EnumerateDevices<OVR::SensorDevice>().CreateDevice();
	}

	const char* detectionMessage=0;
	if (!m_HMD && !m_sensor)
		detectionMessage = "Oculus Rift not detected.";
	else if (!m_HMD)
		detectionMessage = "Oculus Sensor detected; HMD Display not detected.";
	else if (!m_sensor)
		detectionMessage = "Oculus HMD Display detected; Sensor not detected.";
	else if (m_HMD_info.DisplayDeviceName[0] == '\0')
		detectionMessage = "Oculus Sensor detected; HMD display EDID not detected.";
	else
		detectionMessage = 0;

	if (detectionMessage)
	{
		::MessageBoxA(0, detectionMessage, "Oculus Rift ERROR", MB_OK);
	}
		
	if (m_sensor)
	{
		// We need to attach sensor to SensorFusion object for it to receive 
		// body frame messages and update orientation. SFusion.GetOrientation() 
		// is used in OnIdle() to orient the view.
		m_sensor_fusion.AttachToSensor(m_sensor);
		m_sensor_fusion.SetPredictionEnabled(true);
		m_sensor_fusion.SetDelegateMessageHandler(this);			
	}
}

bool Oculus::get_sensor_position(float& x, float& y, float& z) const
{
	// TODO. Some form of positional tracking.
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	return false;
}

bool Oculus::get_sensor_orientation(float& yaw, float& pitch, float& roll) const
{
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
	
	if(!m_sensor_fusion.IsAttachedToSensor())
		return false;

	OVR::Quatf hmdOrient = m_sensor_fusion.GetOrientation();
	hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
	return true;
}

//virtual 
void Oculus::OnMessage(const OVR::Message& msg)
{
	if(msg.pDevice == m_sensor)
	{
		if (msg.Type == OVR::Message_BodyFrame)
		{
		}
	}
	else if(msg.pDevice == m_device_manager)
	{
		if (msg.Type == OVR::Message_DeviceAdded)
		{
			trace("DeviceManager reported device added.");
		}
		else if (msg.Type == OVR::Message_DeviceRemoved)
		{
			trace("DeviceManager reported device removed.");
		}
		else if (msg.Type == OVR::Message_DeviceAdded)
		{
			trace("Sensor reported device added.");
		}
		else if (msg.Type == OVR::Message_DeviceRemoved)
		{
			trace("Sensor reported device removed.");
		}
	}
}
