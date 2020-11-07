//
// Created by Connor on 10/24/2019.
//

#pragma once

#include "../FirstPersonScene.h"
#include "../../common/ObjectData.h"

const uint32 numCubes = 3;

struct Cube {
  bool wireframe = false;
  glm::vec3 worldPos;
  glm::vec3 boundingBoxMin;
  glm::vec3 boundingBoxMax;
};

class GUIScene final : public FirstPersonScene {
public:
  GUIScene(GLFWwindow* window);
  void init(uint32 windowWidth, uint32 windowHeight);
  void deinit();
  void drawFrame();

  void key_E_pressed();
  void key_LeftMouseButton_pressed(float32 xPos, float32 yPos);
  void mouseMovement(float32 xOffset, float32 yOffset);

private:
  GLFWwindow* window = NULL;

  bool cursorMode = false;

  Shader* cubeShader;

  VertexAtt cubeVertexAtt;

  float32 startTime = 0;
  float32 deltaTime = 0;
  float32 lastFrame = 0;

  std::string text;
  glm::mat4 projectionMat;
  glm::mat4 viewMat;

  Cube cubes[numCubes];
  glm::mat4 cubeModelMats[3];

  const glm::vec3 cubeColor = { 0.8f, 0.0f, 0.0f };
  const glm::vec3 wireFrameColor = { 0.0f, 0.0f, 0.0f };

  int32 originalCursorMode = GLFW_CURSOR_DISABLED;

  void checkMouseClickCollision(float32 mouseX, float32 mouseY);
  bool checkCubeCollision(glm::vec3* worldRay, glm::vec3* rayOrigin, Cube* cube);
};
