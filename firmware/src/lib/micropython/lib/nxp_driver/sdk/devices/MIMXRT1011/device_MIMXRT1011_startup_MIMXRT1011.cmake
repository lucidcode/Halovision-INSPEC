include_guard(GLOBAL)
message("device_MIMXRT1011_startup component is included.")

target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/gcc/startup_MIMXRT1011.S
)


include(device_MIMXRT1011_system_MIMXRT1011)

