#version 330 core
layout (location = 0) in vec3 vertPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords =  vertPos;
	vec4 pos = projection * view * vec4(vertPos, 1.0);
    gl_Position =  pos;
}  