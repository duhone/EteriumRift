#include "StdAfx.h"
#include "DeviceNative.h"
#include "OVR.h"
#include "..\src\OVR_CAPI_D3D.h"
#include "DeviceInfoConversions.h"
#include <atlbase.h>

using namespace RE::OculusNative;
using namespace std;

namespace RE{
namespace OculusNative{
	class DeviceNativeImpl
	{
	public:
		DeviceNativeImpl(ovrHmd a_device, DeviceConfig a_cfg);
		~DeviceNativeImpl();
		
		const DeviceInfo& GetDeviceInfo() const { return m_deviceInfo; }
		const StereoSettings& GetStereoSettings() const { return m_stereoSettings; }
		float Calc2DOffset(float _depth);
		Pose GetPose();		
		void ResetPose();
		void BeginFrame();
		void EndFrame();
		void BeginEyeRender(EYE a_eye);
		void EndEyeRender();

	private:
		DeviceInfo m_deviceInfo;
		ovrHmd m_ovrDevice;
		DeviceConfig m_cfg;
		StereoSettings m_stereoSettings;
		ovrFrameTiming m_frameTiming;
		ovrEyeType m_currentEye;
		ovrPosef m_renderEyePoses[2];
		ovrMatrix4f m_projections[2];
		ovrEyeRenderDesc m_renderDesc[2];
	};
}}

