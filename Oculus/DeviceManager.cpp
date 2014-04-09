// This is the main DLL file.

#include "stdafx.h"
#include "DeviceManager.h"
#include <vector>
#include "..\OculusNative\DeviceManagerNative.h"
#include "Utils.h"

using namespace System;
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

void DeviceManager::EnumerateDevices(System::Action<RE::Oculus::DeviceInfo>^ _Lambda)
{
	std::vector<OculusNative::DeviceInfo> deviceInfos; //too many restrictions mixing native and managed :(
	m_Pimpl->EnumerateDevices(deviceInfos);
	for(size_t i = 0;i < deviceInfos.size(); ++i)
	{
		Oculus::DeviceInfo deviceInfo = Convert(deviceInfos[i]);
		deviceInfo.DisplayID = deviceInfos[i].DisplayID;
		deviceInfo.Resolution = gcnew array<int>(2);
		deviceInfo.Resolution[0] = deviceInfos[i].Resolution[0];
		deviceInfo.Resolution[1] = deviceInfos[i].Resolution[1];
		_Lambda(deviceInfo);
	}
}

Device^ DeviceManager::CreateDevice(int _displayID)
{
	auto device = m_Pimpl->CreateDevice(_displayID);
	if(!device)
		return nullptr;
	return gcnew Device(device);
}