#include "SceneManager.h"

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include "../TextDebugShader.h"
#include "../common/Input.h"

#include "NessCube/NessCubesScene.h"
#include "InfiniteCube/InfiniteCubeScene.h"
#include "ReflectRefract/ReflectRefractScene.h"
#include "AsteroidBelt/AsteroidBeltScene.h"
#include "Moon/MoonScene.h"
#include "Room/RoomScene.h"
#include "GUIScene/GUIScene.h"
#include "InfiniteCapsules/InfiniteCapsulesScene.h"
#include "Mandelbrot/MandelbrotScene.h"
#include "MengerSponge/MengerSpongeScene.h"
#include "RayTracingSphere/RayTracingSphereScene.h"
#include "Pixel2D/Pixel2DScene.h"

file_access bool sceneManagerIsActive = true;

void toggleWindowSize(GLFWwindow* window, const uint32 width, const uint32 height);

class EmptyScene : public Scene
{
  void init(uint32 windowWidth, uint32 windowHeight) {
    Scene::init(windowWidth, windowHeight);
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  }
  const char* title() { return "Empty Scene"; }
  void drawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }
};

void runScenes(GLFWwindow* window) {
  Extent2D windowExtent = { VIEWPORT_INIT_WIDTH, VIEWPORT_INIT_HEIGHT };

  TextDebugShader textDebugShader = TextDebugShader(windowExtent.x, windowExtent.y);

  EmptyScene emptyScene = EmptyScene();
  NessCubesScene nessCubeScene = NessCubesScene();
  InfiniteCapsulesScene infiniteCapsulesScene = InfiniteCapsulesScene();
  InfiniteCubeScene infiniteCubeScene = InfiniteCubeScene();
  AsteroidBeltScene asteroidBeltScene = AsteroidBeltScene();
  MandelbrotScene mandelbrotScene = MandelbrotScene(window);
  RayTracingSphereScene rayTracingSphereScene = RayTracingSphereScene();
  MengerSpongeScene mengerSpongeScene = MengerSpongeScene(window);
  MoonScene moonScene = MoonScene();
  RoomScene roomScene = RoomScene();
  ReflectRefractScene reflectRefractScene = ReflectRefractScene();
  GUIScene guiScene = GUIScene(window);
  Pixel2DScene pixel2DScene = Pixel2DScene();
  Scene* scenes[] = { &mengerSpongeScene, &rayTracingSphereScene, &mandelbrotScene, &infiniteCubeScene,
                      &infiniteCapsulesScene, &roomScene, &guiScene, &moonScene, &asteroidBeltScene,
                      &reflectRefractScene, &nessCubeScene, &pixel2DScene, &emptyScene };
  uint32 sceneIndex = 0;
  uint32 sceneCount = ArrayCount(scenes);
  bool sceneCursorMode = false;

  auto handleInputForFrame = [&scenes, &sceneIndex, sceneCount, &window, &windowExtent, &textDebugShader, &sceneCursorMode]()
  {
    if(hotPress(KeyboardInput_Tab)) {
      sceneManagerIsActive = !sceneManagerIsActive;
      if(sceneManagerIsActive) {
        sceneCursorMode = isCursorEnabled(window);
        enableCursor(window, true);
      } else { // scene manager deactivated
        enableCursor(window, sceneCursorMode);
      }
    }

    if(hotPress(KeyboardInput_Esc))
    {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if(isActive(KeyboardInput_Alt_Right) && hotPress(KeyboardInput_Enter))
    {
      toggleWindowSize(window, VIEWPORT_INIT_WIDTH, VIEWPORT_INIT_HEIGHT);
    }

    bool windowSizeChange = isActive(WindowInput_SizeChange);
    if(windowSizeChange)
    {
      windowExtent = getWindowExtent();
      textDebugShader.updateWindowDimens(windowExtent.x, windowExtent.y);
    }

    if(!sceneManagerIsActive || windowSizeChange) { // if scene manager isn't active or we have a window size change, pass input to scene
      scenes[sceneIndex]->inputStatesUpdated();
    }
  };

  initializeInput(window, windowExtent);
  scenes[sceneIndex]->init(windowExtent.x, windowExtent.y);
  sceneCursorMode = isCursorEnabled(window);
  enableCursor(window, true);
  float32 deltaTime = 1.0;
  float32 lastFrame = (float32)glfwGetTime();
  while (glfwWindowShouldClose(window) == GL_FALSE)
  {
    loadInputStateForFrame(window);
    handleInputForFrame();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    scenes[sceneIndex]->drawFrame();

    if(!sceneManagerIsActive) { // if scene manager isn't active, draw GUI for scene
      scenes[sceneIndex]->drawGui();
    } else {
      // debug text
      uint32 numFrames = (uint32)(1 / deltaTime);
      textDebugShader.renderText(std::to_string(numFrames) + " FPS", 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
      float32 t = (float32)glfwGetTime();
      deltaTime = t - lastFrame;
      lastFrame = t;

      // ImGui
      // NOTE: below is a GREAT resource for ImGui
      //ImGui::ShowDemoWindow(&showDebugWindows);
      if (ImGui::BeginMainMenuBar())
      {
        if (ImGui::BeginMenu("Scenes"))
        {
          for(uint32 i = 0; i < sceneCount; ++i)
          {
            if (ImGui::MenuItem(scenes[i]->title())) {
              sceneManagerIsActive = false;
              scenes[sceneIndex]->deinit();
              enableCursor(window, false);
              sceneIndex = i;
              scenes[sceneIndex]->init(windowExtent.x, windowExtent.y);
            }
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }
    }

    // Rendering ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window); // swaps double buffers (call after all render commands are completed)
    glfwPollEvents(); // checks for events (ex: keyboard/mouse input)
  }
  scenes[sceneIndex]->deinit();
  deinitializeInput(window);

  glfwTerminate(); // clean up gl resources
}

void toggleWindowSize(GLFWwindow* window, const uint32 width, const uint32 height)
{
  local_access bool windowMode = true;
  if (windowMode) {
    toFullScreenMode(window);
  } else{
    toWindowedMode(window, width, height);
  }
  windowMode = !windowMode;
}