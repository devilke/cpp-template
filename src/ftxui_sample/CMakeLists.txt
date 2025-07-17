add_executable(intro main.cpp)

target_link_libraries(
  intro
  PRIVATE cpp_template::cpp_template_options
          cpp_template::cpp_template_warnings)

target_link_system_libraries(
  intro
  PRIVATE
          CLI11::CLI11
          spdlog::spdlog
          ftxui::screen
          ftxui::dom
          ftxui::component)

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
