cmake_minimum_required(VERSION 3.0)
project(VidInjector9002.5)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
link_libraries(${wxWidgets_LIBRARIES})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

file(GLOB MY_SRC
	"src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)


add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MY_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Application")
