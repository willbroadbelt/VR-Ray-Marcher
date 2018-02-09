
/* 
 * File:   vrsystem.cpp
 * Author: willbroadbelt
 * 
 * Created on 30 November 2017, 12:17
 */

#include "vrsystem.h"

#include <stdio.h>

VRsystem::VRsystem() {

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

}

void VRsystem::SetupMatrices() {
	m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);;
	m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);;
	m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}

bool VRsystem::submitEyes(GLuint leftTexture, GLuint rightTexture) {

	if (!m_pHMD->IsInputAvailable()) {
		printf("Input not available.");
		return false;
	}

	if (m_pHMD) {
		vr::TrackedDevicePose_t pose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		m_mat4HMDPose = ConvertSteamVRMatrixToMatrix4(pose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);


		//Prints 1, 2
		//printf("hellovr: submit GLuints %d, %d\n", leftTexture, rightTexture);
		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		glFinish();
		return true;
	}
	printf("HMd false.");
	return false;
}


void VRsystem::Shutdown()
{
	vr::VR_Shutdown();
	m_pHMD = NULL;
}

void VRsystem::vrEvent() {

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
    
glm::mat4 VRsystem::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	glm::mat4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}
  

glm::mat4 VRsystem::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
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
  glm::mat4 VRsystem::GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye )
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

  glm::mat4 VRsystem::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
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


  VRsystem::~VRsystem() {
	  Shutdown();
  }

  // Get the quaternion representing the rotation
  vr::HmdQuaternion_t VRsystem::GetRotationQuart(vr::HmdMatrix34_t matrix) {
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

  //TDOD: Finish this using http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
  glm::mat3 QuaternionToRotationMat(vr::HmdQuaternion_t quart) {
	  double ws = pow(quart.w, 2);
	  double xs = pow(quart.x, 2);
	  double ys = pow(quart.y, 2);
	  double zs = pow(quart.z, 2);

	  glm::mat3 rot = glm::mat3();

	  return rot;
  }

  // Get the vector representing the position
  vr::HmdVector3_t VRsystem::GetPosition(vr::HmdMatrix34_t matrix) {
	  vr::HmdVector3_t vector;

	  vector.v[0] = matrix.m[0][3];
	  vector.v[1] = matrix.m[1][3];
	  vector.v[2] = matrix.m[2][3];

	  return vector;
  }

  glm::vec3 VRsystem::ParseTrackingFrame() {

	  // Process SteamVR device states
	  for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	  {
		  // if not connected just skip the rest of the routine
		  if (!m_pHMD->IsTrackedDeviceConnected(unDevice))
			  continue;

		  vr::TrackedDevicePose_t trackedDevicePose;
		  vr::TrackedDevicePose_t *devicePose = &trackedDevicePose;

		  vr::VRControllerState_t controllerState;
		  vr::VRControllerState_t *ontrollerState_ptr = &controllerState;

		  vr::HmdVector3_t position;
		  vr::HmdQuaternion_t quaternion;
		/*
		  if (vr::VRSystem()->IsInputFocusCapturedByAnotherProcess()) {
			  char buf[1024];

			  sprintf_s(buf, sizeof(buf), "\nInput Focus by Another Process\n");
			  printf_s(buf);
		  }
		  */
		  bool bPoseValid = trackedDevicePose.bPoseIsValid;
		  vr::HmdVector3_t vVel;
		  vr::HmdVector3_t vAngVel;
		  vr::ETrackingResult eTrackingResult;

		  // Get what type of device it is and work with its data
		  vr::ETrackedDeviceClass trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(unDevice);
		  switch (trackedDeviceClass) {
		  case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
			  // print stuff for the HMD here, see controller stuff in next case block

			  // get pose relative to the safe bounds defined by the user
			  vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1);

			  // get the position and rotation
			  position = GetPosition(devicePose->mDeviceToAbsoluteTracking);
			  quaternion = GetRotationQuart(devicePose->mDeviceToAbsoluteTracking);
			  glm::vec3 forwardDirection = RotateVectorByQuaternion(quaternion, CONST_FORWARD);


			  // get some data
			  vVel = trackedDevicePose.vVelocity;
			  vAngVel = trackedDevicePose.vAngularVelocity;
			  eTrackingResult = trackedDevicePose.eTrackingResult;
			  bPoseValid = trackedDevicePose.bPoseIsValid;

			  // print the tracking data
			  char buf[1024];
			  sprintf_s(buf, sizeof(buf), "\nHMD\nx: %.2f y: %.2f z: %.2f\n", position.v[0], position.v[1], position.v[2]);
			  printf_s(buf);
			  sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
			  printf_s(buf);
			  sprintf_s(buf, sizeof(buf), "x: %.2f y: %.2f z: %.2f \n", forwardDirection.x, forwardDirection.y, forwardDirection.z);
			  printf_s(buf);

			  //return glm::vec3(position.v[0], position.v[1]-1, position.v[2]);
			  return forwardDirection;
		  }
			 
	  }
  }

  glm::vec3 VRsystem::RotateVectorByQuaternion(vr::HmdQuaternion_t quart, glm::vec3 vec) {
	  
	  glm::vec3 quartVec3 = glm::vec3(quart.x, quart.y, quart.z);
	  // Scalar part of the quaternion
	  float s = quart.w;

	  //Questionable... Fund at https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	  glm::vec3 direction = 2.0f * dot(quartVec3, vec) * quartVec3
		  + (s*s - dot(quartVec3, quartVec3)) * vec
		  + 2.0f * s * cross(quartVec3, vec);

	  return direction;
	  
  }