/* 
 * File:   camera.h
 * Author: willbroadbelt
 *
 * Created on 02 November 2017, 10:40
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "glm\glm.hpp"

class Camera
{
public:
    Camera(const glm::vec3& pos, const glm::vec3& dir)
    {
        m_position = pos;
        m_direction = dir;
		b_defaultUp = false;
    }

    glm::vec3 GetPos();
    glm::vec3 GetUp();
    glm::vec3 GetDir();
    
    void UpdatePos(const glm::vec3& pos);
    void UpdateDir(const glm::vec3& dir);
	void UpdateUp(const glm::vec3& up);
    
    
private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
	glm::vec3 m_up;

	bool b_defaultUp;
    
};

    /*
     * //Example of lookAt to get a ViewProjection.
    inline glm::mat4 getViewProjection() const
    {
        return m_perspective * glm::lookAt(m_position, m_position + m_forward, m_up);
        
    }
     */

#endif /* CAMERA_H */

