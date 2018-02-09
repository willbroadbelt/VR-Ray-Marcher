#version 330 core

uniform sampler2D tex;
uniform vec2 tex_size;

in vec2 UV;

layout(location = 0) out vec4 out_color;

void main()
{
    //vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    out_color = texture(tex, UV.xy).xyz;
}