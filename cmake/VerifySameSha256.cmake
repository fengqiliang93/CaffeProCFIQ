if(NOT DEFINED LEFT OR NOT DEFINED RIGHT)
    message(FATAL_ERROR "VerifySameSha256.cmake requires LEFT and RIGHT")
endif()

if(NOT EXISTS "${LEFT}")
    message(FATAL_ERROR "Missing LEFT file: ${LEFT}")
endif()

if(NOT EXISTS "${RIGHT}")
    message(FATAL_ERROR "Missing RIGHT file: ${RIGHT}")
endif()

file(SHA256 "${LEFT}" LEFT_SHA256)
file(SHA256 "${RIGHT}" RIGHT_SHA256)

if(NOT LEFT_SHA256 STREQUAL RIGHT_SHA256)
    message(FATAL_ERROR "SHA-256 mismatch: ${LEFT} (${LEFT_SHA256}) != ${RIGHT} (${RIGHT_SHA256})")
endif()

message(STATUS "SHA-256 verified: ${LEFT} == ${RIGHT} (${LEFT_SHA256})")
