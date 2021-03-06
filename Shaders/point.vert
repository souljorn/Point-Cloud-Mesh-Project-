#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float alpha;
out vec4 outcolor;

void main()
{
outcolor = vec4(color.x, color.y, alpha, 1);
gl_Position = projection * view * model * vec4(position, 1.0);
//gl_Position = vec4(position.x,position.y, position.z, 1.0);
gl_PointSize = 1.0f;
}
