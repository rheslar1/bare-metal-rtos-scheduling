cmake_minimum_required(VERSION 3.16)
project(bare_metal_rtos_scheduling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

find_package(Threads REQUIRED)

add_library(scheduling_core
  src/Scheduler.cpp
)
target_include_directories(scheduling_core PUBLIC include)
target_link_libraries(scheduling_core PUBLIC Threads::Threads)
target_compile_options(scheduling_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE scheduling_core)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

add_executable(scheduler_tests tests/SchedulerTests.cpp)
target_link_libraries(scheduler_tests PRIVATE scheduling_core)
target_compile_options(scheduler_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME scheduler_tests COMMAND scheduler_tests)
