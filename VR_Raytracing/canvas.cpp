/* 
 * File:   canvas.cpp
 * Author: willbroadbelt
 * 
 * Created on 21 November 2017, 10:41
 * 
 * Largely taken from OpenGL tutorial 14 - render to texture
 */

#include <vector>
#include <iostream>

#include "canvas.h"
#include "shaderloader.h"
#include "chrono";
#include "ctime";


void Canvas::InitQuad() {
    
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f
    };
    
    
    glGenVertexArrays(1, &m_windowVAO);
    glBindVertexArray(m_windowVAO);

    glGenBuffers(1, &m_quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders for the sdf ray marcher
    m_sdf_programID = glCreateProgram();
    GLuint quadVertexShader = CreateShader(LoadShader("resources/passthrough.vert"), GL_VERTEX_SHADER);
    GLuint quadFragmentShader = CreateShader(LoadShader("resources/sdf_ray.frag"), GL_FRAGMENT_SHADER);

    glAttachShader(m_sdf_programID, quadVertexShader);
    glAttachShader(m_sdf_programID, quadFragmentShader);
    glLinkProgram(m_sdf_programID);

    // Check the program
    int InfoLogLength;
    GLint Result = GL_FALSE;
    glGetProgramiv(m_sdf_programID, GL_LINK_STATUS, &Result);
    glGetProgramiv(m_sdf_programID, GL_INFO_LOG_LENGTH, &InfoLogLength);


    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(m_sdf_programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(m_sdf_programID, quadVertexShader);
    glDetachShader(m_sdf_programID, quadFragmentShader);

    glDeleteShader(quadVertexShader);
    glDeleteShader(quadFragmentShader);
	
    this->UniformHandles();
    
    glBindVertexArray(0);
	
	// Create a companion window program
	m_window_programID = glCreateProgram();
	GLuint winVertexShader = CreateShader(LoadShader("resources/passthrough.vert"), GL_VERTEX_SHADER);
	GLuint winFragmentShader = CreateShader(LoadShader("resources/companionWindowFrag.frag"), GL_FRAGMENT_SHADER);

	glAttachShader(m_window_programID, winVertexShader);
	glAttachShader(m_window_programID, winFragmentShader);
	glLinkProgram(m_window_programID);

	// Check the program
	
	Result = GL_FALSE;
	glGetProgramiv(m_window_programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(m_window_programID, GL_INFO_LOG_LENGTH, &InfoLogLength);


	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(m_window_programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(m_window_programID, winVertexShader);
	glDetachShader(m_window_programID, winFragmentShader);

	glDeleteShader(winVertexShader);
	glDeleteShader(winFragmentShader);

	this->UniformHandles();
	
	glBindVertexArray(0);
}


void Canvas::DrawCanvas() {
	auto start = std::chrono::system_clock::now();

    //Left eye:
    m_fbo_left.Bind(true);
	// Clear the screen
	m_display.Clear(0.3f, 0.3f, 0.6f, 0.5f);
    
    // Render to the screen
	glEnable(GL_MULTISAMPLE);

    glBindVertexArray(m_windowVAO);
    // Use our shader
    glUseProgram(m_sdf_programID);

    // Bind our texture in Texture Unit 0
    m_fbo_left.ActivateTexture();
    
    this->UpdateUniforms(true);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vertexbuffer);
    glVertexAttribPointer(
            0, // attribute 0. Must match the layout in the shader.
            2, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*) 0 // array buffer offset
            );
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glDisableVertexAttribArray(0);
	
    glBindVertexArray(0);
	m_fbo_left.Flush();
	glDisable(GL_MULTISAMPLE);

	m_fbo_left.DrawFramebuffer();
	glBindTexture(GL_TEXTURE_2D, 0);
    
    //Right Eye:
    m_fbo_right.Bind(false);
    m_display.Clear(0.3f, 0.3f, 0.6f, 0.5f);
	glEnable(GL_MULTISAMPLE);

    glBindVertexArray(m_windowVAO);
    
    glUseProgram(m_sdf_programID);

    // Bind our texture in Texture Unit 0
    m_fbo_right.ActivateTexture();
    
    this->UpdateUniforms(false);

    //Right eye vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vertexbuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    
    // Draw the triangles 
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
	m_fbo_right.Flush();
	glDisable(GL_MULTISAMPLE);
	m_fbo_right.DrawFramebuffer();

   // m_display.Update();

    glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_MULTISAMPLE);

	glUseProgram(0);
	glFlush();
	glFinish();

	auto finish = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_time = finish - start;
	
	if (b_test) {
		m_timingFile << elapsed_time.count() << "\n";
	}


	

}

