#pragma once

///LibMath include
#include "LibMath/Matrix/Mat4x4.h"
#include "LibMath/Angle/Degree.h"
using namespace LibMath::Literal;
#include "pch.h"


namespace Visual
{
	///Class for the camera 
	class Camera
	{
	public:
		LibMath::Mat4 m_view			= LibMath::Mat4::Identity();
		LibMath::Mat4 m_transformation	= LibMath::Mat4::Identity();
		LibMath::Mat4 m_projection		= LibMath::Mat4::Perspective(LibMath::Degree(90).raw(), 1920.f / 1080.f, 0.1f, 60.f);

		LibMath::Vector3 m_position = { 0.0f, 0.0f, 0.5f };
		LibMath::Vector3 m_toTheSky = { 0.0f, 1.0f, 0.0f };
		LibMath::Vector3 m_front = {0.0f, 0.0f, 1.0f};

		LibMath::Degree m_yaw;
		LibMath::Degree m_pitch;

		float m_movementSpeed = 1.f;
		float m_lastx = 400;
		float m_lasty = 300;

		void cameraUpdate();

		void cameraInputHandler(double pox, double posy);

		Camera() = default;

		~Camera() = default;
	};
}
