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
	public enum class EYE
	{
		LEFT_EYE,
		RIGHT_EYE
	};
	
	public value struct Pose
	{
		Microsoft::Xna::Framework::Quaternion Orientation;
		Microsoft::Xna::Framework::Vector3 Position;
	};

	public value struct DeviceInfo
	{
		int DisplayID;
		System::String^ DeviceName;
		Microsoft::Xna::Framework::Point Resolution;
		Microsoft::Xna::Framework::Point RecommendedTextureResolutionLeft;
		Microsoft::Xna::Framework::Point RecommendedTextureResolutionRight;
		EYE PreferredFirstEye;
	};
	
	public value struct DeviceConfig
	{
		int DisplayID;
		Microsoft::Xna::Framework::Game^ GameInstance;
		Microsoft::Xna::Framework::Graphics::GraphicsDevice^ Device;
		Microsoft::Xna::Framework::Graphics::RenderTarget2D^ LeftRenderTexture;
		Microsoft::Xna::Framework::Graphics::RenderTarget2D^ RightRenderTexture;
		Microsoft::Xna::Framework::Point RenderTargetResolution; //generally the resoultion of the kit, i.e. DeviceInfo::Resolution
		Microsoft::Xna::Framework::Point RenderTextureResolutionLeft;
		Microsoft::Xna::Framework::Point RenderTextureResolutionRight;
		float zNear;
		float zFar;
	};

	public value struct StereoSettings
	{
		float FovY;
		Microsoft::Xna::Framework::Matrix LeftProjection;
		Microsoft::Xna::Framework::Matrix RightProjection;
		Microsoft::Xna::Framework::Vector3 LeftViewAdjust;
		Microsoft::Xna::Framework::Vector3 RightViewAdjust;
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
		
		Pose GetPose();
		void ResetPose();
		void BeginFrame();
		void EndFrame();
		void BeginEyeRender(EYE a_eye);
		void EndEyeRender();
		float Calc2DOffset(float _depth);
	private:
		void UpdateStereoSettings();

		DeviceInfo m_DeviceInfo;
		StereoSettings m_StereoSettings;
		RE::OculusNative::DeviceNative* m_Pimpl;
		Pose m_Pose;
	};
}
}