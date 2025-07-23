// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/ext/matrix_clip_space.hpp>// glm::perspective
#include <glm/ext/matrix_float4x4.hpp>// glm::mat4
#include <glm/ext/matrix_transform.hpp>// glm::translate, glm::rotate, glm::scale
#include <glm/ext/vector_float3.hpp>// glm::vec3
#include <glm/geometric.hpp>// glm::cross
#include <glm/trigonometric.hpp>// glm::radians

using namespace glm;

#include "controls.hpp"

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
glm::mat4 viewMatrix;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
glm::mat4 projectionMatrix;
}// namespace

glm::mat4 getViewMatrix() { return viewMatrix; }
glm::mat4 getProjectionMatrix() { return projectionMatrix; }

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

void computeMatricesFromInputs(GLFWwindow *window)
{

  // Initial position : on +Z
  static glm::vec3 position{ 0, 0, 5 };
  // Initial horizontal angle : toward -Z
  static float horizontalAngle = 3.14F;
  // Initial vertical angle : none
  static float verticalAngle = 0.0F;
  // Initial Field of View
  const float initialFoV = 45.0F;

  const float speed = 3.0F;// 3 units / second
  const float mouseSpeed = 0.005F;

  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  const double currentTime = glfwGetTime();
  const auto deltaTime = static_cast<float>(currentTime - lastTime);

  // Get mouse position
  double xpos = 0;
  double ypos = 0;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position for next frame
  glfwSetCursorPos(window, 1024.0 / 2, 768.0 / 2);

  // Compute new orientation
  horizontalAngle += mouseSpeed * static_cast<float>((1024.0 / 2.0) - xpos);
  verticalAngle += mouseSpeed * static_cast<float>((768.0 / 2.0) - ypos);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  const glm::vec3 direction(
    cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));

  // Right vector
  const glm::vec3 rightVector =
    glm::vec3(sin(horizontalAngle - (3.14F / 2.0F)), 0, cos(horizontalAngle - (3.14F / 2.0F)));

  const glm::vec3 upVector = glm::cross(rightVector, direction);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { position += direction * deltaTime * speed; }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { position -= direction * deltaTime * speed; }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { position += rightVector * deltaTime * speed; }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { position -= rightVector * deltaTime * speed; }

  const float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's
                               // a bit too complicated for this beginner's tutorial, so it's disabled instead.

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  projectionMatrix = glm::perspective(glm::radians(FoV), 4.0F / 3.0F, 0.1F, 100.0F);
  // Camera matrix
  viewMatrix = glm::lookAt(position,// Camera is here
    position + direction,// and looks here : at the same position, plus "direction"
    upVector// Head is up (set to 0,-1,0 to look upside-down)
  );

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(readability-magic-numbers)