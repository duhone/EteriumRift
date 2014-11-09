#include "StdAfx.h"
#include "DeviceManagerNative.h"
#include "OVR.h"
#include "DeviceInfoConversions.h"

using namespace RE::OculusNative;
using namespace std;

namespace RE{
namespace OculusNative{
	class DeviceManagerNativeImpl
	{
	public:
		DeviceManagerNativeImpl();
		~DeviceManagerNativeImpl();

		void EnumerateDevices(function<void (const DeviceInfo&)> a_deviceInfo, float a_quality);
		DeviceNative* CreateDevice(DeviceConfig a_cfg);

	private:
	};
}}

DeviceManagerNativeImpl::DeviceManagerNativeImpl()
{	
	ovr_Initialize();
}

DeviceManagerNativeImpl::~DeviceManagerNativeImpl()
{
	ovr_Shutdown();
}

void DeviceManagerNativeImpl::EnumerateDevices(function<void (const DeviceInfo&)> a_deviceInfo, float a_quality)
{
	DeviceInfo device;
	int hmdCount = ovrHmd_Detect();
	for(int i = 0;i < hmdCount; ++i)
	{
		auto hmd = ovrHmd_Create(i);
		if(hmd)
		{
			device = Convert(hmd, a_quality);

			a_deviceInfo(device);
			ovrHmd_Destroy(hmd);
		}
	}
}

DeviceNative* DeviceManagerNativeImpl::CreateDevice(DeviceConfig a_cfg)
{
	int hmdCount = ovrHmd_Detect();
	for(int i = 0;i < hmdCount; ++i)
	{
		auto hmd = ovrHmd_Create(i);
		if(hmd)
		{
			if(hmd->DisplayId == a_cfg.DisplayID)
				return new DeviceNative((void*)hmd, a_cfg);
			else
				ovrHmd_Destroy(hmd);
		}
	}
	return nullptr;
}

DeviceManagerNative::DeviceManagerNative() : m_Pimpl(new DeviceManagerNativeImpl)
{
}


DeviceManagerNative::~DeviceManagerNative(void)
{
}

void DeviceManagerNative::EnumerateDevices(function<void (const DeviceInfo&)> a_deviceInfo, float a_quality)
{
	m_Pimpl->EnumerateDevices(a_deviceInfo, a_quality);
}

void DeviceManagerNative::EnumerateDevices(std::vector<DeviceInfo>& a_deviceInfos, float a_quality)
{
	m_Pimpl->EnumerateDevices([&](const DeviceInfo& a_deviceInfo){
		a_deviceInfos.push_back(a_deviceInfo);
	}, a_quality);
}

DeviceNative* DeviceManagerNative::CreateDevice(DeviceConfig a_cfg)
{
	return m_Pimpl->CreateDevice(a_cfg);
}