#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <array>

namespace OVR{
	class HMDDevice;
	class SensorDevice;
}

namespace RE{
namespace OculusNative
{
	struct DeviceInfo
	{
		int DisplayID;
		std::string DeviceName;
		int Resolution[2];
	};

	struct StereoSettings
	{
		//These settings are the same for both eyes
		float FovY;
		float AspectRatio;
		//These settings are diffrent for each eye
		float ProjectionCenterOffset;
		float ViewTranslation;
		float DistortionK[4];
		float DistortionCenterOffset[2];
		float DistortionScale;
		float ChromAberration[4];
		//float Ortho[4][4];
	};
	
	class DeviceNative
	{
	public:
		enum EYE
		{
			LEFT_EYE,
			RIGHT_EYE
		};

		DeviceNative(OVR::HMDDevice* _device, OVR::SensorDevice* _sensor);
		~DeviceNative(void);

		const DeviceInfo& GetDeviceInfo() const;
		const StereoSettings& GetStereoSettings() const;
		void SelectEye(EYE _eye);
		float Calc2DOffset(float _depth);
		std::array<float, 4> GetOrientation();
		void ResetOrientation();
	private:
		std::unique_ptr<class DeviceNativeImpl> m_Pimpl;
	};
}}

