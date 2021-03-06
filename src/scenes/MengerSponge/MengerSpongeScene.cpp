//
// Created by Connor on 11/21/2019.
//

#include <imgui/imgui.h>
#include "MengerSpongeScene.h"
#include "../../common/FileLocations.h"
#include "../../common/Util.h"
#include "../../common/Input.h"

const uint32 diff1TextureIndex = 0;
const uint32 spec1TextureIndex = diff1TextureIndex + 1;
const uint32 diff2TextureIndex = spec1TextureIndex + 1;
const uint32 spec2TextureIndex = diff2TextureIndex + 1;

MengerSpongeScene::MengerSpongeScene(GLFWwindow* window): FirstPersonScene(), window(window)
{
  camera.Position = glm::vec3(0.0f, 1.0f, 30.0f);
}

const char* MengerSpongeScene::title()
{
  return "Menger Prison SDF + Raster Cube";
}

void MengerSpongeScene::init(Extent2D windowExtent)
{
  FirstPersonScene::init(windowExtent);

  enableCursor(window, showDebugWindows);

  mengerSpongeShader = new ShaderProgram(UVCoordVertexShaderFileLoc, MengerSpongeFragmentShaderFileLoc);
  pixel2DShader = new ShaderProgram(pixel2DVertexShaderFileLoc, textureFragmentShaderFileLoc);
  cubeShader = new ShaderProgram(CubePosNormTexVertexShaderFileLoc, CubeTextureFragmentShaderFileLoc);

  quadVertexAtt = initializeFramebufferQuadVertexAttBuffers();
  cubeVertexAtt = initializeCubePosNormTexVertexAttBuffers();

  // We want a framebuffer equal in size to our highest resolution
  dynamicResolutionFBO = initializeFramebuffer(screenResolutions[ArrayCount(screenResolutions) - 1]);
  publicPseudoDrawFramebuffer = {
          dynamicResolutionFBO.id,
          dynamicResolutionFBO.colorAttachment,
          dynamicResolutionFBO.depthStencilAttachment,
          currentResolution.width,
          currentResolution.height
  };

  // NOTE: This is helps maintain same projection for both the ray marching and rasterization
  // If how we shoot rays change, this must change. If this changes, how we shoot rays must change.
  const float rayMarchFovVertical = glm::radians(53.14f);
  projectionMat = glm::perspective(rayMarchFovVertical, (float32)currentResolution.width / (float32)currentResolution.height, 0.1f, 200.0f);

  load2DTexture(scarabWingsTextureLoc, textureDiff1Id, true, true, &textureWidth, &textureHeight);
  load2DTexture(scarabWingsSpecTextureLoc, textureSpec1Id, true, false);
  load2DTexture(scarabTextureLoc, textureDiff2Id, true, true);
  load2DTexture(scarabSpecTextureLoc, textureSpec2Id, true, false);

  mengerSpongeShader->use();
  mengerSpongeShader->setUniform("viewPortResolution", glm::vec2(currentResolution.width, currentResolution.height));
  mengerSpongeShader->setUniform("directionalLight.color.ambient", directionalLightAmb);
  mengerSpongeShader->setUniform("directionalLight.color.diffuse", directionalLightDiff);
  mengerSpongeShader->setUniform("directionalLight.color.specular", directionalLightSpec);
  mengerSpongeShader->setUniform("directionalLight.direction", directionalLightDir);
  mengerSpongeShader->setUniform("projection", projectionMat);

  cubeShader->use();
  cubeShader->setUniform("projection", projectionMat);
  cubeShader->setUniform("material.diffTexture", 0);
  cubeShader->setUniform("material.specTexture", 1);
  cubeShader->setUniform("material.shininess", 16.0f);
  cubeShader->setUniform("directionalLight.color.ambient", directionalLightAmb);
  cubeShader->setUniform("directionalLight.color.diffuse", directionalLightDiff);
  cubeShader->setUniform("directionalLight.color.specular", directionalLightSpec);
  cubeShader->setUniform("directionalLight.direction", directionalLightDir);

//  pixel2DShader->use();
//  pixel2DShader->setUniform("windowDimens", glm::vec2(currentResolution.width, currentResolution.height));
//  pixel2DShader->setUniform("lowerLeftOffset", glm::vec2((currentResolution.width / 2) - 16.0, (currentResolution.height / 2) - 16.0));
//  pixel2DShader->setUniform("spriteDimens", glm::vec2(textureWidth, textureHeight));

  timer.lengthInSeconds = 5;

  lastFrame = getTime();
  startTime = lastFrame;
}

