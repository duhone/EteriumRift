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

		//a_quality only effects the returned recommended resolution, which you are free to ignore.
		//a value of 1 returns the highest resolution that will make a diffrence. around 1182x1461 on DK2. values below 
		//around 0.5(591x731) start to get impractical on DK2.
		void EnumerateDevices(float a_quality, System::Action<DeviceInfo>^ a_lambda);
		Device^ CreateDevice(DeviceConfig a_cfg);

	private:
		RE::OculusNative::DeviceManagerNative* m_Pimpl;
	};
}}
