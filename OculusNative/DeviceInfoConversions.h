#pragma once
#include "DeviceNative.h"
#include "OVR.h"

namespace RE{
namespace OculusNative{
	inline RE::OculusNative::DeviceInfo Convert(ovrHmd a_hmd, float a_quality)
	{
		RE::OculusNative::DeviceInfo result;

		result.DisplayID = a_hmd->DisplayId;
		result.Resolution[0] = a_hmd->Resolution.w;
		result.Resolution[1] = a_hmd->Resolution.h;
		result.DeviceName = a_hmd->DisplayDeviceName;
		
		
		auto recommenedTexSizeLeft = ovrHmd_GetFovTextureSize(a_hmd, ovrEye_Left, a_hmd->DefaultEyeFov[0], a_quality);
		result.RecommendedTextureResolutionLeft[0] = recommenedTexSizeLeft.w;
		result.RecommendedTextureResolutionLeft[1] = recommenedTexSizeLeft.h;
		
		auto recommenedTexSizeRight = ovrHmd_GetFovTextureSize(a_hmd, ovrEye_Right, a_hmd->DefaultEyeFov[1], a_quality);
		result.RecommendedTextureResolutionRight[0] = recommenedTexSizeRight.w;
		result.RecommendedTextureResolutionRight[1] = recommenedTexSizeRight.h;
		/*
		result.RecommendedTextureResolutionLeft[0] = a_hmd->Resolution.w/2;
		result.RecommendedTextureResolutionLeft[1] = a_hmd->Resolution.h;
		result.RecommendedTextureResolutionRight[0] = a_hmd->Resolution.w/2;
		result.RecommendedTextureResolutionRight[1] = a_hmd->Resolution.h;
		*/
		result.PreferredFirstEye = a_hmd->EyeRenderOrder[0] == ovrEyeType::ovrEye_Left ? EYE::LEFT_EYE : EYE::RIGHT_EYE;

		return result;
	}
}
}