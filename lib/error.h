#ifndef PLAZMA_LIB_ERROR_H
#define PLAZMA_LIB_ERROR_H

/** Generic error */
#define LIB_ERR_ERROR_GENERIC_ERROR          -0x0001
/** This is a bug in the library */
#define LIB_ERR_ERROR_CORRUPTION_DETECTED    -0x006E

/** Hardware accelerator failed */
#define LIB_ERR_PLATFORM_HW_ACCEL_FAILED     -0x0070
/** The requested feature is not supported by the platform */
#define LIB_ERR_PLATFORM_FEATURE_UNSUPPORTED -0x0072

#define LIB_ERR_IO_WANT_READ                 -0x6900
#define LIB_ERR_IO_WANT_WRITE                -0x6880

#endif // PLAZMA_LIB_ERROR_H