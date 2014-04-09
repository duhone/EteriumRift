#include "StdAfx.h"
#include "DeviceNative.h"
#include "OVR.h"
#include "DeviceInfoConversions.h"

using namespace RE::OculusNative;
using namespace std;

namespace RE{
namespace OculusNative{
	class DeviceNativeImpl
	{
	public:
		DeviceNativeImpl(OVR::HMDDevice* _device, OVR::SensorDevice* _sensor);
		~DeviceNativeImpl();
		
		const DeviceInfo& GetDeviceInfo() const { return m_DeviceInfo; }
		const StereoSettings& GetStereoSettings() const { return m_StereoSettings; }
		void SelectEye(DeviceNative::EYE _eye);
		float Calc2DOffset(float _depth);
		std::array<float, 4> GetOrientation();		
		void ResetOrientation();

	private:
		DeviceInfo m_DeviceInfo;
		OVR::Ptr<OVR::HMDDevice> m_OvrDevice;
		OVR::Ptr<OVR::SensorDevice>	m_OvrSensor;
		OVR::SensorFusion m_OvrFusion;
		OVR::Util::Render::StereoConfig m_StereoConfig;
		StereoSettings m_StereoSettings;
		OVR::Util::Render::StereoEyeParams m_EyeParams;
		OVR::HMDInfo m_NativeInfo;
		DeviceNative::EYE m_Eye;
	};
}}

DeviceNativeImpl::DeviceNativeImpl(OVR::HMDDevice* _device, OVR::SensorDevice* _sensor) :
	m_OvrDevice(_device), m_OvrSensor(_sensor), m_Eye(DeviceNative::LEFT_EYE)
{	
	assert(_device && _sensor);

	m_OvrFusion.AttachToSensor(_sensor);
	m_OvrFusion.SetPrediction(1.0f/60.0f, true);

	m_OvrDevice->GetDeviceInfo(&m_NativeInfo);
	m_DeviceInfo = Convert(m_NativeInfo);
	
	m_StereoConfig.SetFullViewport(OVR::Util::Render::Viewport(0, 0, m_DeviceInfo.Resolution[0], m_DeviceInfo.Resolution[1]));
	m_StereoConfig.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
	m_StereoConfig.SetHMDInfo(m_NativeInfo);
	m_StereoConfig.SetDistortionFitPointVP(-1, 0);
		
	m_StereoSettings.FovY = m_StereoConfig.GetYFOVRadians();
	m_StereoSettings.AspectRatio = m_StereoConfig.GetAspect();
}

DeviceNativeImpl::~DeviceNativeImpl()
{	
}

void DeviceNativeImpl::SelectEye(DeviceNative::EYE _eye)
{
	m_Eye = _eye;
	if(_eye == DeviceNative::LEFT_EYE)
	{
		m_EyeParams = m_StereoConfig.GetEyeRenderParams(OVR::Util::Render::StereoEye_Left);
		m_StereoSettings.ProjectionCenterOffset = m_StereoConfig.GetProjectionCenterOffset();
		m_StereoSettings.ViewTranslation = m_StereoConfig.GetHMDInfo().InterpupillaryDistance *
			m_StereoConfig.GetHMDInfo().HScreenSize * 0.25f * 0.5f;
		m_StereoSettings.DistortionCenterOffset[0] = m_EyeParams.pDistortion->XCenterOffset;
		//memcpy(m_StereoSettings.Ortho, m_EyeParams.OrthoProjection.M, sizeof(m_EyeParams.OrthoProjection));
	}
	else
	{
		m_EyeParams = m_StereoConfig.GetEyeRenderParams(OVR::Util::Render::StereoEye_Right);
		m_StereoSettings.ProjectionCenterOffset = -m_StereoConfig.GetProjectionCenterOffset();
		m_StereoSettings.ViewTranslation = -m_StereoConfig.GetHMDInfo().InterpupillaryDistance *
			m_StereoConfig.GetHMDInfo().HScreenSize * 0.25f * 0.5f;
		m_StereoSettings.DistortionCenterOffset[0] = -m_EyeParams.pDistortion->XCenterOffset;
		//memcpy(m_StereoSettings.Ortho, m_EyeParams.OrthoProjection.M, sizeof(m_EyeParams.OrthoProjection));
	}
	m_StereoSettings.DistortionK[0] = m_EyeParams.pDistortion->K[0];
	m_StereoSettings.DistortionK[1] = m_EyeParams.pDistortion->K[1];
	m_StereoSettings.DistortionK[2] = m_EyeParams.pDistortion->K[2];
	m_StereoSettings.DistortionK[3] = m_EyeParams.pDistortion->K[3];
	m_StereoSettings.DistortionCenterOffset[1] = m_EyeParams.pDistortion->YCenterOffset;
	m_StereoSettings.DistortionScale = m_EyeParams.pDistortion->Scale;
	m_StereoSettings.ChromAberration[0] = m_NativeInfo.ChromaAbCorrection[0];
	m_StereoSettings.ChromAberration[1] = m_NativeInfo.ChromaAbCorrection[1];
	m_StereoSettings.ChromAberration[2] = m_NativeInfo.ChromaAbCorrection[2];
	m_StereoSettings.ChromAberration[3] = m_NativeInfo.ChromaAbCorrection[3];
}

