#ifndef ACQUIRE_ZARR_H
#define ACQUIRE_ZARR_H

#include <stdint.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// Forward declaration of AcquireZarrSinkWrapper
struct AcquireZarrSinkWrapper;



// Configuration for Zarr sink
struct AcquireZarrSinkConfig
{
    char filename[512];
   

    // Zarr Version
    enum AcquireZarrVersion
    {
        AcquireZarrVersion_2,
        AcquireZarrVersion_3
    } zarr_version;

    // Compression options for Zarr
    enum AcquireZarrCompression
    {
        AcquireZarrCompression_NONE,
        AcquireZarrCompression_BLOSC_LZ4,
        AcquireZarrCompression_BLOSC_ZSTD,
    } compression;

    uint8_t multiscale; // 0 or 1, because I don't have bool in C
};


// 

// Open a Zarr sink with the given configuration
EXTERNC struct AcquireZarrSinkWrapper* zarr_sink_open(const struct AcquireZarrSinkConfig* config);

// Close the Zarr sink
EXTERNC void zarr_sink_close(struct AcquireZarrSinkWrapper*);

EXTERNC int zarr_sink_append(struct AcquireZarrSinkWrapper* zarr_sink, uint8_t* image_data, size_t image_size);


#endif // ACQUIRE_ZARR_H