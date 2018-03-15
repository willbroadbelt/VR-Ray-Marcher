
/* 
 * File:   VRHandler.cpp
 * Author: willbroadbelt
 * 
 * Created on 30 November 2017, 12:17
 */

#include "vrhandler.h"

#include <stdio.h>

VRHandler::VRHandler() {

	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None) {
		m_pHMD = NULL;
		char buf[1024];
		snprintf(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL);
		exit(EXIT_FAILURE);
	}

	uint32_t rWidth, rHeight;
	m_pHMD->GetRecommendedRenderTargetSize(&rWidth, &rHeight);
	printf("Recomended render width, height: %d , %d .\n", rWidth, rHeight);
	

	if (vr::VR_IsHmdPresent()) {
		printf("HMD is connected.\n");
	}
	else {
		printf("HMD is not connected.\n");
	}

	vr::EVRInitError peError = vr::VRInitError_None;

	if (!vr::VRCompositor())
	{
		printf("Compositor initialization failed. See log file for details\n");
	}

	m_pHMD->GetRecommendedRenderTargetSize(&m_renderWidth, &m_renderHeight);
	//printf("Width: %i, Height: %i", m_renderWidth, m_renderHeight);

}


void VRHandler::SetupMatrices() {
	m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);;
	m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);;
	m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}

bool VRHandler::submitEyes(GLuint leftTexture, GLuint rightTexture) {

	if (!m_pHMD->IsInputAvailable()) {
		printf("Input not available.");
		return false;
	}

	if (m_pHMD) {
		vr::TrackedDevicePose_t pose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		
		vr::HmdMatrix34_t poseMatrix = pose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
		vr::HmdVector3_t position = GetPosition(poseMatrix);
		vr::HmdQuaternion_t quaternion = GetRotationQuart(poseMatrix);

		m_position = glm::vec3(position.v[0], position.v[1], position.v[2]);
		m_forward = RotateVectorByQuaternion(quaternion, CONST_FORWARD);
		m_up = RotateVectorByQuaternion(quaternion, CONST_UP);

		

		
		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		glFinish();
		return true;
	}
	printf("Hmd false.");
	return false;
}


void VRHandler::Shutdown()
{
	vr::VR_Shutdown();
	m_pHMD = NULL;
}

void VRHandler::vrEvent() {

	vr::VREvent_t event;

	while (m_pHMD->PollNextEvent(&event, sizeof(event))) {
		switch (event.eventType) {

		case vr::VREvent_TrackedDeviceActivated:
		{
			printf("Tracked device HMD activated.");
		}
		break;

		case vr::VREvent_TrackedDeviceDeactivated:
		{
			printf("Device %u deactiavted.\n", event.trackedDeviceIndex);
		}
		break;

		case vr::VREvent_TrackedDeviceUpdated:
		{
			printf("Device %u updated.\n", event.trackedDeviceIndex);
		}
		break;

		}
		
	}
	//ParseTrackingFrame();

}
    
glm::mat4 VRHandler::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	glm::mat4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}
  

glm::mat4 VRHandler::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
	glm::mat4 matMVP;
	if( nEye == vr::Eye_Left )
	{
		matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose;
	}
	else if( nEye == vr::Eye_Right )
	{
		matMVP = m_mat4ProjectionRight * m_mat4eyePosRight *  m_mat4HMDPose;
	}

	return matMVP;
}


  //Taken from sample
  glm::mat4 VRHandler::GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye )
{
	if ( !m_pHMD )
		return glm::mat4();
       float m_fNearClip = 0.1f;
 	float m_fFarClip = 30.0f;

	vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix( nEye, m_fNearClip, m_fFarClip );

	return glm::mat4(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1], 
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2], 
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
}

  glm::mat4 VRHandler::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
	if ( !m_pHMD )
		return glm::mat4();

	vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform( nEye );
	glm::mat4 matrixObj(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0, 
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
		);
	//TODO invert
	return matrixObj;
}


  VRHandler::~VRHandler() {
	  Shutdown();
  }

  // Get the quaternion representing the rotation
  vr::HmdQuaternion_t VRHandler::GetRotationQuart(vr::HmdMatrix34_t matrix) {
	  vr::HmdQuaternion_t q;

	  q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	  q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	  q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	  q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	  q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	  q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	  q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
	  return q;
  }

  // Get the vector representing the position
  vr::HmdVector3_t VRHandler::GetPosition(vr::HmdMatrix34_t matrix) {
	  vr::HmdVector3_t vector;

	  vector.v[0] = matrix.m[0][3];
	  vector.v[1] = matrix.m[1][3];
	  vector.v[2] = matrix.m[2][3];

	  return vector;
  }

  glm::vec4 VRHandler::getProjectionRaw(bool leftEye) {
	  float left, right, top, bottom;
	  vr::Hmd_Eye eye = leftEye ? vr::Eye_Left : vr::Eye_Right;
	  m_pHMD->GetProjectionRaw(eye, &left, &right, &top, &bottom);
	  return glm::vec4(left, right, top, bottom);
  }


  glm::vec3 VRHandler::RotateVectorByQuaternion(vr::HmdQuaternion_t quart, glm::vec3 vec) {
	  
	  glm::vec3 quartVec3 = glm::vec3(quart.x, quart.y, quart.z);
	  // Scalar part of the quaternion
	  float s = quart.w;

	  //Found at https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	  glm::vec3 direction = 2.0f * glm::dot(quartVec3, vec) * quartVec3
		  + (s*s - glm::dot(quartVec3, quartVec3)) * vec
		  + 2.0f * s * glm::cross(quartVec3, vec);

	  return glm::normalize(direction);
	  
  }