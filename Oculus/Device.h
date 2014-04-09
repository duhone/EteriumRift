#pragma once

namespace RE{
namespace OculusNative
{
	class DeviceNative;
}
}

namespace RE{
namespace Oculus
{
	public value struct DeviceInfo
	{
		int DisplayID;
		System::String^ DeviceName;
		array<int>^ Resolution;
	};

	public value struct StereoSettings
	{
		//These settings are the same for both eyes
		float FovY;
		float AspectRatio;
		//These settings are diffrent for each eye
		float ProjectionCenterOffset;
		float ViewTranslation;
		array<float>^ DistortionK;
		array<float>^ DistortionCenterOffset;
		array<float>^ ChromAberration;
		float DistortionScale;
		//array<float>^ Ortho;
	};
	
	public enum class EYE
	{
		LEFT_EYE,
		RIGHT_EYE
	};

	public ref class Device
	{
	public:
		Device(RE::OculusNative::DeviceNative* _impl);
		~Device();
		!Device();

		property DeviceInfo DeviceInformation
		{
			DeviceInfo get() { return m_DeviceInfo; }
		}
		property StereoSettings SSettings
		{
			StereoSettings get() { return m_StereoSettings; }
		}

		void Update();
		void SelectEye(EYE _eye);
		float Calc2DOffset(float _depth);		
		array<float>^ GetOrientation() { return m_Orientation; }
		void ResetOrientation();
	private:
		void UpdateStereoSettings();

		DeviceInfo m_DeviceInfo;
		StereoSettings m_StereoSettings;
		RE::OculusNative::DeviceNative* m_Pimpl;
		array<float>^ m_Orientation;
	};
}
}