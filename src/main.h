#pragma once

#include "LearnOpenGLPlatform.h"

int main();
void loadGLFW();
GLFWwindow* createWindow();
void initializeGLAD();
void initImgui(GLFWwindow* window);
void toggleWindowSize(GLFWwindow* window, uint32 width, uint32 height);
void runScenes(GLFWwindow* window);

// ===== double triangle values =====
const uint32 triangleVertexAttSize = 8;
const uint32 triangleNumElements = 2;
const float32 triangleVertices[] = {
  // First triangle
  // positions            // colors           // texture coords
  -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
  0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
  0.0f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,   0.5, 1.0f,  // top (for first triangle) / bottom (for second triangle)
  // Second triangle
  -0.25f, 0.75f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // top left
  0.25f, 0.75f, 0.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f  // top right
};
const uint32 indices[]{
        0, 1, 2,    // first triangle
        2, 3, 4     // second triangle
};
// ===== double triangle values =====