#include "Core/camera.h"
#include <iostream>

namespace Visual
{
	void Camera::cameraUpdate()
	{
		/// Vector Forward so we can move freely in space
		LibMath::Vector3 forwrard = this->m_front + this->m_position;

		this->m_view = LibMath::Mat4::LookAt(this->m_position, forwrard, this->m_toTheSky);
	}

	void Camera::cameraInputHandler(double positionX, double positionY)
	{
		/// Calculate the offset in x and y
		float xoffset = positionX - this->m_lastx;
		float yoffset = positionY - this->m_lasty;

		/// Set the last x and y for the next mouse move
		this->m_lastx = positionX;
		this->m_lasty = positionY;

		/// Multiplicate our movement by our sensitivity
		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		/// our angle is equal to our offset on the m_screen
		this->m_yaw += LibMath::Degree(xoffset);
		this->m_pitch += LibMath::Degree(yoffset);

		/// Fix the rotation between 0 and 90
		if (this->m_pitch.raw() > 89.0f) m_pitch = LibMath::Degree(89.0f);
		if (this->m_pitch.raw() < -89.0f) m_pitch = LibMath::Degree(-89.0f);

		/// Move our Front vector based on our calculations before
		this->m_front.x = LibMath::cos(this->m_yaw) * LibMath::cos(m_pitch);
		this->m_front.y = LibMath::sin(m_pitch);
		this->m_front.z = LibMath::sin(this->m_yaw) * LibMath::cos(m_pitch);

		/// Keep the vector Front between 0 and 1
		this->m_front = LibMath::Vector3::normalize(this->m_front);

		/// Call the camera update once again
		this->cameraUpdate();
	}
}



