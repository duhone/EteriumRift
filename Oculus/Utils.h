#pragma once

#include "Device.h"
#include "..\OculusNative\DeviceNative.h"

namespace RE{
namespace Oculus{

inline RE::Oculus::DeviceInfo Convert(const RE::OculusNative::DeviceInfo& _devInfo)
{
	RE::Oculus::DeviceInfo result;

	result.DisplayID = _devInfo.DisplayID;
	result.Resolution = gcnew array<int>(2);
	result.Resolution[0] = _devInfo.Resolution[0];
	result.Resolution[1] = _devInfo.Resolution[1];
	std::wstring deviceName(begin(_devInfo.DeviceName), end(_devInfo.DeviceName));
	result.DeviceName = gcnew System::String(deviceName.data());

	return result;
}

}
}