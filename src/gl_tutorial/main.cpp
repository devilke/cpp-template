#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

// Include glad before GLFW to avoid conflicts
#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>// glm::perspective
#include <glm/ext/matrix_float4x4.hpp>// glm::mat4
#include <glm/ext/matrix_transform.hpp>// glm::translate, glm::rotate, glm::scale
#include <glm/trigonometric.hpp>// glm::radians

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{
  // Create the shaders
  const GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  const GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if (VertexShaderStream.is_open()) {
    std::stringstream sstr{};
    sstr << VertexShaderStream.rdbuf();
    VertexShaderCode = sstr.str();
    VertexShaderStream.close();
  } else {
    std::println(
      "Impossible to open {}. Are you in the right directory ? Don't forget to read the FAQ !", vertex_file_path);
    return 0;
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if (FragmentShaderStream.is_open()) {
    std::stringstream sstr{};
    sstr << FragmentShaderStream.rdbuf();
    FragmentShaderCode = sstr.str();
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength = 0;

  // Compile Vertex Shader
  std::println("Compiling shader : {}", vertex_file_path);
  char const *VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(static_cast<size_t>(InfoLogLength));
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, VertexShaderErrorMessage.data());
    std::println("{}", VertexShaderErrorMessage.data());
  }

  // Compile Fragment Shader
  std::println("Compiling shader : {}", fragment_file_path);
  char const *FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(static_cast<size_t>(InfoLogLength));
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, FragmentShaderErrorMessage.data());
    std::println("{}", FragmentShaderErrorMessage.data());
  }

  // Link the program
  std::println("Linking program");
  const GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(static_cast<size_t>(InfoLogLength));
    glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, ProgramErrorMessage.data());
    std::println("{}", ProgramErrorMessage.data());
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

