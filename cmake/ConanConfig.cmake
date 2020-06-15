if(${PROJECT_NAME}_ENABLE_CONAN)
  
  # Setup Conan requires and options here:
  set(${PROJECT_NAME}_CONAN_REQUIRES "")
  set(${PROJECT_NAME}_CONAN_OPTIONS "")

  #
  # If `conan.cmake` (from https://github.com/conan-io/cmake-conan) does not exist, download it.
  #
  #if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  # message(
  #    STATUS
  #      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan..."
  ##  )
   # file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
   #   "${CMAKE_BINARY_DIR}/conan.cmake"
   # )
   # message(STATUS "Cmake-Conan downloaded succesfully.")
  #endif()

  include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)

  conan_add_remote(NAME bincrafters 
      URL
          https://api.bintray.com/conan/bincrafters/public-conan
  )

  conan_cmake_run(
    REQUIRES
      ${CONAN_REQUIRES}
	CONANFILE
	  conanfile.txt
    OPTIONS
      ${CONAN_OPTIONS}
    BASIC_SETUP
    BUILD
      missing
  )
  
  if (${CMAKE_${LANGUAGE}_COMPILER_ID} STREQUAL GNU)
	 execute_process(COMMAND ls -la /Users/runner/.conan/data/glew/2.1.0/bincrafters/stable/package/47f2cde9a3b0b3c58d5cfbd072deff334035728e/include)
  endif()

  verbose_message("Conan is setup and all requires have been installed.")
endif()
