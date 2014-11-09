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
	m_StereoSettings.FovY = ssettings.Fov;
	m_StereoSettings.LeftProjection = ConvertMatrix(ssettings.LeftProjection);
	m_StereoSettings.RightProjection = ConvertMatrix(ssettings.RightProjection);
	m_StereoSettings.LeftViewAdjust = ConvertVector(ssettings.LeftViewAdjust);
	m_StereoSettings.RightViewAdjust = ConvertVector(ssettings.RightViewAdjust);
}
		
float Device::Calc2DOffset(float _depth)
{
	return m_Pimpl->Calc2DOffset(_depth);
}	

void Device::Update()
{
	auto pose = m_Pimpl->GetPose();
	m_Pose.Orientation.X = pose.Orientation[0];
	m_Pose.Orientation.Y = pose.Orientation[1];
	m_Pose.Orientation.Z = pose.Orientation[2];
	m_Pose.Orientation.W = pose.Orientation[3];
	m_Pose.Orientation.Conjugate();

	m_Pose.Position.X = -pose.Position[0];
	m_Pose.Position.Y = -pose.Position[1];
	m_Pose.Position.Z = -pose.Position[2];
}

RE::Oculus::Pose Device::GetPose()
{
	return m_Pose;
}

void Device::ResetPose()
{
	m_Pimpl->ResetPose();
}

void Device::BeginFrame()
{
	m_Pimpl->BeginFrame();
}

void Device::EndFrame()
{
	m_Pimpl->EndFrame();
}

void Device::BeginEyeRender(RE::Oculus::EYE a_eye)
{
	m_Pimpl->BeginEyeRender(a_eye == RE::Oculus::EYE::LEFT_EYE ? RE::OculusNative::EYE::LEFT_EYE : RE::OculusNative::EYE::RIGHT_EYE);
}

void Device::EndEyeRender()
{
	m_Pimpl->EndEyeRender();
}

