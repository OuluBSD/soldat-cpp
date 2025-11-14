cmake_minimum_required(VERSION 3.14)
project(soldat)

option(ADD_ASSETS "Include soldat.smod and font file" True)
option(BUILD_CLIENT "Build client" True)
option(BUILD_SERVER "Build server" True)
option(BUILD_STEAM "Build steam version" False)
option(BUILD_GNS "Build GameNetworkingSockets" True)
option(CROSS_LINUX_64 "Cross compile to Linux 64bit" False)
option(CROSS_LINUX_32 "Cross compile to Linux 32bit" False)
option(CROSS_WINDOWS_64 "Cross compile to Windows x86_64" False)
option(CROSS_WINDOWS_32 "Cross compile to Windows i386" False)
option(CROSS_DARWIN_64 "Cross compile to macOS x86_64" False)
option(CROSS_DARWIN_32 "Cross compile to macOS i386" False)

option(ADD_BFT "Copy BFT test to scripts directory" False)
option(ADD_FFI_FUZZ "Enable ScriptCore FFI fuzz tests" False)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
include(${CMAKE_MODULE_PATH}/utils.cmake)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (default Debug)" FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# On Unices other than Mac, use GNUInstallDirs for an easier packaging process.
if (UNIX AND NOT APPLE)
  include(GNUInstallDirs)
  set(target_full_library_install_dir "${CMAKE_INSTALL_FULL_LIBDIR}")
  set(target_library_install_dir "${CMAKE_INSTALL_LIBDIR}")
  set(target_full_binary_install_dir "${CMAKE_INSTALL_FULL_BINDIR}")
  set(target_binary_install_dir "${CMAKE_INSTALL_BINDIR}")
  set(target_full_data_install_dir "${CMAKE_INSTALL_FULL_DATADIR}")
  set(target_data_install_dir "${CMAKE_INSTALL_DATADIR}")
else()
  set(target_full_library_install_dir "${CMAKE_INSTALL_PREFIX}")
  set(target_library_install_dir ".")
  set(target_full_binary_install_dir "${CMAKE_INSTALL_PREFIX}")
  set(target_binary_install_dir ".")
  set(target_full_data_install_dir "${CMAKE_INSTALL_PREFIX}")
  set(target_data_install_dir ".")
endif()

#set(CMAKE_VERBOSE_MAKEFILE on)

# RPATH settings. This needs to be set for C++ build
if(UNIX)
  if(APPLE)
    set(target_full_library_install_dir "${CMAKE_INSTALL_PREFIX}/../Frameworks/")
    set(target_library_install_dir "../Frameworks/")
    #set(CMAKE_INSTALL_PREFIX "Soldat.app/Contents/MacOS/")
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
    set(CMAKE_INSTALL_NAME_DIR "@executable_path/../Frameworks")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  else()
    set(CMAKE_INSTALL_RPATH
      "$ORIGIN/../${target_library_install_dir}/:$ORIGIN/:${target_full_library_install_dir}"
    )
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  endif()

  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_SKIP_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

# Enable C++ language
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C++ flags that will be shared between client and server.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG_BUILD")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Other dependencies shared between client and server.
# Find PhysFS
if(NOT PHYSFS_LIBRARY OR NOT PHYSFS_INCLUDE_DIR)
  find_package(PhysFS REQUIRED)
endif()

if(ADD_ASSETS)
  include(${CMAKE_MODULE_PATH}/SoldatAssets.cmake)
  download_assets()
endif()

# Set include directories for the project
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/shared/libs/PhysFS)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/Pascal-SDL-2-Headers)  # Still need for headers during transition

if(BUILD_CLIENT OR BUILD_SERVER)
  if(BUILD_STEAM)
    # Create steam_appid.txt
    file(WRITE ${PROJECT_BINARY_DIR}/bin/steam_appid.txt "638490")
  else()
    # GameNetworkingSockets
    if(BUILD_GNS)
      if(EXISTS "${CMAKE_SOURCE_DIR}/shared/libs/GameNetworkingSockets")
        add_subdirectory(shared/libs/GameNetworkingSockets)
      endif()
    endif()
  endif()
endif()

if(BUILD_CLIENT)
  # Find required libraries for client
  find_package(SDL2 REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(OpenAL)
  find_package(Freetype REQUIRED)
  
  # Define client executable
  file(GLOB CLIENT_SOURCES 
    "src/Client*.cpp"
    "src/client/*.cpp"
    "src/shared/*.cpp"
    "src/*.cpp"
  )
  
  add_executable(soldat_client ${CLIENT_SOURCES})
  target_include_directories(soldat_client PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
  )
  
  # Add OpenAL if available
  if(OPENAL_FOUND)
    target_include_directories(soldat_client PRIVATE ${OPENAL_INCLUDE_DIR})
    target_link_libraries(soldat_client 
      ${SDL2_LIBRARIES}
      ${OPENGL_LIBRARIES}
      ${OPENAL_LIBRARY}
      ${FREETYPE_LIBRARIES}
      physfs
    )
  else()
    target_link_libraries(soldat_client 
      ${SDL2_LIBRARIES}
      ${OPENGL_LIBRARIES}
      ${FREETYPE_LIBRARIES}
      physfs
    )
  endif()
  
  # Add subdirectory if it exists
  if(EXISTS "${CMAKE_SOURCE_DIR}/client/CMakeLists.txt")
    add_subdirectory(client)
  endif()
endif()

if(BUILD_SERVER)
  # Define server executable
  file(GLOB SERVER_SOURCES 
    "src/Server*.cpp"
    "src/server/*.cpp"
    "src/shared/*.cpp"
    "src/*.cpp"
  )
  
  add_executable(soldat_server ${SERVER_SOURCES})
  target_link_libraries(soldat_server physfs)
  
  # Add subdirectory if it exists
  if(EXISTS "${CMAKE_SOURCE_DIR}/server/CMakeLists.txt")
    add_subdirectory(server)
  endif()
endif()

# Install step that does 3 things:
# 1) Cleans binary directory by removing unnecessary files from compilation.
# 2) Resets file permissions (for example removes executable flag from shared
#    libraries on linux).
# 3) Copies cleaned directory to path specified by CMAKE_INSTALL_PREFIX.
#
# It's likely not the best approach, but should get the job done.
# CMake v3.16 has file(GET_RUNTIME_DEPENDENCIES), which would probably be
# more suitable for this. That approach comes with its own set of limitations though.
# For instance, .smod dependency will most likely not be detected. Also, we would
# probably need to filter out some dependencies manually, as we probably don't want
# to provide all .so files on linux - users can get those easily from package managers.
install(
  DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/
  DESTINATION ${target_library_install_dir}
  FILES_MATCHING
  PATTERN "*.sh" EXCLUDE
  PATTERN "*.bat" EXCLUDE
  PATTERN "*.res" EXCLUDE
  PATTERN "*.smod" EXCLUDE
  PATTERN "*.dll"
  PATTERN "*.so"
  PATTERN "*.dylib"
  # Ignore subdirectories, just in case... it's not necessary on clean build,
  # only if you ran client/server executables before installing.
  REGEX "configs|demos|logs|maps|mods|screens|scripts" EXCLUDE
)

if(ADD_ASSETS)
  install(
    DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/
    # FIXME: should be target_data_install_dir, but unsupported by soldat
    # (needs wrapper script or some equivalent global config)
    DESTINATION "${target_binary_install_dir}"
    FILES_MATCHING
    PATTERN "*.smod"
  )
endif()
