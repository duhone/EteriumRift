#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "DeviceNative.h"

namespace RE{
namespace OculusNative
{
	class DeviceManagerNative
	{
	public:
		DeviceManagerNative(void);
		~DeviceManagerNative(void);

		void EnumerateDevices(std::function<void (const DeviceInfo&)> a_deviceInfo, float a_quality);
		void EnumerateDevices(std::vector<DeviceInfo>& a_deviceInfos, float a_quality);
		DeviceNative* CreateDevice(DeviceConfig a_cfg);

	private:
		std::unique_ptr<class DeviceManagerNativeImpl> m_Pimpl;
	};
}}

