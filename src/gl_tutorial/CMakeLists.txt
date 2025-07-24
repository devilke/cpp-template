add_executable(gl-tutorial 
  main.cpp
  shader.cpp
  shader.hpp
  controls.cpp
  controls.hpp
  SimpleFragmentShader.fragmentshader
  SimpleVertexShader.vertexshader)

# Install shader files to the binary directory
install(FILES 
  SimpleFragmentShader.fragmentshader
  SimpleVertexShader.vertexshader
  DESTINATION bin/src/gl_tutorial)

# Copy shader files to build directory for development
configure_file(SimpleFragmentShader.fragmentshader ${CMAKE_BINARY_DIR}/src/gl_tutorial/SimpleFragmentShader.fragmentshader COPYONLY)
configure_file(SimpleVertexShader.vertexshader ${CMAKE_BINARY_DIR}/src/gl_tutorial/SimpleVertexShader.vertexshader COPYONLY)

target_link_libraries(
  gl-tutorial
  PRIVATE cpp_template::cpp_template_options
          cpp_template::cpp_template_warnings
          )

target_link_system_libraries(
  gl-tutorial
  PRIVATE
          glad::glad
          glfw
          glm::glm
          )

target_include_directories(gl-tutorial PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
