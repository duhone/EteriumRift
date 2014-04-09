// Oculus.h

#pragma once
#include "Device.h"

namespace RE{
namespace OculusNative
{
	class DeviceManagerNative;
}
}

namespace RE{
namespace Oculus
{
	public ref class DeviceManager
	{
	public:
		DeviceManager();
		~DeviceManager();
		!DeviceManager();

		void EnumerateDevices(System::Action<DeviceInfo>^ _lambda);
		Device^ CreateDevice(int _displayID);

	private:
		RE::OculusNative::DeviceManagerNative* m_Pimpl;
	};
}}
