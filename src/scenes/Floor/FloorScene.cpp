#include "FloorScene.h"
#include "../../common/FileLocations.h"
#include "../../Model.h"
#include "../../common/Util.h"
#include "../../common/ObjectData.h"

const uint32 SHADOW_MAP_WIDTH = 2048;
const uint32 SHADOW_MAP_HEIGHT = 2048;

FloorScene::FloorScene(GLFWwindow* window, uint32 initScreenHeight, uint32 initScreenWidth)
  : GodModeScene(window, initScreenHeight, initScreenWidth),
  directionalLightShader(lightSpaceVertexShaderFileLoc, directionalLightShadowMapFragmentShaderFIleLoc),
  singleColorShader(posVertexShaderFileLoc, singleColorFragmentShaderFileLoc),
  depthMapShader(simpleDepthVertexShaderFileLoc, emptyFragmentShaderFileLoc) {}

void FloorScene::runScene()
{
  uint32 floorVAO, floorVBO, floorEBO;
  initializeQuadVertexAttBuffers(floorVAO, floorVBO, floorEBO);

  uint32 cubeVAO, cubeVBO, cubeEBO;
  initializeCubePosTexNormAttBuffers(cubeVAO, cubeVBO, cubeEBO);

  renderLoop(floorVAO, cubeVAO);

  glDeleteVertexArrays(1, &floorVAO);
  glDeleteBuffers(1, &floorVBO);
  glDeleteBuffers(1, &floorEBO);

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &cubeEBO);
}


