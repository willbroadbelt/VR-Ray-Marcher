
/* 
 * File:   VRHandler.h
 * Author: willbroadbelt
 *
 * Created on 30 November 2017, 12:17
 */

#ifndef VRHANDLER_H
#define VRHANDLER_H

#include <openvr.h>
#include "SDL2\SDL.h"
#include "GL\glew.h"
#include "glm\glm.hpp"
#include <string>

class VRHandler {
public:
    VRHandler();
	~VRHandler();

	bool submitEyes(GLuint leftTexture, GLuint rightTexture);

	void vrEvent();

	vr::HmdQuaternion_t GetRotationQuart(vr::HmdMatrix34_t matrix);
	vr::HmdVector3_t GetPosition(vr::HmdMatrix34_t matrix);
	glm::vec3 RotateVectorByQuaternion(vr::HmdQuaternion_t quart, glm::vec3 vec);

	glm::vec4 getProjectionRaw(bool leftEye);

	inline glm::vec3 GetPosition() { return m_position;  }
	inline glm::vec3 GetForward() { return m_forward; }
	inline glm::vec3 GetUp() { return m_up; }

	void Shutdown();


private:
	void SetupMatrices();
    
    vr::IVRSystem *m_pHMD;
    vr::IVRRenderModels *m_pRenderModels;
    std::string m_strDriver;
    std::string m_strDisplay;
    vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
    
	glm::mat4 m_mat4HMDPose;
	glm::mat4 m_mat4eyePosLeft;
	glm::mat4 m_mat4eyePosRight;

	glm::mat4 m_mat4ProjectionCenter;
	glm::mat4 m_mat4ProjectionLeft;
	glm::mat4 m_mat4ProjectionRight;
  
	glm::mat4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
	glm::mat4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
	glm::mat4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
	glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);

	glm::vec3 m_position, m_forward, m_up;

	const glm::vec3 CONST_FORWARD = glm::vec3(0, 0, -1);
	const glm::vec3 CONST_UP = glm::vec3(0, 1, 0);
	const float CONST_SCALE = 3.0f;
};

#endif /* VRHANDLER_H */
