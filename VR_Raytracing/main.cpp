
#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>
#include "GL/glew.h"
#include "SDL2\SDL.h"
#include "glm\glm.hpp"

#include "display.h"
#include "camera.h"
#include "framebuffer.h"
#include "shaderloader.h"
#include "canvas.h"

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const int RENDER_WIDTH = 1586;
const int RENDER_HEIGHT = 1762;

int main(int argc, char** argv) {
    Canvas canvas(RENDER_WIDTH, RENDER_HEIGHT);
    
    float counter = 0.0f,sin, cos;
    
    clock_t t;
    
    while(!canvas.IsClosed())
    {
        t  = clock();
        
        sin = sinf(counter);
        cos = cosf(counter);
        
        //NB: Check if event occurred and only update screen if it has.
        //canvas.UpdateLeftCamera(glm::vec3(0,0,8),glm::vec3(sin,0,cos));
        canvas.RenderLoop();
        
        //Frame rate
        if((int)(counter)%5==0 && clock()-t > 0){
            t = clock() - t;
			double millisecs = (double)(t)/CLOCKS_PER_SEC;
           // printf("Frame rate = %f. Or %d ticks.\n",1/millisecs,t);
            t = clock();
        }
        
        counter+=0.01f;
 
    }
    
    return 0;
}
