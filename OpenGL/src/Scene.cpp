//
//  Model.cpp
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "../include/Scene.h"
#include "../include/GLFWManager.h"
#include "../include/shader.hpp"
#include "../include/TimerManager.h"
#include "../include/glfwSet.h"
#include "../include/mesh.h"
#include "../include/sys.h"
#include "../include/log.h"
#include "../include/camera.h"
#include "../include/geometry.h"
#include "../include/texture.h"
#include "SOIL2/SOIL2.h"

#include  <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <random>


const unsigned int NR_LIGHTS = 32;

///////////////////// Just For This Test//////////////////////////////
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
GLuint cubId;
GLuint hdrColorTextur;
GLuint depthRenderBuffer;
GLuint hdrFBO;

GLboolean hdr = false;
GLfloat exposure;
GLboolean inverse_normals = false;
GLuint woodTextureID;
///////////////////////////////////////////////////////////////////

Scene::Scene()
{
}

void Scene::Initialize()
{
	initOpengl();
	initLight();
	initSceneObjs();
	initShader();
	initTexture();
	initFBO();
	initThisDemo();
}

void Scene::initOpengl(void)
{
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

}

void Scene::initTexture(void)
{
	 
	//woodTexture.init(3,GL_RGB,GL_RGB,"D:/CODES/LearnOpenGL-master/resources/textures/wood.png");
	
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image("D:/CODES/LearnOpenGL-master/resources/textures/wood.png", 
		&width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	woodTextureID =  textureID;

}

void Scene::initBOs(GLuint size)
{
	
}

void Scene::initFBO(void)
{
	glGenFramebuffers(1, &hdrFBO);
	// - Create floating point color buffer
	GLuint colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _screenWidth, _screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// - Create depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,  _screenWidth, _screenHeight);
	// - Attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Scene::initLight(void)
{
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
}

void Scene::initShader(void)
{
	std::string  shaderBasePath;
#if defined X_OS_WIN32 || defined X_OS_WIN64
	shaderBasePath = "D:/projects/opengl/HDR/OpenGL/shader/";
#elif  __APPLE__
	shaderBasePath = "/Users/glp/Documents/projects/OpenGL/OpenGL/shader/";
#endif

	shaders.push_back(new Shader); //  
	shaders.push_back(new Shader); //  
	
	std::string vs = shaderBasePath + "lighting.vert"; 
	std::string fs = shaderBasePath + "lighting.frag";
	shaders[0]->LoadShaders(vs.c_str(), fs.c_str(), nullptr);
	Shader * currentShader = shaders[0];
	currentShader->TurnOn();
	
	GLint location = currentShader->GetVariable("diffuseTexture");
	currentShader->SetInt(location,0);
	
	currentShader->TurnOff();


	vs = shaderBasePath + "hdr.vert"; 
	fs = shaderBasePath + "hdr.frag";
	shaders[1]->LoadShaders(vs.c_str(), fs.c_str(), nullptr);
}

void Scene::initSceneObjs(void)
{

#if defined X_OS_WIN32 || defined X_OS_WIN64
	
#elif defined __APPLE__

#endif
	initCub(cubId);
}

void Scene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader * currentShader = shaders[0];
	currentShader->TurnOn();
	initUniformVal(currentShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,woodTextureID);

	for (GLuint i = 0; i < lightPositions.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(currentShader->getShaderId(), ("lights[" + std::to_string((long long )i) + "].Position").c_str()), 1, &lightPositions[i][0]);
		glUniform3fv(glGetUniformLocation(currentShader->getShaderId(), ("lights[" + std::to_string((long long )i) + "].Color").c_str()), 1, &lightColors[i][0]);
	}

	Matrix model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 55.0f));
	glUniformMatrix4fv(glGetUniformLocation(currentShader->getShaderId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(glGetUniformLocation(currentShader->getShaderId(), "inverse_normals"), true);
	
	drawCub(cubId);

	currentShader->TurnOff();
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
}

// This handles all the cleanup for our model, like the VBO/VAO buffers and shaders.
void Scene::Destroy()
{
	for (int i = shaders.size() - 1; i >= 0; i--)
	{
		Shader * s = shaders[i];
		s->Destroy();
		s = NULL;
	}

	for(unsigned int i = 0;i < _modes.size();i++)
	{
		delete _modes[i];
		_modes[i] = NULL;
	}

	for(unsigned int i = 0;i < _textures.size();i++)
	{
		glDeleteBuffers(1,&_textures[i]);
	}

	for(unsigned int i = 0;i < _fbos.size();i++)
	{
		glDeleteFramebuffers(1,&_fbos[i]);
	}
}

void Scene::initThisDemo(void)
{
	

}

void Scene::setScreenWH(unsigned int w,unsigned int h)
{
	_screenWidth = w;
	_screenHeight = h;
}

void modelInput(InputCodes code)
{
	switch (code)
	{
	case  InputCodes::b:
		break;
	case  InputCodes::c:
		break;
	case InputCodes::u:
		break;
	case InputCodes::y:
		break;
	default:
		break;
	}
}

void windowSize(GLFWwindow*window, int width, int height)  
{
	glViewport(0, 0, width, height);
}
