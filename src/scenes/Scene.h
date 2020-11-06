#pragma once

#include <map>

#include "../LearnOpenGLPlatform.h"
#include "../common/Input.h"
#include "../Shader.h"

struct Character
{
  GLuint textureID;  // ID handle of the glyph texture
  glm::ivec2 size;       // Size of glyph
  glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
  GLuint advance;    // Offset to advance to next glyph
};

class Scene : public KeyboardConsumer
{
public:
  Scene(GLFWwindow* window);
  virtual void framebufferSizeChange(uint32 width, uint32 height);
  virtual void init(uint32 windowWidth, uint32 windowHeight);
  virtual void deinit(){} // de-initializes scene
  virtual void drawFrame(){} // draws scene to back buffer
  virtual void runScene() = 0;
  void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

protected:
  // TODO: Remove GLFWwindow from Scene?
  GLFWwindow* window;
  uint32 windowWidth = 0;
  uint32 windowHeight = 0;

private:
  std::map<GLchar, Character> Characters;
  Shader textDebugShader;
  uint32 textDebugVAO;
  uint32 textDebugVBO;
  glm::mat4 textDebugProjectionMat = glm::mat4( 1.0 );

  void initDebugTextCharacters();
  void initDebugTextBuffers();
  void updateTextDebugProjectionMat();
};