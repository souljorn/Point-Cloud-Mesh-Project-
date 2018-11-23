//===============================================================================
// Basic Shader Program
// learnopengl.com
//===============================================================================

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#ifndef GL_TEXTURE_TOO_LARGE
#ifdef GL_TEXTURE_TOO_LARGE_EXT
#define GL_TEXTURE_TOO_LARGE GL_TEXTURE_TOO_LARGE_EXT
#else
#define GL_TEXTURE_TOO_LARGE 0x8065
#endif
#endif

class Shader
{
public:
    GLuint Program;
	Shader() {}

    Shader(const char *vertexPath, const char *fragmentPath)
    {
        //declarations
        std::string vertexShaderCode;
        std::string fragmentShaderCode;
        std::ifstream vertexShaderFile;
        std::ifstream fragmentShaderFile;
        
        //opening from path
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);

        //ss
        std::stringstream vShaderStream, fShaderStream;

        //read from buffer into stream
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();

        //closing to clean
        vertexShaderFile.close();
        fragmentShaderFile.close();

        //stream to string, string to c string
        vertexShaderCode = vShaderStream.str();
        fragmentShaderCode = fShaderStream.str();
        const char *vCode = vertexShaderCode.c_str();
        const char *fCode = fragmentShaderCode.c_str();

        //initializing shaders
        GLuint vertexShader, fragmentShader;
        GLint success;
        char infoLog[512];
        
        //creating vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vCode, NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            //output error message on compilation failure
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "Error in vertex shader compilation:\n" << infoLog << std::endl;
        }
        //creating fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fCode, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "Error in fragment shader compilation:\n" << infoLog << std::endl;
            printf("Code is: %s\n", fCode);

        }

        //creating program and linking
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertexShader);
        glAttachShader(this->Program, fragmentShader);
        glLinkProgram(this->Program);
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cout << "Error linking shader program:\n" << infoLog << std::endl;
        }
        //USELESS! GET RID OF THEM!
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }
    //a way to use the shader
    void Use()
    {
        glUseProgram(this->Program);
    }
    
    // Taken from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        int location = glGetUniformLocation(this->Program, name.c_str());

        GLenum glErr;
        int    retCode = 0;
        
        glErr = glGetError();
        if (glErr != GL_NO_ERROR)
        {


			/*
			* A cut-down local version of gluErrorString to avoid depending on GLU.
			*/
			const char* myerror = NULL;
			switch (glErr) {
				case GL_INVALID_ENUM: myerror =  "invalid enumerant";
				case GL_INVALID_VALUE: myerror =  "invalid value";
				case GL_INVALID_OPERATION: myerror =  "invalid operation";
#ifndef GL_ES_VERSION_2_0
				case GL_STACK_OVERFLOW: myerror =  "stack overflow";
				case GL_STACK_UNDERFLOW: myerror =  "stack underflow";
#endif
				case GL_OUT_OF_MEMORY: myerror =  "out of memory";
				case GL_TABLE_TOO_LARGE: myerror =  "table too large";
				case GL_INVALID_FRAMEBUFFER_OPERATION: myerror =  "invalid framebuffer operation";
				case GL_TEXTURE_TOO_LARGE: myerror =  "texture too large";
				default: myerror =  "unknown GL error";
			}

            printf("glError in file %s @ line %d: %s\n",
                   "SHADRER.H", 111, myerror);
            retCode = 1;
        }

        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }
    
    
};

#endif

