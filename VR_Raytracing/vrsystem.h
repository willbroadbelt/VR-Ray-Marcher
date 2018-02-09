
/* 
 * File:   vrsystem.h
 * Author: willbroadbelt
 *
 * Created on 30 November 2017, 12:17
 */

#ifndef VRSYSTEM_H
#define VRSYSTEM_H

#include <openvr.h>
#include "SDL2\SDL.h"
#include "GL\glew.h"
#include "glm\glm.hpp"
#include <string>

class VRsystem {
public:
    VRsystem();
	~VRsystem();

	bool submitEyes(GLuint leftTexture, GLuint rightTexture);

	void vrEvent();
	glm::vec3 ParseTrackingFrame();

	vr::HmdQuaternion_t GetRotationQuart(vr::HmdMatrix34_t matrix);
	vr::HmdVector3_t GetPosition(vr::HmdMatrix34_t matrix);
	glm::vec3 RotateVectorByQuaternion(vr::HmdQuaternion_t quart, glm::vec3 vec);

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

	const glm::vec3 CONST_FORWARD = glm::vec3(0, 0, -1);
	const glm::vec3 CONST_UP = glm::vec3(0, 1, 0);
};

#endif /* VRSYSTEM_H */
