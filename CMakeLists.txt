cmake_minimum_required(VERSION 3.14)
project(COMP4300_Assignment3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(SOURCE_FILES src/main.cpp
        src/GameEngine/GameEngine.cpp
        src/GameEngine/GameEngine.h
        src/Physics/Physics.cpp
        src/Physics/Physics.h
        src/Scenes/Scene/Scene.cpp
        src/Scenes/Scene/Scene.h
        src/Scenes/SMenu/SMenu.cpp
        src/Scenes/SMenu/SMenu.h
        src/Scenes/SPlay/SPlay.cpp
        src/Scenes/SPlay/SPlay.h
        src/Components/Components.h
        src/Vec2/Vec2.cpp
        src/Vec2/Vec2.h
        src/Assets/Assets.cpp
        src/Assets/Assets.h
        src/Animation/Animation.cpp
        src/Animation/Animation.h
        src/Action/Action.cpp
        src/Action/Action.h
        src/Entities/Entity/Entity.h
        src/Entities/EntityManager/EntityManager.cpp
        src/Entities/EntityManager/EntityManager.h
        src/Entities/Entity/Entity.cpp)
add_executable(COMP4300_Assignment3 ${SOURCE_FILES})

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(COMP4300_Assignment3 sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

file(COPY resources DESTINATION .)