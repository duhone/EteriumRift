// This is the main DLL file.

#include "stdafx.h"
#include "DeviceManager.h"
#include <vector>
#include "..\OculusNative\DeviceManagerNative.h"
#include "Utils.h"

using namespace System;
using namespace System::Reflection;
using namespace RE;
using namespace RE::Oculus;
using namespace RE::OculusNative;

DeviceManager::DeviceManager() : m_Pimpl(new DeviceManagerNative)
{
}

DeviceManager::~DeviceManager()
{
	this->!DeviceManager();
}

DeviceManager::!DeviceManager()
{
	delete m_Pimpl;
	m_Pimpl = nullptr;
}

void DeviceManager::EnumerateDevices(float a_quality, System::Action<RE::Oculus::DeviceInfo>^ a_lambda)
{
	std::vector<OculusNative::DeviceInfo> deviceInfos; //too many restrictions mixing native and managed :(
	m_Pimpl->EnumerateDevices(deviceInfos, a_quality);
	for(size_t i = 0;i < deviceInfos.size(); ++i)
	{
		Oculus::DeviceInfo deviceInfo = Convert(deviceInfos[i]);
		deviceInfo.DisplayID = deviceInfos[i].DisplayID;
		deviceInfo.Resolution.X = deviceInfos[i].Resolution[0];
		deviceInfo.Resolution.Y = deviceInfos[i].Resolution[1];
		deviceInfo.RecommendedTextureResolutionLeft.X = deviceInfos[i].RecommendedTextureResolutionLeft[0];
		deviceInfo.RecommendedTextureResolutionLeft.Y = deviceInfos[i].RecommendedTextureResolutionLeft[1];
		deviceInfo.RecommendedTextureResolutionRight.X = deviceInfos[i].RecommendedTextureResolutionRight[0];
		deviceInfo.RecommendedTextureResolutionRight.Y = deviceInfos[i].RecommendedTextureResolutionRight[1];
		deviceInfo.PreferredFirstEye = deviceInfos[i].PreferredFirstEye == OculusNative::EYE::LEFT_EYE ? 
			Oculus::EYE::LEFT_EYE : Oculus::EYE::RIGHT_EYE;

		a_lambda(deviceInfo);
	}
}

Device^ DeviceManager::CreateDevice(Oculus::DeviceConfig a_cfg)
{
	OculusNative::DeviceConfig cfg;
	cfg.DisplayID = a_cfg.DisplayID;
	cfg.RenderTargetResolution[0] = a_cfg.RenderTargetResolution.X;
	cfg.RenderTargetResolution[1] = a_cfg.RenderTargetResolution.Y;
	cfg.RenderTextureResolutionLeft[0] = a_cfg.RenderTextureResolutionLeft.X;
	cfg.RenderTextureResolutionLeft[1] = a_cfg.RenderTextureResolutionLeft.Y;
	cfg.RenderTextureResolutionRight[0] = a_cfg.RenderTextureResolutionRight.X;
	cfg.RenderTextureResolutionRight[1] = a_cfg.RenderTextureResolutionRight.Y;
	cfg.WindowHandle = a_cfg.GameInstance->Window->Handle.ToPointer();
	FieldInfo^ fi = a_cfg.Device->GetType()->GetField("pComPtr", BindingFlags::NonPublic | BindingFlags::Instance);
    cfg.Device = static_cast<IDirect3DDevice9*>(Pointer::Unbox(fi->GetValue(a_cfg.Device)));
	fi = a_cfg.LeftRenderTexture->GetType()->GetField("pComPtr", BindingFlags::NonPublic | BindingFlags::Instance);
	cfg.LeftRenderTexture = static_cast<IDirect3DTexture9*>(Pointer::Unbox(fi->GetValue(a_cfg.LeftRenderTexture)));
	fi = a_cfg.RightRenderTexture->GetType()->GetField("pComPtr", BindingFlags::NonPublic | BindingFlags::Instance);
	cfg.RightRenderTexture = static_cast<IDirect3DTexture9*>(Pointer::Unbox(fi->GetValue(a_cfg.RightRenderTexture)));
	cfg.zNear = a_cfg.zNear;
	cfg.zFar = a_cfg.zFar;

	auto device = m_Pimpl->CreateDevice(cfg);
	if(!device)
		return nullptr;
	return gcnew Device(device);
}