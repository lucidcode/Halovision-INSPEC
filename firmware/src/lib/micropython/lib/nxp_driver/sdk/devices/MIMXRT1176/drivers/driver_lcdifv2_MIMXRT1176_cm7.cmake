include_guard(GLOBAL)
message("driver_lcdifv2 component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/fsl_lcdifv2.c
)


target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/.
)


include(driver_memory_MIMXRT1176_cm7)

include(driver_common_MIMXRT1176_cm7)