float DeviceNativeImpl::Calc2DOffset(float _depth)
{
    float metersToPixels          = (m_NativeInfo.HResolution / m_NativeInfo.HScreenSize);
    float lensDistanceScreenPixels= metersToPixels * m_NativeInfo.LensSeparationDistance;
	float eyeDistanceScreenPixels = metersToPixels * m_NativeInfo.InterpupillaryDistance;
    float offCenterShiftPixels    = (m_NativeInfo.EyeToScreenDistance / _depth) * eyeDistanceScreenPixels;
    float leftPixelCenter         = (m_NativeInfo.HResolution / 2) - lensDistanceScreenPixels * 0.5f;
    float rightPixelCenter        = lensDistanceScreenPixels * 0.5f;
    float pixelDifference         = leftPixelCenter - rightPixelCenter;
    
    // This computes the number of pixels that fit within specified 2D FOV (assuming
    // distortion scaling will be done).
    float percievedHalfScreenDistance = tan(1.48352986f * 0.5f) * m_NativeInfo.EyeToScreenDistance;
    float vfovSize = 2.0f * percievedHalfScreenDistance / m_EyeParams.pDistortion->Scale;
    float FovPixels = m_NativeInfo.VResolution * vfovSize / m_NativeInfo.VScreenSize;
    
    float orthoPixelOffset = (pixelDifference + offCenterShiftPixels/m_EyeParams.pDistortion->Scale) * 0.5f;
	orthoPixelOffset = orthoPixelOffset * 2.0f / FovPixels;
	if(m_Eye == DeviceNative::RIGHT_EYE)
		orthoPixelOffset *= -1;
		
    // Create orthographic matrix.   
    OVR::Matrix4f m;//      = OrthoCenter;
    m.SetIdentity();
    m.M[0][0] = FovPixels / (m_NativeInfo.HResolution * 0.5f);
    m.M[1][1] = -FovPixels / m_NativeInfo.VResolution;
    m.M[0][3] = 0;
    m.M[1][3] = 0;
    m.M[2][2] = 0;
	
	m = m * OVR::Matrix4f::Translation(orthoPixelOffset, 0, 0);

	return m.M[0][3] * (m_NativeInfo.HResolution/2);
}

std::array<float, 4> DeviceNativeImpl::GetOrientation()
{
	array<float, 4> result;
	auto rotation = m_OvrFusion.GetPredictedOrientation();
	result[0] = rotation.x;
	result[1] = rotation.y;
	result[2] = rotation.z;
	result[3] = rotation.w;
	/*
	char temp[256];
	sprintf_s(temp, "Orientation %0.2f %0.2f %0.2f %0.2f\n", result[0], result[1], result[2], result[3]);
	OutputDebugStringA(temp);
	*/
	return result;
}

void DeviceNativeImpl::ResetOrientation()
{
	m_OvrFusion.Reset();
}

DeviceNative::DeviceNative(OVR::HMDDevice* _device, OVR::SensorDevice* _sensor) : 
	m_Pimpl(new DeviceNativeImpl(_device, _sensor))
{
}


DeviceNative::~DeviceNative(void)
{
}

const DeviceInfo& DeviceNative::GetDeviceInfo() const
{
	return m_Pimpl->GetDeviceInfo();
}

const StereoSettings& DeviceNative::GetStereoSettings() const
{
	return m_Pimpl->GetStereoSettings();
}

void DeviceNative::SelectEye(EYE _eye)
{
	m_Pimpl->SelectEye(_eye);
}

float DeviceNative::Calc2DOffset(float _depth)
{
	return m_Pimpl->Calc2DOffset(_depth);
}

std::array<float, 4> DeviceNative::GetOrientation()
{
	return m_Pimpl->GetOrientation();
}

void DeviceNative::ResetOrientation()
{
	m_Pimpl->ResetOrientation();
}