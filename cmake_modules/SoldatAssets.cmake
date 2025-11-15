# Downloads required assets during configure step (i.e. when
# calling cmake). We add custom targets, so that client and server
# can set them as their dependencies, which enables automatic
# copying of asset files during build step (i.e. when calling make).
macro(download_assets)
  include(FetchContent)

  set(BASE_REPOSITORY_URL "https://github.com/opensoldat/base"
      CACHE STRING "We pull soldat.smod and font from here")
  set(BASE_GIT_TAG "v0.2"
      CACHE STRING "Git tag associated with release in base repository")
  set(SOLDAT_SMOD_SHA1 "bf87492d10563319839cec7dc414976deffeba25"
      CACHE STRING "Expected SHA1 of soldat.smod download")
  set(DOWNLOAD_URL ${BASE_REPOSITORY_URL}/releases/download/${BASE_GIT_TAG})

  # Check if persistent assets directory was provided
  if(PERSISTENT_ASSETS_DIR AND EXISTS "${CMAKE_SOURCE_DIR}/${PERSISTENT_ASSETS_DIR}/soldat.smod" AND EXISTS "${CMAKE_SOURCE_DIR}/${PERSISTENT_ASSETS_DIR}/play-regular.ttf")
    # Use persistent assets - convert to absolute path
    set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/${PERSISTENT_ASSETS_DIR}")
    message(STATUS "Using persistent assets from ${ASSETS_DIR}")
  else()
    # Use temporary downloads directory
    set(ASSETS_DIR downloads)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_DIR}")
      file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_DIR}")
    endif()
    
    message(STATUS "Soldat assets will be downloaded from ${DOWNLOAD_URL}")
    file(DOWNLOAD
      ${DOWNLOAD_URL}/soldat.smod
      ${ASSETS_DIR}/soldat.smod
      EXPECTED_HASH SHA1=${SOLDAT_SMOD_SHA1}
    )

    file(DOWNLOAD
      ${DOWNLOAD_URL}/play-regular.ttf
      ${ASSETS_DIR}/play-regular.ttf
    )
  endif()

  # Configure targets, so that we can set dependencies
  # on client and server. This will make sure that
  # asset files get copied during build step (make).
  add_custom_target(
    soldat_smod
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/soldat.smod
                                                  ${EXECUTABLE_OUTPUT_PATH}/soldat.smod
    COMMENT "Copying soldat.smod to ${EXECUTABLE_OUTPUT_PATH}"
  )
  add_custom_target(
    soldat_font
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSETS_DIR}/play-regular.ttf
                                                  ${EXECUTABLE_OUTPUT_PATH}/play-regular.ttf
    COMMENT "Copying font to ${EXECUTABLE_OUTPUT_PATH}"
  )
endmacro()
