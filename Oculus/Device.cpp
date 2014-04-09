#include "stdafx.h"
#include "Device.h"
#include "..\OculusNative\DeviceNative.h"
#include "Utils.h"

using namespace System;
using namespace RE;
using namespace RE::Oculus;
using namespace RE::OculusNative;

Device::Device(class RE::OculusNative::DeviceNative* _impl) : m_Pimpl(_impl)
{
	m_DeviceInfo = Convert(m_Pimpl->GetDeviceInfo()); //shouldnt change, marshal once and cache
	UpdateStereoSettings();
}

Device::~Device()
{
	this->!Device();
}

Device::!Device()
{
	delete m_Pimpl;
	m_Pimpl = nullptr;
}

void Device::UpdateStereoSettings()
{
	auto ssettings = m_Pimpl->GetStereoSettings();
	m_StereoSettings.ProjectionCenterOffset = ssettings.ProjectionCenterOffset;
	m_StereoSettings.FovY = ssettings.FovY;
	m_StereoSettings.AspectRatio = ssettings.AspectRatio;
	m_StereoSettings.ViewTranslation = ssettings.ViewTranslation;
	
	m_StereoSettings.DistortionK = gcnew array<float>(4);
	m_StereoSettings.DistortionK[0] = ssettings.DistortionK[0];
	m_StereoSettings.DistortionK[1] = ssettings.DistortionK[1];
	m_StereoSettings.DistortionK[2] = ssettings.DistortionK[2];
	m_StereoSettings.DistortionK[3] = ssettings.DistortionK[3];

	m_StereoSettings.DistortionCenterOffset = gcnew array<float>(2);
	m_StereoSettings.DistortionCenterOffset[0] = ssettings.DistortionCenterOffset[0];
	m_StereoSettings.DistortionCenterOffset[1] = ssettings.DistortionCenterOffset[1];
	
	m_StereoSettings.DistortionScale = ssettings.DistortionScale;

	
	m_StereoSettings.ChromAberration = gcnew array<float>(4);
	m_StereoSettings.ChromAberration[0] = ssettings.ChromAberration[0];
	m_StereoSettings.ChromAberration[1] = ssettings.ChromAberration[1];
	m_StereoSettings.ChromAberration[2] = ssettings.ChromAberration[2];
	m_StereoSettings.ChromAberration[3] = ssettings.ChromAberration[3];

	/*m_StereoSettings.Ortho = gcnew array<float>(16);
	for(int y = 0;y < 4; ++y)
	{
		for(int x = 0;x < 4; ++x)
		{
			m_StereoSettings.Ortho[y*4+x] = ssettings.Ortho[x][y];
		}
	}*/

}

void Device::SelectEye(EYE _eye)
{
	if(_eye == EYE::LEFT_EYE)
		m_Pimpl->SelectEye(DeviceNative::LEFT_EYE);
	else
		m_Pimpl->SelectEye(DeviceNative::RIGHT_EYE);

	UpdateStereoSettings();
}

float Device::Calc2DOffset(float _depth)
{
	return m_Pimpl->Calc2DOffset(_depth);
}	

void Device::Update()
{
	auto rotation = m_Pimpl->GetOrientation();

	m_Orientation = gcnew array<float>(4);
	m_Orientation[0] = rotation[0];
	m_Orientation[1] = rotation[1];
	m_Orientation[2] = rotation[2];
	m_Orientation[3] = rotation[3];
}

void Device::ResetOrientation()
{
	m_Pimpl->ResetOrientation();
}