#version 330

layout(location = 0) in vec3 v;
out vec2 UV;

void main() {
	gl_Position = vec4(v, 1);
	UV = (v.xy+vec2(1,1))/2.0;
}