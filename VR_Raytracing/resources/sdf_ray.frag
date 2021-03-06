#version 330

uniform vec2 resolution;
uniform float currentTime;
uniform vec3 camPos;
uniform vec3 camDir;
uniform vec3 camUp;
uniform sampler2D tex;
uniform bool showStepDepth;
uniform vec4 projectionRaw;

in vec3 pos;

out vec3 color;

#define PI 3.1415926535897932384626433832795
#define RENDER_DEPTH 800
#define CLOSE_ENOUGH 0.00001

#define BACKGROUND -1
#define BALL 0
#define BASE 1

#define GRADIENT(pt, func) vec3( \
    func(vec3(pt.x + 0.0001, pt.y, pt.z)) - func(vec3(pt.x - 0.0001, pt.y, pt.z)), \
    func(vec3(pt.x, pt.y + 0.0001, pt.z)) - func(vec3(pt.x, pt.y - 0.0001, pt.z)), \
    func(vec3(pt.x, pt.y, pt.z + 0.0001)) - func(vec3(pt.x, pt.y, pt.z - 0.0001)))

const vec3 LIGHT_POS[] = vec3[](vec3(5, 18, 10));

///////////////////////////////////////////////////////////////////////////////

vec3 getBackground(vec3 dir) {
  float u = 0.5 + atan(dir.z, -dir.x) / (2 * PI);
  float v = 0.5 - asin(dir.y) / PI;
  vec4 texColor = texture(tex, vec2(u, v));
  return texColor.rgb;
}

vec3 getRayDir() {
  vec3 xAxis = normalize(cross(camDir, camUp));
  float left = projectionRaw.x;
  float right = projectionRaw.y;
  float top = projectionRaw.z;
  float bottom = projectionRaw.w;
  float horizontal = right-left;
  float vertical = bottom-top;
  vec2 p = 0.5 * pos.xy + 0.5;

  return normalize((left + p.x * horizontal) * xAxis+ (top + p.y * vertical) * camUp + camDir);
}

///////////////////////////////////////////////////////////////////////////////

float sphere(vec3 pt) {
  return length(pt) - 1;
}

float cube(vec3 pt) {
	vec3 d = abs(pt) - vec3(1); //radius of 1
	return min(max(d.x, max(d.y,d.z)), 0.0) + length(max(d, 0.0));
}

float torus(vec3 pt, vec2 rs){
	vec2 q = vec2(length(pt.xz)-rs.x,pt.y);
	return length(q) - rs.y;
}

float sdPlane(vec3 pt, vec4 n){
	//vec4 norm = normalize(n);
	return dot(pt,n.xyz) +n.w;
}

float tCube(vec3 pt, vec3 pos){
	return cube(pt - pos);
}
float tSphere(vec3 pt, vec3 pos){
	return sphere(pt - pos);
}


float smin(float a, float b) {//Blend
	float k = 0.2;
	float h = clamp(0.5 + 0.5 * (b-a) /k, 0, 1);
	return mix(b, a, h) - k * h * (1-h);
}

float CubesAndSpheres(vec3 pt) {
  float fScene = 0;
  
  float aC = tCube(pt, vec3(-3,0,-3));
  float aS = tSphere(pt, vec3(-2,0,-2));
  float un = min(aC,aS);//union
  fScene = un;
  
  float bC = tCube(pt, vec3(3,0,-3));
  float bS = tSphere(pt, vec3(4,0,-2));
  float difference = max(bC,-bS);
  fScene = min(fScene,difference);
  
  float cC = tCube(pt, vec3(-3,0,3));
  float cS = tSphere(pt, vec3(-2,0,4));
  float blend = smin(cC,cS);
  fScene = min(fScene,blend);
  
  float dC = tCube(pt, vec3(3,0,3));
  float dS = tSphere(pt, vec3(4,0,4));
  float intersection = max(dC,dS);
  fScene = min(fScene,intersection);
  
  return fScene;
}

float TorusScene(vec3 pt){
	float fScene = torus(pt - vec3(0,3,0), vec2(3,1));
	
	return fScene;
}

//////////////////// ---Test Scenes---  //////////////////////

//Circle of cubes with faces directed towards the center
// n = number of cubes in circle
float CircleOfCubes(vec3 pt, int n, float scale){
    float scene = tCube(pt, vec3(scale,0,0));
    for(int i = 1; i<n; i++){
        float angle = 2*i*PI/n ;
        float cosine = cos(angle);
        float sine = sin(angle);
        mat4 rotation = mat4(cosine, 0, -sine, 0, //1st Column
                            0, 1, 0, 0,
                            sine, 0, cosine, 0,
                            scale*cosine, 0, scale*sine, 1);
        scene = min(scene, cube((inverse(rotation) * vec4(pt,1)).xyz));
    }
    return scene;
}

//Psuedo random number generator - hard to find the origin of this equation
float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

