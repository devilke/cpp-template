include(cmake/CPM.cmake)


# https://stackoverflow.com/questions/37434946/how-do-i-iterate-over-all-cmake-targets-programmatically
macro(find_targets targets dir)
  get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
  foreach(subdir ${subdirectories})
    find_targets(${targets} ${subdir})
  endforeach()

  get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro()

function(make_folder folder)
  if (CMAKE_GENERATOR MATCHES "Visual Studio")
    foreach(project ${ARGN})
      set_property(TARGET ${project} PROPERTY FOLDER ${folder})
    endforeach(project)
  endif()
endfunction(make_folder)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(cpp_template_setup_dependencies)

  Set(FETCHCONTENT_QUIET FALSE)
  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.15.2
      GITHUB_REPOSITORY
      "gabime/spdlog")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.8.1")
  endif()

  if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.5.0")
  endif()

  if(NOT TARGET ftxui::screen)
    cpmaddpackage("gh:ArthurSonzogni/FTXUI@6.0.2")
  endif()

  CPMAddPackage(
    NAME glfw
    VERSION 3.3.10
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.10
    OPTIONS "BUILD_SHARED_LIBS ON"
            "GLFW_BUILD_EXAMPLES OFF"
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_DOCS OFF"
  )
  find_targets(glfw_targets ${glfw_SOURCE_DIR})
  make_folder("glfw" ${glfw_targets})

  CPMAddPackage("gh:g-truc/glm#master")

  CPMAddPackage("gh:Dav1dde/glad@2.0.8")
  if(glad_ADDED)
    add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR})
    glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6 LANGUAGE C++)
    add_library(glad::glad ALIAS glad_gl_core_46)
    make_folder("glad" glad_gl_core_46)
  endif()

endfunction()
