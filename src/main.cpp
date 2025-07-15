#define GLM_ENABLE_EXPERIMENTAL
#include "Shader.h"
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

  Shader shader(SHADERS_PATH "shader.vs", SHADERS_PATH "shader.fs");
  Shader lightShader(SHADERS_PATH "light.vs", SHADERS_PATH "light.fs");

  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



  shader.use();
  //cube vao
  unsigned cubeVao, cubeVbo, cubeEbo;
  glGenVertexArrays(1, &cubeVao);
  glGenBuffers(1, &cubeVbo);

  glBindVertexArray(cubeVao);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // normals
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  //light vao
  unsigned lightVao;
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(lightVao);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glEnable(GL_DEPTH_TEST);
  SDL_SetRelativeMouseMode(SDL_TRUE);

  SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);

  glm::vec3 lightPos = glm::vec3(3, 3, 3);

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)
    };

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
        if (event.key.keysym.sym == SDLK_UP) {
          lightPos.y += 0.03f;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
          lightPos.y -= 0.03f;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          lightPos.x -= 0.03f;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          lightPos.x += 0.03f;
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

    shader.use();

    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    glm::mat4 projection = glm::perspective(
      glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f
    );

    shader.setVec3("viewPos", camera.Position);
    shader.setMat4("projection", projection);
    shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader.setVec3("lightPos", lightPos);

    glm::mat4 model = glm::mat4(1.0f);
    glBindVertexArray(cubeVao);
    for (unsigned i = 0; i < 10; i++) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glViewport(0, 0, w, h);

    SDL_GL_SwapWindow(window);
  }

  // Cleanup SDL
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
