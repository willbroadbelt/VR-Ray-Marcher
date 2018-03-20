/* 
 * File:   framebuffer.cpp
 * Author: willbroadbelt
 * 
 * Created on 03 November 2017, 17:34
 */

#include "framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(int width, int height)
{
    m_width = width;
    m_height = height;
    
    m_framebuffer = 0;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    glGenTextures(1, &m_renderedTexture);
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderedTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	
    glGenRenderbuffers(1, &m_depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer);
    
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Frame buffer failed" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glGenFramebuffers(1, &m_resolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_resolveFramebufferId);

	glGenTextures(1, &m_resolveTextureId);
	glBindTexture(GL_TEXTURE_2D, m_resolveTextureId);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 500, 500, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_resolveTextureId, 0);
	
    
    GLenum DrawBuffers2[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers2);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Frame buffer failed" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind(bool left)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glViewport(0, 0, m_width, m_height);
}

void Framebuffer::Flush()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteBuffers(1, &m_framebuffer);
    glDeleteBuffers(1, &m_depthrenderbuffer);
    glDeleteTextures(1, &m_renderedTexture);
    
}

void Framebuffer::ActivateTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
}

void Framebuffer::DrawFramebuffer() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolveFramebufferId);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, 500, 500,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

//Problem area
void Framebuffer::Renderwindow() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_resolveTextureId);
	
}