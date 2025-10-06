cmake_minimum_required(VERSION 3.8)
project(Euro_Counter VERSION 0.1 LANGUAGES CXX )

find_package(OpenCV 4 REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_library(Manzini src/Manzini.cpp)
target_link_libraries(Manzini ${OpenCV_LIBRARIES})

add_library(Jin src/Jin.cpp)
target_link_libraries(Jin Manzini ${OpenCV_LIBRARIES})

add_library(Guidolin src/Guidolin.cpp)
target_link_libraries(Guidolin Jin Manzini ${OpenCV_LIBRARIES})

add_executable(${PROJECT_NAME} src/Euro_Counter.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Manzini Jin Guidolin)
