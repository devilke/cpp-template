add_executable(gl-tutorial main.cpp)

target_link_libraries(
  gl-tutorial
  PRIVATE cpp_template::cpp_template_options
          cpp_template::cpp_template_warnings
          glad::glad
          )

target_link_system_libraries(
  gl-tutorial
  PRIVATE
          glfw
          glm::glm
          )

target_include_directories(gl-tutorial PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
