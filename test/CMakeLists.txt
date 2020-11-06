cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()


# Project details
project(
  ${CMAKE_PROJECT_NAME}Tests
  LANGUAGES CXX
)

verbose_message("Adding tests under ${CMAKE_PROJECT_NAME}Tests...")

# Set the sources for the unit tests and add the executable(s)

set(test_sources 
  src/tmp_test.cpp
)
add_executable(${PROJECT_NAME} ${test_sources})

#
# Set the compiler standard

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)


# Setup code coverage if enabled
if (${CMAKE_PROJECT_NAME}_ENABLE_CODE_COVERAGE)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(${CMAKE_PROJECT_NAME} PUBLIC -fprofile-arcs -ftest-coverage)
  verbose_message("Code coverage is enabled and provided with GCC.")
endif()

#
# Load the desired unit testing framework

if(${CMAKE_PROJECT_NAME}_BUILD_EXECUTABLE)
	set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME})
else()
	set(${CMAKE_PROJECT_NAME}_TEST_LIB ${CMAKE_PROJECT_NAME})
endif()



if(${CMAKE_PROJECT_NAME}_USE_GTEST AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  #find_package(GTest REQUIRED)

  if(${CMAKE_PROJECT_NAME}_USE_GOOGLE_MOCK)
       set(GOOGLE_MOCK_LIBRARIES  CONAN_PKG::gtest::gmock  CONAN_PKG::gtest::gmock_main)
  endif()

  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
      CONAN_PKG::gtest
      ${GOOGLE_MOCK_LIBRARIES}
  #    ${${CMAKE_PROJECT_NAME}_TEST_LIB}
  )
elseif(${CMAKE_PROJECT_NAME}_USE_CATCH2)
  find_package(Catch2 REQUIRED)
  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
      Catch2::Catch2
 #     ${${CMAKE_PROJECT_NAME}_TEST_LIB}
  )
else()
  message( "Unknown testing library. Please setup your desired unit testing library by using `target_link_libraries`.")  
endif()

#
# Add the unit tests
add_test(
  NAME
    ${PROJECT_NAME}
  COMMAND
    ${PROJECT_NAME}
)


verbose_message("Finished adding unit tests for ${CMAKE_PROJECT_NAME}.")