void MengerSpongeScene::deinit()
{
  FirstPersonScene::deinit();

  mengerSpongeShader->deleteShaderResources();
  pixel2DShader->deleteShaderResources();
  cubeShader->deleteShaderResources();
  delete mengerSpongeShader;
  delete pixel2DShader;
  delete cubeShader;

  VertexAtt deleteVertexAttributes[] = { quadVertexAtt, cubeVertexAtt };
  deleteVertexAtts(ArrayCount(deleteVertexAttributes), deleteVertexAttributes);

  deleteFramebuffer(&dynamicResolutionFBO);

  uint32 deleteTextures[] = { textureDiff1Id, textureSpec1Id, textureDiff2Id, textureSpec2Id };
  glDeleteTextures(ArrayCount(deleteTextures), deleteTextures);
}

Framebuffer MengerSpongeScene::drawFrame()
{
  glBindVertexArray(quadVertexAtt.arrayObject);

  glActiveTexture(GL_TEXTURE0 + diff1TextureIndex);
  glBindTexture(GL_TEXTURE_2D, textureDiff1Id);
  glActiveTexture(GL_TEXTURE0 + spec1TextureIndex);
  glBindTexture(GL_TEXTURE_2D, textureSpec1Id);
  glActiveTexture(GL_TEXTURE0 + diff2TextureIndex);
  glBindTexture(GL_TEXTURE_2D, textureDiff2Id);
  glActiveTexture(GL_TEXTURE0 + spec2TextureIndex);
  glBindTexture(GL_TEXTURE_2D, textureSpec2Id);

  // background clear color
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);

  // NOTE: the viewport dictates the area of the bound
  glViewport(0, 0, currentResolution.width, currentResolution.height);

  float32 t = getTime() - startTime;
  deltaTime = t - lastFrame;
  lastFrame = t;

  cubeShader->use();
  if(((uint32)(t / frameTime) % 2) == 0) {
    cubeShader->setUniform("material.diffTexture", 0);
    cubeShader->setUniform("material.specTexture", 1);
  } else {
    cubeShader->setUniform("material.diffTexture", 2);
    cubeShader->setUniform("material.specTexture", 3);
  }

  // Auto run forward
  //    glm::vec3 deltaCameraPos = camera.Front;
  //    deltaCameraPos *= 0.07;
  //    camera.Position += deltaCameraPos;
  glm::mat4 cameraMat = camera.UpdateViewMatrix(deltaTime, cameraMovementSpeed * 4.0f, false);

  // bind our frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, dynamicResolutionFBO.id);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(mengerSpongeShader->updateShadersWhenOutdated(FragmentShaderFlag, timer)) {
    mengerSpongeShader->use();
    mengerSpongeShader->setUniform("viewPortResolution", glm::vec2(currentResolution.width, currentResolution.height));
    mengerSpongeShader->setUniform("directionalLight.color.ambient", directionalLightAmb);
    mengerSpongeShader->setUniform("directionalLight.color.diffuse", directionalLightDiff);
    mengerSpongeShader->setUniform("directionalLight.color.specular", directionalLightSpec);
    mengerSpongeShader->setUniform("directionalLight.direction", directionalLightDir);
    mengerSpongeShader->setUniform("projection", projectionMat);
  }

  mengerSpongeShader->use();
  mengerSpongeShader->setUniform("rayOrigin", camera.Position);
  mengerSpongeShader->setUniform("elapsedTime", t);
  mengerSpongeShader->setUniform("view", cameraMat);
  mengerSpongeShader->setUniform("cameraPos", camera.Position);
  mengerSpongeShader->setUniform("numSamples", numSamples);
  glBindVertexArray(quadVertexAtt.arrayObject);
  glDrawElements(GL_TRIANGLES, // drawing mode
                 6, // number of elements to draw (3 vertices per triangle * 2 triangles per quad)
                 GL_UNSIGNED_INT, // type of the indices
                 0 /* offset in the EBO */);

  // NOTE: Cube will be positioned at <0,0>
  glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), t * glm::radians(20.0f), cubeRotAxis);
  glm::mat4 cubeModel = glm::scale(glm::mat4(1.0f), glm::vec3(cubeScale)); // scale is uniform
  cubeModel = cubeModel * rotateMatrix;

  glBindVertexArray(cubeVertexAtt.arrayObject);

  uint32 cubeAttrIndices[ArrayCount(cubeAttributeIndices)];
  glm::mat3 inverseCubeModelRotation = glm::transpose(glm::mat3(rotateMatrix));
  cubeAttributeIndicesBackToFront(inverseCubeModelRotation * camera.Position, cubeAttrIndices);

  // bind element buffer object to give indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVertexAtt.indexObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeAttrIndices), cubeAttrIndices, GL_DYNAMIC_DRAW);

  cubeShader->use();
  cubeShader->setUniform("model", cubeModel);
  cubeShader->setUniform("view", cameraMat);
  cubeShader->setUniform("cameraPos", camera.Position);
  glDrawElements(GL_TRIANGLES, // drawing mode
                 cubePosNormTexNumElements * 3, // number of elements to draw (3 vertices per triangle * 2 triangles per face * 6 faces)
                 GL_UNSIGNED_INT, // type of the indices
                 0); // offset in the EBO
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

