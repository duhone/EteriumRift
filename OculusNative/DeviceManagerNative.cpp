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

		void EnumerateDevices(function<void (const DeviceInfo&)> _deviceInfo);
		DeviceNative* CreateDevice(int _DisplayID);

	private:
		OVR::System m_OVRSystem;
		OVR::Ptr<OVR::DeviceManager> m_OVRDeviceManager;
	};
}}

DeviceManagerNativeImpl::DeviceManagerNativeImpl()
{	
	m_OVRDeviceManager = *OVR::DeviceManager::Create();
}

DeviceManagerNativeImpl::~DeviceManagerNativeImpl()
{
}

void DeviceManagerNativeImpl::EnumerateDevices(function<void (const DeviceInfo&)> _deviceInfo)
{
	DeviceInfo device;
	auto deviceIter = m_OVRDeviceManager->EnumerateDevices<OVR::HMDDevice>();
	while(deviceIter)
	{
		OVR::HMDInfo hmdInfo;
		if(deviceIter.GetDeviceInfo(&hmdInfo))
		{
			device = Convert(hmdInfo);
			_deviceInfo(device);
		}
		deviceIter.Next();
	}
}
DeviceNative* DeviceManagerNativeImpl::CreateDevice(int _DisplayID)
{
	auto deviceIter = m_OVRDeviceManager->EnumerateDevices<OVR::HMDDevice>();
	while(deviceIter)
	{
		OVR::HMDInfo hmdInfo;
		if(deviceIter.GetDeviceInfo(&hmdInfo))
		{
			if(hmdInfo.DisplayId == _DisplayID)
			{
				auto hmdDevice = deviceIter.CreateDevice();
				if(hmdDevice)
				{
					auto sensor = hmdDevice->GetSensor();
					if(sensor)
					{	
						return new DeviceNative(hmdDevice, sensor);
					}
					hmdDevice->Release();
				}
			}
		}
		deviceIter.Next();
	}
	return nullptr;
}

DeviceManagerNative::DeviceManagerNative() : m_Pimpl(new DeviceManagerNativeImpl)
{
}


DeviceManagerNative::~DeviceManagerNative(void)
{
}

void DeviceManagerNative::EnumerateDevices(function<void (const DeviceInfo&)> _deviceInfo)
{
	m_Pimpl->EnumerateDevices(_deviceInfo);
}

void DeviceManagerNative::EnumerateDevices(std::vector<DeviceInfo>& _deviceInfos)
{
	m_Pimpl->EnumerateDevices([&](const DeviceInfo& _deviceInfo){
		_deviceInfos.push_back(_deviceInfo);
	});
}

DeviceNative* DeviceManagerNative::CreateDevice(int _DisplayID)
{
	return m_Pimpl->CreateDevice(_DisplayID);
}