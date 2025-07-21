// Include standard headers
#include <cstdlib>
#include <iostream>
#include <print>

// #include <GL/glew.h>

#include <glad/gl.h>
//   Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
// #include <glm/glm.hpp>
// using namespace glm;

// NOLINTNEXTLINE(bugprone-exception-escape)
int main()
{
  // Initialize GLFW
  if (glfwInit() != GL_TRUE) {
    std::println(std::cerr, "Failed to initialize GLFW");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);// To make macOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  constexpr auto width = 800;
  constexpr auto height = 600;
  GLFWwindow *window = glfwCreateWindow(width, height, "Tutorial 01", nullptr, nullptr);
  if (window == nullptr) {
    std::println(std::cerr,
      "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the "
      "tutorials.");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  const int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::println("Failed to initialize OpenGL context");
    return -1;
  }

  // Successfully loaded OpenGL
  std::println("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  // Dark blue background
  const GLfloat red = 0.0F;
  const GLfloat green = 0.0F;
  const GLfloat blue = 4.0F;
  glClearColor(red, green, blue, 0.0F);

  // Main loop
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw nothing, see you in tutorial 2 !


    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