//Place n cubes at random with faces pointing towards the center
float RandomCubesCenterFacing(vec3 pt, int n, int scale){
    float scene = tCube(pt, vec3(scale,0,0));
    for(int i = 1; i<n; i++){
        float randomScale = scale * rand(vec2(i,1));
        float angle = 2*i*PI/n ;
        float cosine = cos(angle);
        float sine = sin(angle);
        mat4 rotation = mat4(cosine, 0, -sine, 0, //1st Column
                            0, 1, 0, 0,
                            sine, 0, cosine, 0,
                            randomScale*cosine, 0, randomScale*sine, 1);
        scene = min(scene, cube((inverse(rotation) * vec4(pt,1)).xyz));
    }
    return scene;
}

//Place n cubes at random with faces pointing randomly
float RandomCubes(vec3 pt, int n, int scale){
    float scene = tCube(pt, vec3(scale,0,0));
    for(int i = 1; i<n; i++){
        float randomScale = scale * rand(vec2(i,1));
        float random = rand(vec2(i,i));
        float angle = 2*PI*random ;
        float cosine = cos(angle);
        float sine = sin(angle);
        mat4 rotation = mat4(cosine, 0, -sine, 0, //1st Column
                            0, 1, 0, 0,
                            sine, 0, cosine, 0,
                            randomScale*cosine, 0, randomScale*sine, 1);
        scene = min(scene, cube((inverse(rotation) * vec4(pt,1)).xyz));
    }
    return scene;
}

//Infinite objects
float InfiniteObjs(vec3 pt){
	vec3 c = vec3(4,0,4);
	vec3 q = mod(pt,c) - 0.5*c;
	return cube(q);
}

//Returns the scene to use (used so only have to update in one place)
float TestScene(vec3 pt){
   	return InfiniteObjs(pt); 
	//return RandomCubesCenterFacing(pt, 9, 6);
    	//return CubesAndSpheres(pt);
}

float sceneWithPlane(vec3 pt){
	float plane = sdPlane(pt-vec3(0,-1,0), vec4(0,1,0,1));
        float scene = TestScene(pt);
  	return min(scene, plane);
}
///////////////////////////////////////////////////

vec3 getColor(vec3 pt) {
if(pt.y<-1){
	float objs = TestScene(pt);
	float split = mod(objs,1);
	float border = mod(objs,5);
	if(4.75<=border){
		return vec3(0);
	}else{
		return mix(vec3(0.4,1,0.4),vec3(0.4,0.4,1), split);
	}
}else{
  return vec3(0.8,0.8,0.8);
}

}

///////////////////////////////////////////////////////////////////////////////

float shade(vec3 eye, vec3 pt, vec3 n) {
  float diffuse = 0;
  float specular = 0;
  float ambient = 0.1;
  float diffCo = 1;
  float specCo  = 0.4;
  float specShin = 256;
  
  vec3 e = normalize(pt-eye);
  
  
  for (int i = 0; i < LIGHT_POS.length(); i++) {
    vec3 l = normalize(LIGHT_POS[i] - pt); 
    float nDotL = clamp(dot(n, l),0.0, 1.0);
    vec3 r = (reflect(-l,n));
    
    diffuse += diffCo * nDotL;
    specular += specCo * pow(clamp(0, dot(e,r), 1),specShin);
  }
  return ambient + diffuse + specular;
}

float getShadow(vec3 pt) {
  vec3 lightDir = normalize(LIGHT_POS[0] - pt);
  float kd = 1;
  int step = 0;

  for (float t = 0.1; t < length(LIGHT_POS[0] - pt) && step < RENDER_DEPTH && kd > 0.001; ) {
    float d = TestScene(pt + t * lightDir);
    if (d < 0.001) {
      kd = 0;
    } else {
      kd = min(kd, 16 * d / t);
    }
    t += d;
    step++;
  }
  return kd;
}

vec3 illuminate(vec3 camPos, vec3 rayDir, vec3 pt) {
  vec3 c, n;
  n = normalize(GRADIENT(pt, sceneWithPlane));
  c = getColor(pt);
  return shade(camPos, pt, n) * c * (0.1 + getShadow(pt));
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

vec3 raymarch(vec3 camPos, vec3 rayDir) {
  int step = 0;
  float t = 0;
  
  

  for (float d = 1000; step < RENDER_DEPTH && abs(d) > CLOSE_ENOUGH; t += abs(d)) {
   // d = cube(camPos + t * rayDir);
    d = sceneWithPlane(camPos + t * rayDir);
    step++;
  }

  if (step == RENDER_DEPTH) {
    return getBackground(rayDir);
  } else if (showStepDepth) {
    return vec3(float(step) / RENDER_DEPTH);
  } else {
    return illuminate(camPos, rayDir, camPos + t * rayDir);
  }
}

///////////////////////////////////////////////////////////////////////////////

void main() {
  color = raymarch(camPos, getRayDir());
}