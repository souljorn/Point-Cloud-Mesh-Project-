#version 330 core


//TO DO: Write code to compute the reflection vector - GLSL has a function for this.
// Then sample your cube map and update the fragment color.
//Be sure to pass in the correct variables and set your out variables
//-----------------------------------------------------------------------

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

out vec4 FragColor;
void main()
{

//   FragColor  = vec4(1,0,0,1);

    vec3 I = normalize(Position - cameraPos);
    vec3 R = cameraPos * reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);

}
//-----------------------------------------------------------------------
