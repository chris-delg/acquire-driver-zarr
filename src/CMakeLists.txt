if (NOT TARGET acquire-core-logger)
    add_subdirectory(../acquire-common/acquire-core-libs ${CMAKE_CURRENT_BINARY_DIR}/acquire-core-libs)
endif ()

set(tgt acquire-driver-zarr)
add_library(${tgt} MODULE
        common/dimension.hh
        common/dimension.cpp
        common/thread.pool.hh
        common/thread.pool.cpp
        common/s3.connection.hh
        common/s3.connection.cpp
        common/utilities.hh
        common/utilities.cpp
        writers/sink.hh
        writers/sink.creator.hh
        writers/sink.creator.cpp
        writers/file.sink.hh
        writers/file.sink.cpp
        writers/s3.sink.hh
        writers/s3.sink.cpp
        writers/array.writer.hh
        writers/array.writer.cpp
        writers/zarrv2.array.writer.hh
        writers/zarrv2.array.writer.cpp
        writers/zarrv3.array.writer.hh
        writers/zarrv3.array.writer.cpp
        writers/blosc.compressor.hh
        writers/blosc.compressor.cpp
        zarr.hh
        zarr.cpp
        zarr.v2.hh
        zarr.v2.cpp
        zarr.v3.hh
        zarr.v3.cpp
        zarr.driver.c
)

target_include_directories(${tgt} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
)

target_enable_simd(${tgt})
target_link_libraries(${tgt} PRIVATE
        acquire-core-logger
        acquire-core-platform
        acquire-device-kit
        acquire-device-properties
        blosc_static
        nlohmann_json::nlohmann_json
        miniocpp::miniocpp
)
set_target_properties(${tgt} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

install(TARGETS ${tgt} LIBRARY DESTINATION lib)
