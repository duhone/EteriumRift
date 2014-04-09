#pragma once
#include "DeviceNative.h"
#include "OVR.h"

namespace RE{
namespace OculusNative{

inline RE::OculusNative::DeviceInfo Convert(const OVR::HMDInfo& _hmdInfo)
{
	RE::OculusNative::DeviceInfo result;

	result.DisplayID = _hmdInfo.DisplayId;
	result.Resolution[0] = _hmdInfo.HResolution;
	result.Resolution[1] = _hmdInfo.VResolution;
	result.DeviceName = _hmdInfo.DisplayDeviceName;

	return result;
}

}
}