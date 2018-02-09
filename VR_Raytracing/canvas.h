/* 
 * File:   canvas.h
 * Author: willbroadbelt
 *
 * Created on 21 November 2017, 10:41
 */

#ifndef CANVAS_H
#define CANVAS_H

#include "framebuffer.h"
#include "display.h"
#include "camera.h"
#include "vrsystem.h";

class Canvas
{
public:
    Canvas(int width, int height, const std::string& title = "Window") : 
		m_display(1000, 500, title), m_fbo_left(width, height), m_fbo_right(width, height),
        m_camera_left(glm::vec3(0.0f,10.0f,8.0f), glm::vec3(0,0,-1)),
        m_camera_right(glm::vec3(0.0f,10.0f,8.0f), glm::vec3(0,0,-1)), m_vr() { InitQuad(); };
    void DrawCanvas();
    bool IsClosed();
    void UpdateLeftCamera(const glm::vec3& pos, const glm::vec3& dir);
	void UpdateRightCamera(const glm::vec3& pos, const glm::vec3& dir);
	void RenderScene(bool leftEye);
	void RenderLoop();
    
private:
    void InitQuad();
	void InitController();
    void UpdateUniforms(bool leftEye);
    void UniformHandles();
	void RenderStereoTargets();
	void RenderCompanionWindow();
    
    
    Display m_display;
    Framebuffer m_fbo_left,m_fbo_right;
    Camera m_camera_left, m_camera_right;
	VRsystem m_vr;
    GLuint m_quad_vertexbuffer, m_sdf_programID, m_windowVAO, m_controllerVAO, m_controllerVertBuffer, m_window_programID;
    GLuint m_texID, m_timeID, m_resolution, m_camPos, m_camDir, m_camUp, m_showstepdepth;
};

#endif /* CANVAS_H */
