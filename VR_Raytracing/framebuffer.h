/* 
 * File:   framebuffer.h
 * Author: willbroadbelt
 *
 * Created on 03 November 2017, 17:34
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "GL/glew.h"

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    void Bind(bool left);
    void Flush();
    void ActivateTexture();
	void DrawFramebuffer();
	void Renderwindow();

	inline GLuint getTexture() { return m_renderedTexture; }
	inline int GetWidth() { return m_width; }
	inline int GetHeight() { return m_height; }

    virtual ~Framebuffer();
    
private:
    GLuint m_renderedTexture;
    GLuint m_framebuffer;
    GLuint m_depthrenderbuffer;
	GLuint m_resolveTextureId;
	GLuint m_resolveFramebufferId;
    
    int m_width,m_height;

    
};

#endif /* FRAMEBUFFER_H */
