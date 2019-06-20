#pragma once

#include <glm/glm.hpp>

#include "LearnOpenGLPlatform.h"
#include "Kernels.h"
#include "Scene.h"
#include "ObjectData.h"
#include "Shader.h"

const glm::vec3 colors[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.75f, 0.25f, 0.0f),
	glm::vec3(0.5f, 0.5f, 0.0f),
	glm::vec3(0.25f, 0.75f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.75f, 0.25f),
	glm::vec3(0.0f, 0.5f, 0.5f),
	glm::vec3(0.0f, 0.25f, 0.75f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.25f, 0.0f, 0.75f),
	glm::vec3(0.5f, 0.0f, 0.5f),
	glm::vec3(0.75f, 0.0f, 0.25f)
};

struct FrameBuffer {
	uint32 frameBuffer = 0;
	uint32 frameBufferTexture = 0;
	uint32 rbo = 0;
};

class InfiniteCubeScene final : public FirstPersonScene {
public:
	InfiniteCubeScene(GLFWwindow* window, uint32 initScreenHeight, uint32 initScreenWidth);
	void runScene();

	// override
	void frameBufferSize(uint32 width, uint32 height);

private:
	Shader cubeShader;
	Shader cubeOutlineShader;
	Shader frameBufferShader;

	// frame rate
	float32 deltaTime = 0.0f;	// Time between current frame and last frame
	float32 lastFrame = 0.0f; // Time of last frame

	FrameBuffer frameBuffers[2] = {
		{0,0,0},
		{0,0,0}
	};

	void renderLoop(GLFWwindow* window, uint32& shapesVAO, uint32& quadVAO);
};