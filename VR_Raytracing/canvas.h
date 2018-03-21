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
#include "vrhandler.h";
#include "texture.h";

#include <fstream>
using namespace std;

class Canvas
{
public:
    Canvas(int width, int height, bool testMode, const std::string& title = "Window") :
		m_display(1000, 500, title), m_fbo_left(width, height), m_fbo_right(width, height),
        m_camera_left(glm::vec3(0.0f,10.0f,8.0f), glm::vec3(0,0,-1)),
        m_camera_right(glm::vec3(0.0f,10.0f,8.0f), glm::vec3(0,0,-1)), m_vr(),
		m_backgroundTexture("resources/background_sky.jpg"), b_test(testMode) {InitQuad();
		if(testMode) m_timingFile.open("o_infinteobjs_x_4_800.txt");//o_scene_num_scale_depth.txt
	};
    void DrawCanvas();
    bool IsClosed();
    void UpdateLeftCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up);
	void UpdateRightCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up);
	void RenderLoop();
	~Canvas() { m_timingFile.close(); }
    
private:
    void InitQuad();
    void UpdateUniforms(bool leftEye);
    void UniformHandles();
	void RenderCompanionWindow();
    
    
    Display m_display;
    Framebuffer m_fbo_left,m_fbo_right;
    Camera m_camera_left, m_camera_right;
	VRHandler m_vr;
	Texture m_backgroundTexture;
    GLuint m_quad_vertexbuffer, m_sdf_programID, m_windowVAO, m_window_programID;
    GLuint m_texID, m_timeID, m_resolution, m_camPos, m_camDir, m_camUp, m_projectionRaw, m_showstepdepth;

	ofstream m_timingFile;
	bool b_test;
};

#endif /* CANVAS_H */
