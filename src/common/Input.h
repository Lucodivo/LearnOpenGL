#pragma once

#define GLFW_INCLUDE_NONE // ensure GLFW doesn't load OpenGL headers

#include <GLFW/glfw3.h>

#include "../LearnOpenGLPlatform.h"

// TODO: Do I need multiple types of input consumers or should I merge into one InputConsumer?
class KeyboardConsumer
{
public:
  virtual void key_LeftShift_pressed() {};
  virtual void key_LeftShift_released() {};
  virtual void key_Esc(){};
  virtual void key_W() {};
  virtual void key_S() {};
  virtual void key_A() {};
  virtual void key_D() {};
  virtual void key_Q_pressed() {};
  virtual void key_Q_released() {};
  virtual void key_E_pressed() {};
  virtual void key_E_released() {};
  virtual void key_O_pressed() {};
  virtual void key_O_released() {};
  virtual void key_P_pressed() {};
  virtual void key_P_released() {};
  virtual void key_Space() {};
  virtual void key_Up() {};
  virtual void key_Down() {};
  virtual void key_Left() {};
  virtual void key_Right() {};
  virtual void key_AltEnter_pressed() {};
  virtual void key_AltEnter_released() {};
  virtual void key_Tab_pressed() {};
  virtual void key_Tab_released() {};
};

class MouseConsumer
{
public:
  virtual void mouseMovement(float32 xOffset, float32 yOffset) {};
  virtual void mouseScroll(float32 yOffset) {};
  virtual void key_LeftMouseButton_pressed(float32 xPos, float32 yPos) {};
  virtual void key_LeftMouseButton_released(float32 xPos, float32 yPos) {};
  virtual void key_RightMouseButton_pressed(float32 xPos, float32 yPos) {};
  virtual void key_RightMouseButton_released(float32 xPos, float32 yPos) {};
};

class ControllerConsumer
{
public:
  virtual void leftAnalog(int16 stickX, int16 stickY) {};
  virtual void rightAnalog(int16 stickX, int16 stickY) {};
  virtual void button_A_pressed() {};
  virtual void button_A_released() {};
  virtual void button_B_pressed() {};
  virtual void button_B_released() {};
  virtual void button_X_pressed() {};
  virtual void button_X_released() {};
  virtual void button_Y_pressed() {};
  virtual void button_Y_released() {};
  virtual void button_dPadUp_pressed() {};
  virtual void button_dPadDown_pressed() {};
  virtual void button_dPadLeft_pressed() {};
  virtual void button_dPadRight_pressed() {};
  virtual void button_leftShoulder_pressed() {};
  virtual void button_rightShoulder_pressed() {};
  virtual void button_start_pressed() {};
  virtual void button_select_pressed() {};
  virtual void button_select_released() {};
};

class WindowSizeConsumer
{
public:
  virtual void windowSizeChanged(uint32 width, uint32 height) {};
};

void initializeInput(GLFWwindow* window);
void processInput(GLFWwindow* window);
void processKeyboardInput(GLFWwindow* window);
void processMouseInput(GLFWwindow* window);
void processXInput();
void subscribeKeyboardInput(KeyboardConsumer* consumer);
void subscribeXInput(ControllerConsumer* consumer);
void subscribeMouseInput(MouseConsumer* consumer);
void subscribeWindowSize(WindowSizeConsumer* consumer);
bool unsubscribeKeyboardInput(KeyboardConsumer* consumer);
bool unsubscribeXInput(ControllerConsumer* consumer);
bool unsubscribeMouseInput(MouseConsumer* consumer);
void unsubscribeWindowSize();
void mouse_movement_callback(GLFWwindow* window, float64 xPos, float64 yPos);
void mouse_scroll_callback(GLFWwindow* window, float64 xOffset, float64 yOffset);
void window_size_callback(GLFWwindow* window, int32 width, int32 height);