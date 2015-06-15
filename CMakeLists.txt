cmake_minimum_required(VERSION 2.8.11)

project (qGo)

SET(CMAKE_MODULE_PATH /usr/local/lib/cmake/)

# Répertoire d'installation de Qt5 (dépend de l'installation)
SET(CMAKE_PREFIX_PATH "~/Qt/5.4/gcc/")

find_package (OpenCV REQUIRED)
find_package (aruco REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTORCC ON)
# Use moc files in the bin folder
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the Qt5 components
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Multimedia)

set(EXECUTABLE_OUTPUT_PATH bin)
SET(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(src)
include_directories(src/audio)
include_directories(src/board)
include_directories(src/game_interfaces)
include_directories(src/game_tree)
include_directories(src/gtp)
include_directories(src/network)
include_directories(src/resources)
include_directories(src/sgf)
include_directories(src/translations)

file(
        GLOB_RECURSE
        source_files
        src/*.cpp
)

file(
        GLOB_RECURSE
        ui_files
        src/*.ui
)

file(
        GLOB_RECURSE
        header_files
        src/*.h
        src/*.hpp
)

file(
        GLOB_RECURSE
        resources_files
        src/*qrc
)

QT5_WRAP_UI(header_ui ${ui_files})
QT5_ADD_RESOURCES(qt_resources ${resources_files})

# Tell CMake to create the helloworld executable
add_executable(qGo WIN32 ${source_files} ${header_ui} ${header_files} ${qt_resources})

# Use the modules from Qt 5.
target_link_libraries(qGo Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Multimedia ${OpenCV_LIBS} ${aruco_LIBS})

