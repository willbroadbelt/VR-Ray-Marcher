
#include "camera.h"
#include <glm/glm.hpp>

void Camera::UpdatePos(const glm::vec3& pos) {
    m_position = pos;
}

void Camera::UpdateDir(const glm::vec3& dir) {
    m_direction = dir;
}

void Camera::UpdateUp(const glm::vec3& up) {
	m_up = up;
}

glm::vec3 Camera::GetPos() {
    return m_position;
}

glm::vec3 Camera::GetDir() {
    return glm::normalize(m_direction);
}

glm::vec3 Camera::GetUp() {
	if (b_defaultUp) {
		glm::vec3 dir = this->GetDir();
		glm::vec3 Y = glm::vec3(0, 1, 0);
		glm::vec3 X = glm::cross(Y, dir);
		return glm::normalize(glm::cross(dir, X));
	}
	return m_up;
    
}
