#version 330 core

uniform sampler2D tex;

in vec3 pos;
out vec4 FragColor;

void main()
{
    //vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    FragColor = texture(tex, ((pos+vec3(1))/2).xy);
}