//    pixel2DShader->use();
//    glDrawElements(GL_TRIANGLES, // drawing mode
//                   6, // number of elements to draw (3 vertices per triangle * 2 triangles per quad)
//                   GL_UNSIGNED_INT, // type of the indices
//                   0); // offset in the EBO

  return publicPseudoDrawFramebuffer;
}

void MengerSpongeScene::drawGui()
{
  FirstPersonScene::drawGui();

  if (showDebugWindows){
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (ImGui::SliderInt("Supersamples", &numSamples, 1, 8)) {}
    ImGui::PopItemWidth();
  }
}

void MengerSpongeScene::inputStatesUpdated() {
  FirstPersonScene::inputStatesUpdated();

  if(hotPress(KeyboardInput_E))
  {
    if(++currentResolutionIndex >= ArrayCount(screenResolutions)) {
      currentResolutionIndex = 0;
    }

    currentResolution = screenResolutions[currentResolutionIndex];

    publicPseudoDrawFramebuffer.extent.width = currentResolution.width;
    publicPseudoDrawFramebuffer.extent.height = currentResolution.height;

    mengerSpongeShader->use();
    mengerSpongeShader->setUniform("viewPortResolution", glm::vec2(currentResolution.width, currentResolution.height));

    pixel2DShader->use();
    pixel2DShader->setUniform("windowDimens", glm::vec2(currentResolution.width, currentResolution.height));
    pixel2DShader->setUniform("lowerLeftOffset", glm::vec2((currentResolution.width / 2) - 16.0, (currentResolution.height / 2) - 16.0));
  }

  if(hotPress(KeyboardInput_Q))
  {
    if(--currentResolutionIndex == -1) {
      currentResolutionIndex = ArrayCount(screenResolutions) - 1;
    }

    currentResolution = screenResolutions[currentResolutionIndex];

    publicPseudoDrawFramebuffer.extent.width = currentResolution.width;
    publicPseudoDrawFramebuffer.extent.height = currentResolution.height;

    mengerSpongeShader->use();
    mengerSpongeShader->setUniform("viewPortResolution", glm::vec2(currentResolution.width, currentResolution.height));

    pixel2DShader->use();
    pixel2DShader->setUniform("windowDimens", glm::vec2(currentResolution.width, currentResolution.height));
    pixel2DShader->setUniform("lowerLeftOffset", glm::vec2((currentResolution.width / 2) - 16.0, (currentResolution.height / 2) - 16.0));
  }

  if(hotPress(KeyboardInput_R))
  {
    showDebugWindows = !showDebugWindows;
    enableDefaultMouseCameraMovement(!showDebugWindows);
    enableCursor(window, showDebugWindows);
  }
}