void Canvas::RenderCompanionWindow() {

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 500, 500);

	glBindVertexArray(m_windowVAO);
	glUseProgram(m_window_programID);

	// render left eye
	m_fbo_left.ActivateTexture();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_quad_vertexbuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw the triangles 
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glViewport(500, 0, 500, 500);

	// render right eye
	m_fbo_right.ActivateTexture();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_quad_vertexbuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw the triangles 
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray(0);
	glUseProgram(0);

	m_display.Update();
}


bool Canvas::IsClosed() {
    return m_display.IsClosed();
}

void Canvas::UniformHandles() {
    //m_texID = glGetUniformLocation(m_sdf_programID, "tex");
    m_timeID = glGetUniformLocation(m_sdf_programID, "currentTime");
    m_resolution = glGetUniformLocation(m_sdf_programID, "resolution");
    m_camPos = glGetUniformLocation(m_sdf_programID, "camPos");
    m_camDir = glGetUniformLocation(m_sdf_programID, "camDir");
    m_camUp = glGetUniformLocation(m_sdf_programID, "camUp");
	m_projectionRaw = glGetUniformLocation(m_sdf_programID, "projectionRaw");
    m_showstepdepth = glGetUniformLocation(m_sdf_programID, "showStepDepth");
}

void Canvas::UpdateUniforms(bool leftEye) {
	m_backgroundTexture.Bind(0);
	glUniform1f(m_timeID, 1);
    glUniform2f(m_resolution, (GLfloat)m_fbo_left.GetWidth(), (GLfloat)m_fbo_left.GetHeight());
    glUniform1i(m_showstepdepth, (GLuint)0);// 1/0 - T/F
    glm::vec3 pos, dir, up;
	if (leftEye) {
		pos = m_camera_left.GetPos();
		dir = m_camera_left.GetDir();
		up = m_camera_left.GetUp();
	}
	else {
		pos = m_camera_right.GetPos();
		dir = m_camera_right.GetDir();
		up = m_camera_right.GetUp();
	}
	
	glUniform3f(m_camPos, (GLfloat)pos.x, (GLfloat)pos.y, (GLfloat)pos.z);
	glUniform3f(m_camDir, (GLfloat)dir.x, (GLfloat)dir.y, (GLfloat)dir.z);
	glUniform3f(m_camUp, (GLfloat)up.x, (GLfloat)up.y, (GLfloat)up.z);

	glm::vec4 proj = m_vr.getProjectionRaw(leftEye);
	glUniform4f(m_projectionRaw, proj.x, proj.y, proj.z, proj.w);

    
}

void Canvas::UpdateLeftCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) {
    m_camera_left.UpdatePos(pos);
    m_camera_left.UpdateDir(dir);
	m_camera_left.UpdateUp(up);
}

//TODO: Find actual IPD
void Canvas::UpdateRightCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up) {
	glm::vec3 cross = glm::normalize(glm::cross(dir, up));
	glm::vec3 eyePos = pos + (cross*0.062f);
	m_camera_right.UpdatePos(eyePos);
	m_camera_right.UpdateDir(dir);
	m_camera_right.UpdateUp(up);
}

void Canvas::RenderLoop() {

	
	DrawCanvas();
	RenderCompanionWindow();
	

	m_vr.submitEyes(m_fbo_left.getTexture(), m_fbo_right.getTexture());

	UpdateLeftCamera(m_vr.GetPosition(), m_vr.GetForward(), m_vr.GetUp());
	UpdateRightCamera(m_vr.GetPosition(), m_vr.GetForward(), m_vr.GetUp());
	
	

}
