 
message(STATUS "Configuring HO-SIRR-GUI...")

# Versions to build:
set(BUILD_PLUGIN_FORMATS "")
if (BUILD_PLUGIN_FORMAT_VST2)
    juce_set_vst2_sdk_path(${CMAKE_CURRENT_SOURCE_DIR}/../SDKs/VST2_SDK)
    list(APPEND BUILD_PLUGIN_FORMATS VST)
    message("--  Building VST versions")
endif()
if (BUILD_PLUGIN_FORMAT_VST3) 
    list(APPEND BUILD_PLUGIN_FORMATS VST3)
    message("--  Building VST3 versions")
endif()
if (BUILD_PLUGIN_FORMAT_AU) 
    list(APPEND BUILD_PLUGIN_FORMATS AU)
    message("--  Building AU versions")
endif()
if (BUILD_PLUGIN_FORMAT_LV2) 
    list(APPEND BUILD_PLUGIN_FORMATS LV2)
    message("--  Building LV2 versions")
endif()

add_subdirectory(_HOSIRR_) 