GLFWwindow *initializeOpenGL()
{
  // Initialize GLFW
  if (glfwInit() != GL_TRUE) {
    std::println(std::cerr, "Failed to initialize GLFW");
    return nullptr;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);// To make macOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// We don't want the old OpenGL

  // Open a window and create its OpenGL context
  constexpr auto width = 800;
  constexpr auto height = 600;
  GLFWwindow *window = glfwCreateWindow(width, height, "Tutorial 02", nullptr, nullptr);
  if (window == nullptr) {
    std::println(std::cerr,
      "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the "
      "tutorials.");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  const int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::println("Failed to initialize OpenGL context");
    return nullptr;
  }

  // Successfully loaded OpenGL
  std::println("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  return window;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main()
{
  GLFWwindow *window = initializeOpenGL();
  if (window == nullptr) { return -1; }

  // Dark blue background
  const GLfloat red = 0.0F;
  const GLfloat green = 0.0F;
  const GLfloat blue = 0.4F;
  const GLfloat alpha = 0.0F;
  glClearColor(red, green, blue, alpha);

  GLuint VertexArrayID = 0;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
  // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
  // clang-format off
  const auto g_vertex_buffer_data = std::to_array<GLfloat>({
    -1.0F,-1.0F,-1.0F, // triangle 1 : begin
    -1.0F,-1.0F, 1.0F,
    -1.0F, 1.0F, 1.0F, // triangle 1 : end
    1.0F, 1.0F,-1.0F, // triangle 2 : begin
    -1.0F,-1.0F,-1.0F,
    -1.0F, 1.0F,-1.0F, // triangle 2 : end
    1.0F,-1.0F, 1.0F,
    -1.0F,-1.0F,-1.0F,
    1.0F,-1.0F,-1.0F,
    1.0F, 1.0F,-1.0F,
    1.0F,-1.0F,-1.0F,
    -1.0F,-1.0F,-1.0F,
    -1.0F,-1.0F,-1.0F,
    -1.0F, 1.0F, 1.0F,
    -1.0F, 1.0F,-1.0F,
    1.0F,-1.0F, 1.0F,
    -1.0F,-1.0F, 1.0F,
    -1.0F,-1.0F,-1.0F,
    -1.0F, 1.0F, 1.0F,
    -1.0F,-1.0F, 1.0F,
    1.0F,-1.0F, 1.0F,
    1.0F, 1.0F, 1.0F,
    1.0F,-1.0F,-1.0F,
    1.0F, 1.0F,-1.0F,
    1.0F,-1.0F,-1.0F,
    1.0F, 1.0F, 1.0F,
    1.0F,-1.0F, 1.0F,
    1.0F, 1.0F, 1.0F,
    1.0F, 1.0F,-1.0F,
    -1.0F, 1.0F,-1.0F,
    1.0F, 1.0F, 1.0F,
    -1.0F, 1.0F,-1.0F,
    -1.0F, 1.0F, 1.0F,
    1.0F, 1.0F, 1.0F,
    -1.0F, 1.0F, 1.0F,
    1.0F,-1.0F, 1.0F
  });

  // One color for each vertex. They were generated randomly.
  // NOLINTBEGIN(readability-magic-numbers)
  const auto g_color_buffer_data = std::to_array<GLfloat> ({
    0.583F,  0.771F,  0.014F,
    0.609F,  0.115F,  0.436F,
    0.327F,  0.483F,  0.844F,
    0.822F,  0.569F,  0.201F,
    0.435F,  0.602F,  0.223F, // NOLINT(modernize-use-std-numbers)
    0.310F,  0.747F,  0.185F,
    0.597F,  0.770F,  0.761F,
    0.559F,  0.436F,  0.730F,
    0.359F,  0.583F,  0.152F,
    0.483F,  0.596F,  0.789F,
    0.559F,  0.861F,  0.639F,
    0.195F,  0.548F,  0.859F,
    0.014F,  0.184F,  0.576F,
    0.771F,  0.328F,  0.970F,
    0.406F,  0.615F,  0.116F,
    0.676F,  0.977F,  0.133F,
    0.971F,  0.572F,  0.833F,
    0.140F,  0.616F,  0.489F,
    0.997F,  0.513F,  0.064F,
    0.945F,  0.719F,  0.592F,
    0.543F,  0.021F,  0.978F,
    0.279F,  0.317F,  0.505F,
    0.167F,  0.620F,  0.077F,
    0.347F,  0.857F,  0.137F,
    0.055F,  0.953F,  0.042F,
    0.714F,  0.505F,  0.345F,
    0.783F,  0.290F,  0.734F,
    0.722F,  0.645F,  0.174F,
    0.302F,  0.455F,  0.848F,
    0.225F,  0.587F,  0.040F,
    0.517F,  0.713F,  0.338F,
    0.053F,  0.959F,  0.120F,
    0.393F,  0.621F,  0.362F,
    0.673F,  0.211F,  0.457F,
    0.820F,  0.883F,  0.371F,
    0.982F,  0.099F,  0.879F
  });
  // NOLINTEND(readability-magic-numbers)
  // clang-format on

  GLuint vertexbuffer = 0;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data.data(), GL_STATIC_DRAW);

  GLuint colorbuffer = 0;
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data.data(), GL_STATIC_DRAW);

  const GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

  // Get a handle for our "MVP" uniform
  const GLint MatrixID = glGetUniformLocation(programID, "MVP");

  // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  const glm::mat4 Projection = glm::perspective(glm::radians(45.0F), 4.0F / 3.0F, 0.1F, 100.0F);
  // Or, for an ortho camera :
  // glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

  // Camera matrix
  const glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3),// Camera is at (4,3,3), in World Space
    glm::vec3(0, 0, 0),// and looks at the origin
    glm::vec3(0, 1, 0)// Head is up (set to 0,-1,0 to look upside-down)
  );
  // Model matrix : an identity matrix (model will be at the origin)
  const glm::mat4 Model = glm::mat4(1.0F);
  // Our ModelViewProjection : multiplication of our 3 matrices
  const glm::mat4 MVP = Projection * View * Model;// Remember, matrix multiplication is the other way around

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);


  // Main loop
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
    // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
    glClear(static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT) | static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT));

    glUseProgram(programID);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,// attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,// size
      GL_FLOAT,// type
      GL_FALSE,// normalized?
      0,// stride
      nullptr// array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1,// attribute 1. No particular reason for 1, but must match the layout in the shader.
      3,// size
      GL_FLOAT,// type
      GL_FALSE,// normalized?
      0,// stride
      nullptr// array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(g_vertex_buffer_data.size()));

    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
