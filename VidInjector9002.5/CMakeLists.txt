cmake_minimum_required(VERSION 3.0)
project(VidInjector9002.5)

set(wxWidgets_ROOT_DIR ./wxWidgets-3.2.6)
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

IF(UNIX)
    find_package(X11)
    IF(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} X11)
    ENDIF()
ENDIF()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${MY_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Application")
