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

		void EnumerateDevices(std::function<void (const DeviceInfo&)> _deviceInfo);
		void EnumerateDevices(std::vector<DeviceInfo>& _deviceInfos); //needed if talking to sub standard languages like .net
		DeviceNative* CreateDevice(int _DisplayID);

	private:
		std::unique_ptr<class DeviceManagerNativeImpl> m_Pimpl;
	};
}}

