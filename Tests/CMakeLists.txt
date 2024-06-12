include(CTest)

enable_testing()

project(GBEmuTests)

add_executable(
  ${PROJECT_NAME}
  ../src/GameBoy.cpp
  ../src/Opcodes.cpp
  ../src/CBopcodes.cpp
  generated_tests.cpp
)
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  GTest::gtest_main
  nlohmann_json::nlohmann_json
  ImGui-SFML::ImGui-SFML
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ../include
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
