// This is core/vil/file_formats/vil_nitf_typeinfo.h
#ifndef vil_nitf_typeinfo_h_
#define vil_nitf_typeinfo_h_

// Usage for typedef: typedef ACTUAL ALIAS
typedef int StatusCode;
typedef unsigned long byte_offset;

// Constant definitions
const int STATUS_GOOD = 0;
const int STATUS_BAD = -1;
const unsigned short MAX_UINT_16 = 65535;

// Taken from TargetJr Image.h - move to vil_nitf_image.h ??  MAL 8oct2003

enum InterleaveType { PIXEL_INTERLEAVED,
                      BLOCK_INTERLEAVED,
                      ROW_INTERLEAVED,  //Needed for NITFv2.1
                      NOT_INTERLEAVED };

enum PixelJustify   { PJUST_LEFT, PJUST_MASKED, PJUST_RIGHT };

// From TargetJr. file BandedImage.h
enum BandType {INTERLEAVED, BANDED};
const int NO_LEVEL = -999999;

// From TargetJr. file NITFHeader.h, class ImageSubHeader
enum rpc_type {UNDEFINED=0, RPC00A, RPC00B};

#endif // vil_nitf_typeinfo_h_

