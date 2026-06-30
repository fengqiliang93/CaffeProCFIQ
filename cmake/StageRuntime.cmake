function(cfiq_stage_copy_if_exists target_name source_path destination_dir)
    if(EXISTS "${source_path}")
        add_custom_command(TARGET "${target_name}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${source_path}" "${destination_dir}/"
        )
    endif()
endfunction()

function(cfiq_configure_stage_runtime)
    if(NOT CFIQ_STAGE_RUNTIME)
        return()
    endif()

    set(CFIQ_STAGE_ALIAS_COMMANDS)

    macro(cfiq_stage_alias source_path alias_name)
        if(EXISTS "${source_path}")
            list(APPEND CFIQ_STAGE_ALIAS_COMMANDS
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${source_path}" "${CFIQ_STAGE_DIR}/lib/${alias_name}"
            )
        endif()
    endmacro()

    cfiq_stage_alias("${CFIQ_PROTOBUF_LIB}" "libprotobuf.so.26")
    cfiq_stage_alias("${CFIQ_PROTOBUF_LIB}" "libprotobuf.so")
    cfiq_stage_alias("${CFIQ_GFORTRAN_LIB}" "libgfortran.so.5")
    cfiq_stage_alias("${CFIQ_GFORTRAN_LIB}" "libgfortran.so")

    if(CFIQ_TARGET_ARCH STREQUAL "aarch64")
        set(CFIQ_RELEASE_README "${CMAKE_CURRENT_SOURCE_DIR}/docs/release-package-aarch64.md")
        cfiq_stage_alias("${CFIQ_OPENCV_CORE_LIB}" "libopencv_core.so.406")
        cfiq_stage_alias("${CFIQ_OPENCV_IMGCODECS_LIB}" "libopencv_imgcodecs.so.406")
        cfiq_stage_alias("${CFIQ_OPENCV_IMGPROC_LIB}" "libopencv_imgproc.so.406")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libopenblasp-r0.3.26.so" "libopenblas.so.0")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libopenblasp-r0.3.26.so" "libopenblas.so")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libopenblasp-r0.3.26.so" "libblas.so.3")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libopenblasp-r0.3.26.so" "liblapack.so.3")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libgfortran.so.5.0.0" "libgfortran.so.5")
        cfiq_stage_alias("${CFIQ_THIRDPART_LIB_DIR}/libgfortran.so.5.0.0" "libgfortran.so")
    else()
        set(CFIQ_RELEASE_README "${CMAKE_CURRENT_SOURCE_DIR}/docs/release-package-x86_64.md")
    endif()

    add_custom_target(stage_runtime
        COMMAND "${CMAKE_COMMAND}" -E rm -rf "${CFIQ_STAGE_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CFIQ_STAGE_DIR}/include" "${CFIQ_STAGE_DIR}/lib" "${CFIQ_STAGE_DIR}/test"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CFIQ_PUBLIC_INCLUDE_ROOT}/CFIQ.h"
                "${CFIQ_STAGE_DIR}/include/CFIQ.h"
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:QualityCtrl>" "${CFIQ_STAGE_DIR}/lib/libQualityCtrl.so"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CFIQ_NIRVANA_KERNEL_LIB}" "${CFIQ_STAGE_DIR}/lib/libnirvana-kernel.so.8.15.1"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CFIQ_NIRVANA_KERNEL_LIB}" "${CFIQ_STAGE_DIR}/lib/libnirvana-kernel.so.8"
        COMMAND "${CMAKE_COMMAND}"
                -DLEFT="${CFIQ_STAGE_DIR}/lib/libnirvana-kernel.so.8.15.1"
                -DRIGHT="${CFIQ_STAGE_DIR}/lib/libnirvana-kernel.so.8"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifySameSha256.cmake"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/build/FQNet_model"
                "${CFIQ_STAGE_DIR}/test/FQNet_model"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/build/SourceBmpLinuxTest36"
                "${CFIQ_STAGE_DIR}/test/SourceBmpLinuxTest36"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/test_cfiq/build/HeatMap.bin"
                "${CFIQ_STAGE_DIR}/test/HeatMap.bin"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CFIQ_RELEASE_README}"
                "${CFIQ_STAGE_DIR}/README.md"
        ${CFIQ_STAGE_ALIAS_COMMANDS}
        DEPENDS QualityCtrl
        COMMENT "Staging ${CFIQ_TARGET_ARCH} runtime package at ${CFIQ_STAGE_DIR}"
    )

    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_OPENCV_CORE_LIB}" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_OPENCV_IMGCODECS_LIB}" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_OPENCV_IMGPROC_LIB}" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_PROTOBUF_LIB}" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_GFORTRAN_LIB}" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_THIRDPART_LIB_DIR}/libopenblasp-r0.3.26.so" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_THIRDPART_LIB_DIR}/libopenblas.so.0" "${CFIQ_STAGE_DIR}/lib")
    cfiq_stage_copy_if_exists(stage_runtime "${CFIQ_THIRDPART_LIB_DIR}/libgfortran.so.5.0.0" "${CFIQ_STAGE_DIR}/lib")

    if(CFIQ_BUILD_TEST AND TARGET FingerQualityCtrlTest)
        add_dependencies(stage_runtime FingerQualityCtrlTest)
        add_custom_command(TARGET stage_runtime POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy
                    "$<TARGET_FILE:FingerQualityCtrlTest>"
                    "${CFIQ_STAGE_DIR}/test/FingerQualityCtrlTest"
        )
    endif()
endfunction()
