#version 330 core

//TO DO: Write code to set the model's position in relation to the view 
//Be sure to pass in the correct variables and set your out variables
//-----------------------------------------------------------------------
layout (location = 1) in vec3 teapotPosition;
layout (location = 2) in vec3 teapotNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 Position;

//void main()
//{
//	gl_Position = projection * view*model*vec4(teapotPosition, 1.0);
//}
//-----------------------------------------------------------------------

void main()
{
    Normal = inverse(model) * teapotNormal;
    Position = vec3(model * vec4(teapotPosition, 1.0));
    gl_Position = projection * view * model * vec4(teapotPosition, 1.0);
} 