#pragma once

#include "Device.h"
#include "..\OculusNative\DeviceNative.h"

namespace RE{
namespace Oculus{

inline RE::Oculus::DeviceInfo Convert(const RE::OculusNative::DeviceInfo& _devInfo)
{
	RE::Oculus::DeviceInfo result;

	result.DisplayID = _devInfo.DisplayID;
	result.Resolution.X = _devInfo.Resolution[0];
	result.Resolution.Y = _devInfo.Resolution[1];
	result.RecommendedTextureResolutionLeft.X = _devInfo.RecommendedTextureResolutionLeft[0];
	result.RecommendedTextureResolutionLeft.Y = _devInfo.RecommendedTextureResolutionLeft[1];
	result.RecommendedTextureResolutionRight.X = _devInfo.RecommendedTextureResolutionRight[0];
	result.RecommendedTextureResolutionRight.Y = _devInfo.RecommendedTextureResolutionRight[1];
	result.PreferredFirstEye = _devInfo.PreferredFirstEye == RE::OculusNative::EYE::LEFT_EYE ? RE::Oculus::EYE::LEFT_EYE : 
		RE::Oculus::EYE::RIGHT_EYE;

	std::wstring deviceName(begin(_devInfo.DeviceName), end(_devInfo.DeviceName));
	result.DeviceName = gcnew System::String(deviceName.data());

	return result;
}

inline Microsoft::Xna::Framework::Matrix ConvertMatrix(float* arg)
{
	Microsoft::Xna::Framework::Matrix result;
	result.M11 = arg[0];
	result.M12 = arg[1];
	result.M13 = arg[2]/2;
	result.M14 = arg[3];
	result.M21 = arg[4];
	result.M22 = arg[5];
	result.M23 = arg[6];
	result.M24 = arg[7];
	result.M31 = arg[8];
	result.M32 = arg[9];
	result.M33 = arg[10];
	result.M34 = arg[11];
	result.M41 = arg[12];
	result.M42 = arg[13];
	result.M43 = arg[14];
	result.M44 = arg[15];
	return Microsoft::Xna::Framework::Matrix::Transpose(result);
}

inline Microsoft::Xna::Framework::Vector3 ConvertVector(float* arg)
{
	return Microsoft::Xna::Framework::Vector3(arg[0], arg[1], arg[2]);
}

}
}