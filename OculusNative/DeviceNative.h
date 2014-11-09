#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <array>

//struct ovrHmdDesc;

struct IDirect3DDevice9;
struct IDirect3DSwapChain9;
struct IDirect3DTexture9;

namespace RE{
namespace OculusNative
{
	enum EYE
	{
		LEFT_EYE,
		RIGHT_EYE
	};

	struct DeviceInfo
	{
		int DisplayID;
		std::string DeviceName;
		int Resolution[2];
		int RecommendedTextureResolutionLeft[2];
		int RecommendedTextureResolutionRight[2];
		EYE PreferredFirstEye;
	};

	struct StereoSettings
	{
		float Fov;
		float LeftProjection[16];
		float RightProjection[16];
		float LeftViewAdjust[3];
		float RightViewAdjust[3];
	};
	
	struct Pose
	{
		Pose()
		{
			Orientation[0] = Orientation[1] = Orientation[2] = Orientation[3] = 0;
			Position[0] = Position[1] = Position[2] = 0;
		}
		std::array<float, 4> Orientation;
		std::array<float, 3> Position;
	};
	
	struct DeviceConfig
	{
		int DisplayID;
		void* WindowHandle;
		IDirect3DDevice9* Device;
		IDirect3DTexture9* LeftRenderTexture;
		IDirect3DTexture9* RightRenderTexture;
		int RenderTargetResolution[2]; //generally the resoultion of the kit, i.e. DeviceInfo::Resolution
		int RenderTextureResolutionLeft[2];
		int RenderTextureResolutionRight[2];
		float zNear;
		float zFar;
	};

	class DeviceNative
	{
	public:

		DeviceNative(void* a_device, DeviceConfig a_cfg);
		~DeviceNative(void);

		const DeviceInfo& GetDeviceInfo() const;
		const StereoSettings& GetStereoSettings() const;
		float Calc2DOffset(float a_depth);
		Pose GetPose();
		void ResetPose();
		void BeginFrame();
		void EndFrame();
		void BeginEyeRender(EYE a_eye);
		void EndEyeRender();

	private:
		std::unique_ptr<class DeviceNativeImpl> m_Pimpl;
	};
}}