void FloorScene::renderLoop(uint32 floorVAO, uint32 cubeVAO)
{
  uint32 floorTextureId, floorNormalTextureId, cubeTextureId;
  load2DTexture(brickTextureLoc, floorTextureId, false, true);
  load2DTexture(brickNormalTextureLoc, floorNormalTextureId, false, true);
  load2DTexture(cementTextureLoc, cubeTextureId, false, true);

  uint32 depthMapTextureId, depthMapFBO;
  generateDepthMap(depthMapTextureId, depthMapFBO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, floorTextureId);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, cubeTextureId);

  const glm::mat4 cameraProjMat = glm::perspective(glm::radians(camera.Zoom), (float32)viewportWidth / (float32)viewportHeight, 0.1f, 120.0f);

  const float32 nearPlane = 1.0f, farPlane = 40.0f, projectionDimens = 12.0f;
  // Note: orthographic projection is used for directional lighting, as all light rays are parallel
  const glm::mat4 lightProjMat = glm::ortho(-projectionDimens, projectionDimens, -projectionDimens, projectionDimens, nearPlane, farPlane);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // background clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
  const float32 lightScale = 0.5f;

  const float32 floorScale = 16.0f;
  const glm::vec3 floorPosition(0.0f, -3.0f, 0.0f);

  const float32 cubeScale1 = 3.0f;
  const glm::vec3 cubePosition1 = glm::vec3(0.0f, floorPosition.y + (cubeScale1 / 2.0f), 0.0f);

  const float32 cubeScale2 = 2.2f;
  const glm::vec3 cubePosition2 = glm::vec3(8.0f, floorPosition.y + (cubeScale2 / 2) + 2.0f, 4.0f);
  
  const float32 cubeScale3 = 1.7f;
  const glm::vec3 cubePosition3 = glm::vec3(2.0f, floorPosition.y + (cubeScale3 / 2.0f) + 4.0f, 3.0f);
  
  const float32 lightRadius = 16.0f;
  const float32 lightHeightOffset = 16.0f;

  // Turn on gamma correction for entire scene
  //glEnable(GL_FRAMEBUFFER_SRGB);

  directionalLightShader.use();

  // set lighting 1
  directionalLightShader.setUniform("directionalLight.color.ambient", lightColor * 0.05f);
  directionalLightShader.setUniform("directionalLight.color.diffuse", lightColor * 0.3f);
  directionalLightShader.setUniform("directionalLight.color.specular", lightColor);

  const uint32 shadowMap2DSamplerIndex = 2;
  directionalLightShader.setUniform("shadowMap", shadowMap2DSamplerIndex);

  uint32 globalVSUniformBuffer;
  uint32 globalVSBufferBindIndex = 0;
  uint32 globalVSBufferViewMatOffset = sizeof(glm::mat4);
  {
    glGenBuffers(1, &globalVSUniformBuffer);

    glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    glBindBufferRange(GL_UNIFORM_BUFFER,		// target
      globalVSBufferBindIndex,	// index of binding point 
      globalVSUniformBuffer,	// buffer id
      0,						// starting offset into buffer object
      4 * 16);				// size: 4 vec3's, 16 bits alignments

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cameraProjMat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    directionalLightShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
    singleColorShader.bindBlockIndex("globalBlockVS", globalVSBufferBindIndex);
  }

  camera.Position += glm::vec3(0.0f, 10.0f, 20.0f);

  // floor data
  glm::mat4 floorModelMat = glm::mat4();
  floorModelMat = glm::translate(floorModelMat, floorPosition);
  floorModelMat = glm::scale(floorModelMat, glm::vec3(floorScale, 1.0f, floorScale));
  floorModelMat = glm::rotate(floorModelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // cube data
  glm::mat4 cubeModelMat1 = glm::mat4();
  cubeModelMat1 = glm::translate(cubeModelMat1, cubePosition1);
  cubeModelMat1 = glm::scale(cubeModelMat1, glm::vec3(cubeScale1));
  glm::mat4 cubeModelMat2 = glm::mat4();
  cubeModelMat2 = glm::translate(cubeModelMat2, cubePosition2);
  cubeModelMat2 = glm::scale(cubeModelMat2, glm::vec3(cubeScale2));
  glm::mat4 cubeModelMat3 = glm::mat4();
  cubeModelMat3 = glm::translate(cubeModelMat3, cubePosition3);
  cubeModelMat3 = glm::scale(cubeModelMat3, glm::vec3(cubeScale3));

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  // NOTE: render/game loop
  while (glfwWindowShouldClose(window) == GL_FALSE)
  {
    // check for input
    processKeyboardInput(window, this);
    processXInput(this);

    float32 t = (float32)glfwGetTime();
    deltaTime = t - lastFrame;
    lastFrame = t;

    glm::mat4 viewMat = camera.GetViewMatrix(deltaTime);

    glBindBuffer(GL_UNIFORM_BUFFER, globalVSUniformBuffer);
    // update global view matrix uniform
    glBufferSubData(GL_UNIFORM_BUFFER, globalVSBufferViewMatOffset, sizeof(glm::mat4), glm::value_ptr(viewMat));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // light data
    glm::vec3 lightPosition = glm::vec3(sin(t) * lightRadius, lightHeightOffset, cos(t) * lightRadius);
    glm::mat4 lightModel;
    lightModel = glm::translate(lightModel, lightPosition);
    lightModel = glm::scale(lightModel, glm::vec3(lightScale));

    // render depth map from light's point of view
    glm::mat4 lightView = glm::lookAt(lightPosition,
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjMat * lightView;


    depthMapShader.use();
    depthMapShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // depth map for floor
    depthMapShader.setUniform("model", floorModelMat);
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
                   6, // number of elements to draw (3 vertices per triangle * 2 triangles per quad)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO


    // depth map for cubes
    depthMapShader.setUniform("model", cubeModelMat1);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
                   cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO
    glBindVertexArray(0);

    depthMapShader.setUniform("model", cubeModelMat2);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
      cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO
    glBindVertexArray(0);

    depthMapShader.setUniform("model", cubeModelMat3);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
      cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render scene using the depth map for shadows (using depth map)
    glViewport(0, 0, viewportWidth, viewportHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0 + shadowMap2DSamplerIndex);
    glBindTexture(GL_TEXTURE_2D, depthMapTextureId);

    // draw directional light representation
    singleColorShader.use();
    glBindVertexArray(cubeVAO);
    singleColorShader.setUniform("model", lightModel);
    singleColorShader.setUniform("color", lightColor);
    glDrawElements(GL_TRIANGLES, // drawing mode
                   cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                   GL_UNSIGNED_INT, // type of the indices
                   0); // offset in the EBO

    glm::vec3 lightDir = glm::normalize(lightPosition);
    directionalLightShader.use();
    directionalLightShader.setUniform("viewPos", camera.Position);
    directionalLightShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
    directionalLightShader.setUniform("directionalLight.direction", lightDir);

    // draw floor
    directionalLightShader.setUniform("model", floorModelMat);
    directionalLightShader.setUniform("material.diffTexture1", 0);
    directionalLightShader.setUniform("material.specTexture1", 0);
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
      6, // number of elements to draw (3 vertices per triangle * 2 triangles per quad)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO

    // draw cubes
    directionalLightShader.setUniform("model", cubeModelMat1);
    directionalLightShader.setUniform("material.diffTexture1", 1);
    directionalLightShader.setUniform("material.specTexture1", 1);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, // drawing mode
      cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO
    directionalLightShader.setUniform("model", cubeModelMat2);
    glDrawElements(GL_TRIANGLES, // drawing mode
      cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO
    directionalLightShader.setUniform("model", cubeModelMat3);
    glDrawElements(GL_TRIANGLES, // drawing mode
      cubePosTexNormNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
      GL_UNSIGNED_INT, // type of the indices
      0); // offset in the EBO
    glBindVertexArray(0);

    glfwSwapBuffers(window); // swaps double buffers
    glfwPollEvents(); // checks for events (ex: keyboard/mouse input)
  }
}

void FloorScene::generateDepthMap(uint32& depthMapTextureId, uint32& depthMapFBO)
{
  glGenFramebuffers(1, &depthMapFBO);

  glGenTextures(1, &depthMapTextureId);
  glBindTexture(GL_TEXTURE_2D, depthMapTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // ensure that areas outside of the shadow map are NEVER determined to be in shadow
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float32 borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextureId, 0);
  // The following two calls tell OpenGL that we are not trying to output any kind of color
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}