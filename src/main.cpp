#define GLM_ENABLE_EXPERIMENTAL
#include "shader.h"
#include "camera.h"
#include "filereader.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <openglErrorReporting.h>
#include <stb_image/stb_image.h>
#include <sys/types.h>
#include <glm/gtx/string_cast.hpp>
#include "loadTexture.h"
#include "model.h"
#include "global.h"


#undef main

unsigned windowWidth = 680, windowHeight = 480;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char *argv[]) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef __APPLE__
  // apple moment...
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

  // Create a window
  SDL_Window *window = SDL_CreateWindow(
      "Hello SDL", 100, 100, windowWidth, windowHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  // Create OpenGL context
  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Load OpenGL functions using glad
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Enable OpenGL error reporting
  enableReportGlErrors();

  global::init();

  stbi_set_flip_vertically_on_load(false);

  glEnable(GL_DEPTH_TEST);

  Shader ourShader(SHADERS_PATH"modelLoading.vs", SHADERS_PATH"modelLoading.fs");

  Model ourModel(RESOURCES_PATH"models/lampPost/lamppostthingy.obj");

  SDL_SetRelativeMouseMode(SDL_TRUE);

  SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);

  bool moveForward = false;
  bool moveBackward = false;
  bool moveLeft = false;
  bool moveRight = false;
  bool running = true;
  while (running) {
    int w = 0, h = 0;
    SDL_GetWindowSize(window, &w, &h);

    uint32_t currentFrame = SDL_GetTicks();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    int xpos, ypos;
    SDL_GetRelativeMouseState(&xpos, &ypos);

    camera.ProcessMouseMovement(xpos, -ypos);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_MOUSEWHEEL) {
        camera.ProcessMouseScroll(event.wheel.y);
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == 'a') {
          moveLeft = true;
        }
        if (event.key.keysym.sym == 'd') {
          moveRight = true;
        }
        if (event.key.keysym.sym == 'w') {
          moveForward = true;
        }
        if (event.key.keysym.sym == 's') {
          moveBackward = true;
        }
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          event.type = SDL_QUIT;
        }
      }
      if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == 'a') {
          moveLeft = false;
        }
        if (event.key.keysym.sym == 'd') {
          moveRight = false;
        }
        if (event.key.keysym.sym == 'w') {
          moveForward = false;
        }
        if (event.key.keysym.sym == 's') {
          moveBackward = false;
        }
      }

      if (event.type == SDL_QUIT) {
        running = false;
      }

      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(window)) {
          running = false;
        }
      }
    }

    if (moveForward) {
      camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (moveBackward) {
      camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (moveLeft) {
      camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (moveRight) {
      camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
            ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.draw(ourShader);

    glViewport(0, 0, w, h);

    SDL_GL_SwapWindow(window);
  }

  // Cleanup SDL
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