DeviceNativeImpl::DeviceNativeImpl(ovrHmd a_device, DeviceConfig a_cfg) :
	m_ovrDevice(a_device), m_cfg(a_cfg), m_currentEye(ovrEyeType::ovrEye_Left)
{	
	assert(a_device);
	
	//quality only used for creating recommended texture rez, which we won't use. so don't force users to send quality to us again.
	//user will send us actual resolution of his targets, which is free to ignore recomendation anyway. so just pass 1.0 as a hack.
	m_deviceInfo = Convert(m_ovrDevice, 1.0f);
		
	ovrHmd_AttachToWindow(m_ovrDevice, a_cfg.WindowHandle, nullptr, nullptr);

	ovrD3D9Config cfg;
	cfg.D3D9.Header.API = ovrRenderAPI_D3D9;
	cfg.D3D9.Header.RTSize = OVR::Sizei(a_cfg.RenderTargetResolution[0], a_cfg.RenderTargetResolution[1]);
	cfg.D3D9.Header.Multisample = 1;
	cfg.D3D9.pDevice = a_cfg.Device;
	a_cfg.Device->GetSwapChain(0, &cfg.D3D9.pSwapChain);
	
    ovrFovPort eyeFov[2] = { m_ovrDevice->DefaultEyeFov[0], m_ovrDevice->DefaultEyeFov[1] } ;
	
	if (!ovrHmd_ConfigureRendering(m_ovrDevice, &cfg.Config, ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp |
		ovrDistortionCap_Overdrive | ovrDistortionCap_Vignette | ovrDistortionCap_SRGB | ovrDistortionCap_HqDistortion,
		eyeFov, &m_renderDesc[0]))
		throw runtime_error("Failed to configure Oculus rendering");
		
    ovrHmd_SetEnabledCaps(m_ovrDevice, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
	
	ovrHmd_ConfigureTracking(a_device, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);

	ResetPose();
	ovrHmd_DismissHSWDisplay(m_ovrDevice);	
	
	m_projections[0] = ovrMatrix4f_Projection(m_renderDesc[0].Fov, m_cfg.zNear, m_cfg.zFar, true);
	memcpy(m_stereoSettings.LeftProjection, &m_projections[0], sizeof(float)*16);
	m_projections[1] = ovrMatrix4f_Projection(m_renderDesc[1].Fov, m_cfg.zNear, m_cfg.zFar, true);
	memcpy(m_stereoSettings.RightProjection, &m_projections[1], sizeof(float)*16);

	memcpy(m_stereoSettings.LeftViewAdjust, &m_renderDesc[0].HmdToEyeViewOffset, sizeof(float)*3);
	memcpy(m_stereoSettings.RightViewAdjust, &m_renderDesc[1].HmdToEyeViewOffset, sizeof(float)*3);

}

DeviceNativeImpl::~DeviceNativeImpl()
{	
	ovrHmd_Destroy(m_ovrDevice);
}

//A hack I'm using to get pixel offsets for XNA sprite batches to give some depth to 2d elements that aren't tied to a
//3d element. i.e. menus and a few other things.
float DeviceNativeImpl::Calc2DOffset(float a_depth)
{
	const ovrVector2f orthoScale   = OVR::Vector2f(1.0f) / OVR::Vector2f(m_renderDesc[m_currentEye].PixelsPerTanAngleAtCenter);
	OVR::Matrix4f proj = ovrMatrix4f_OrthoSubProjection(m_projections[m_currentEye], orthoScale, a_depth, m_renderDesc[m_currentEye].HmdToEyeViewOffset.x);
	auto result = (proj.Transform(OVR::Vector4f(0, 0, 0, 1)).x);
	return result*m_cfg.RenderTextureResolutionLeft[0];
}

Pose DeviceNativeImpl::GetPose()
{
	Pose result;
	//This is not ideal, but was easier to integrate into Eterium for now. GetPose meant to be called once during
	//.net library update and cached in managed land. same pose for both eyes.
	auto trackState = ovrHmd_GetTrackingState(m_ovrDevice, 0.0f);
	m_renderEyePoses[0] = trackState.HeadPose.ThePose;
	m_renderEyePoses[1] = m_renderEyePoses[0];
	
	result.Orientation[0] = m_renderEyePoses[0].Orientation.x;
	result.Orientation[1] = m_renderEyePoses[0].Orientation.y;
	result.Orientation[2] = m_renderEyePoses[0].Orientation.z;
	result.Orientation[3] = m_renderEyePoses[0].Orientation.w;

	result.Position[0] = m_renderEyePoses[0].Position.x;
	result.Position[1] = m_renderEyePoses[0].Position.y;
	result.Position[2] = m_renderEyePoses[0].Position.z;

	/*
	char temp[256];
	sprintf_s(temp, "Orientation %0.2f %0.2f %0.2f %0.2f\n", result[0], result[1], result[2], result[3]);
	OutputDebugStringA(temp);
	*/
	return result;
}

void DeviceNativeImpl::ResetPose()
{
	ovrHmd_RecenterPose(m_ovrDevice);
}

void DeviceNativeImpl::BeginFrame()
{
	m_frameTiming = ovrHmd_BeginFrame(m_ovrDevice, 0); 
}

void DeviceNativeImpl::EndFrame()
{
	CComPtr<IDirect3DVertexDeclaration9> currentDecl;
	m_cfg.Device->GetVertexDeclaration(&currentDecl);
	DWORD colorOp, colorArg1, colorArg2;
	m_cfg.Device->GetTextureStageState(0, D3DTSS_COLOROP, &colorOp);
	m_cfg.Device->GetTextureStageState(0, D3DTSS_COLORARG1, &colorArg1);
	m_cfg.Device->GetTextureStageState(0, D3DTSS_COLORARG2, &colorArg2);

	ovrRecti leftViewport, rightViewport;
	leftViewport.Pos.x = 0;
	leftViewport.Pos.y = 0;
	leftViewport.Size.w = m_cfg.RenderTextureResolutionLeft[0];
	leftViewport.Size.h = m_cfg.RenderTextureResolutionLeft[1];
	rightViewport.Pos.x = 0;
	rightViewport.Pos.y = 0;
	rightViewport.Size.w = m_cfg.RenderTextureResolutionRight[0];
	rightViewport.Size.h = m_cfg.RenderTextureResolutionRight[1];

	ovrD3D9Texture eyeTextures[2];
	eyeTextures[0].D3D9.Header.API = ovrRenderAPI_D3D9;
	eyeTextures[0].D3D9.Header.RenderViewport = leftViewport;
	eyeTextures[0].D3D9.Header.TextureSize = leftViewport.Size;
	eyeTextures[0].D3D9.pTexture = m_cfg.LeftRenderTexture;
	eyeTextures[1].D3D9.Header.API = ovrRenderAPI_D3D9;
	eyeTextures[1].D3D9.Header.RenderViewport = rightViewport;
	eyeTextures[1].D3D9.Header.TextureSize = rightViewport.Size;
	eyeTextures[1].D3D9.pTexture = m_cfg.RightRenderTexture;

	ovrHmd_EndFrame(m_ovrDevice, m_renderEyePoses, &eyeTextures[0].Texture);

	m_cfg.Device->SetVertexDeclaration(currentDecl);
	m_cfg.Device->SetTextureStageState(0, D3DTSS_COLOROP, colorOp);
	m_cfg.Device->SetTextureStageState(0, D3DTSS_COLORARG1, colorArg1);
	m_cfg.Device->SetTextureStageState(0, D3DTSS_COLORARG2, colorArg2);
}

void DeviceNativeImpl::BeginEyeRender(EYE a_eye)
{
	m_currentEye = a_eye == EYE::LEFT_EYE ? ovrEyeType::ovrEye_Left : ovrEyeType::ovrEye_Right;
	//m_renderEyePoses[m_currentEye] = ovrHmd_GetEyePose(m_ovrDevice, m_currentEye);
}

void DeviceNativeImpl::EndEyeRender()
{
	
}

DeviceNative::DeviceNative(void* a_device, DeviceConfig a_cfg) : 
	m_Pimpl(new DeviceNativeImpl((ovrHmd)a_device, a_cfg))
{
}


DeviceNative::~DeviceNative(void)
{
}

const DeviceInfo& DeviceNative::GetDeviceInfo() const
{
	return m_Pimpl->GetDeviceInfo();
}

const StereoSettings& DeviceNative::GetStereoSettings() const
{
	return m_Pimpl->GetStereoSettings();
}

float DeviceNative::Calc2DOffset(float _depth)
{
	return m_Pimpl->Calc2DOffset(_depth);
}

Pose DeviceNative::GetPose()
{
	return m_Pimpl->GetPose();
}

void DeviceNative::ResetPose()
{
	m_Pimpl->ResetPose();
}

void DeviceNative::BeginFrame()
{
	m_Pimpl->BeginFrame();
}

void DeviceNative::EndFrame()
{
	m_Pimpl->EndFrame();
}

void DeviceNative::BeginEyeRender(EYE a_eye)
{
	m_Pimpl->BeginEyeRender(a_eye);
}

void DeviceNative::EndEyeRender()
{
	m_Pimpl->EndEyeRender();
}