#ifndef boxm2_data_traits_h_
#define boxm2_data_traits_h_
//:
// \file
// \brief traits for data types
//
// \author Vishal Jain
// \date nov 17, 2010

#include <string>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>

#include "boxm2_normal_albedo_array.h"
#include "boxm2_feature_vector.h"

class boxm2_mog3_grey_processor;
class boxm2_gauss_grey_processor;
class boxm2_gauss_rgb_processor;
class boxm2_mog6_view_processor;
class boxm2_mog6_view_compact_processor;

/**
 * dec: Use the "X macro" pattern to define a table of enum values, string ids, and
 * associated datatypes, and then use the table to actually create the enum and
 * specializations of the traits classes for each row of the table.  It is ugly, but not
 * as ugly as the many hundreds of lines of copy/pasted code duplication that it replaced.
 * To add a new boxm2 data type, you now just add a row to the bottom of the table below.
 * If the type needs a "processor type" associated with it, that is now in a separate table further below.
 */
typedef vnl_vector_fixed<unsigned char, 2>  vnl_vector_fixed_unsigned_char_2;
typedef vnl_vector_fixed<unsigned char, 8>  vnl_vector_fixed_unsigned_char_8;
typedef vnl_vector_fixed<unsigned char, 16> vnl_vector_fixed_unsigned_char_16;

typedef vnl_vector_fixed<unsigned short, 4> vnl_vector_fixed_unsigned_short_2;
typedef vnl_vector_fixed<unsigned short, 4> vnl_vector_fixed_unsigned_short_4;
typedef vnl_vector_fixed<unsigned short, 8> vnl_vector_fixed_unsigned_short_8;

typedef vnl_vector_fixed<short, 8> vnl_vector_fixed_short_8;

typedef vnl_vector_fixed<int, 4>  vnl_vector_fixed_int_4;
typedef vnl_vector_fixed<int, 8>  vnl_vector_fixed_int_8;
typedef vnl_vector_fixed<int, 16> vnl_vector_fixed_int_16;

typedef vnl_vector_fixed<float, 4>  vnl_vector_fixed_float_4;
typedef vnl_vector_fixed<float, 8>  vnl_vector_fixed_float_8;
typedef vnl_vector_fixed<float, 9>  vnl_vector_fixed_float_9;
typedef vnl_vector_fixed<float, 16> vnl_vector_fixed_float_16;

#define BOXM2_DATATYPE_TABLE \
  X(BOXM2_ALPHA, "alpha", float) \
  X(BOXM2_GAMMA, "gamma", float) \
  X(BOXM2_MOG3_GREY_16, "boxm2_mog3_grey16", vnl_vector_fixed_unsigned_short_8) \
  X(BOXM2_MOG3_GREY, "boxm2_mog3_grey", vnl_vector_fixed_unsigned_char_8) \
  X(BOXM2_MOG6_VIEW_COMPACT, "boxm2_mog6_view_compact", vnl_vector_fixed_unsigned_char_16) \
  X(BOXM2_MOG6_VIEW, "boxm2_mog6_view", vnl_vector_fixed_float_16) \
  X(BOXM2_BATCH_HISTOGRAM, "boxm2_batch_histogram", vnl_vector_fixed_float_8) \
  X(BOXM2_GAUSS_RGB_VIEW_COMPACT, "boxm2_gauss_rgb_view_compact", vnl_vector_fixed_int_8) \
  X(BOXM2_GAUSS_RGB_VIEW, "boxm2_gauss_rgb_view", vnl_vector_fixed_int_16) \
  X(BOXM2_GAUSS_RGB, "boxm2_gauss_rgb", vnl_vector_fixed_unsigned_char_8) \
  X(BOXM2_GAUSS_UV_VIEW, "boxm2_gauss_uv_view", vnl_vector_fixed_int_4) \
  X(BOXM2_MOG2_RGB,"boxm2_mog2_rgb", vnl_vector_fixed_unsigned_char_16) \
  X(BOXM2_NUM_OBS_VIEW_COMPACT, "boxm2_num_obs_view_compact", vnl_vector_fixed_short_8) \
  X(BOXM2_NUM_OBS_VIEW, "boxm2_num_obs_view", vnl_vector_fixed_float_8) \
  X(BOXM2_NUM_OBS_SINGLE_INT, "boxm2_num_obs_single_int", unsigned) \
  X(BOXM2_NUM_OBS_SINGLE, "boxm2_num_obs_single", unsigned short) \
  X(BOXM2_NUM_OBS, "boxm2_num_obs", vnl_vector_fixed_unsigned_short_4) \
  X(BOXM2_LABEL_SHORT, "boxm2_label_short", short) \
  X(BOXM2_INTENSITY, "boxm2_intensity", float) \
  X(BOXM2_AUX0, "aux0", float) \
  X(BOXM2_AUX1, "aux1", float) \
  X(BOXM2_AUX2, "aux2", float) \
  X(BOXM2_AUX3, "aux3", float) \
  X(BOXM2_AUX4, "aux4", float) \
  X(BOXM2_AUX, "aux", vnl_vector_fixed_float_4) \
  X(BOXM2_FLOAT16, "float16", vnl_vector_fixed_float_16) \
  X(BOXM2_FLOAT8, "float8", vnl_vector_fixed_float_8) \
  X(BOXM2_FLOAT, "float", float) \
  X(BOXM2_VIS_SPHERE, "boxm2_vis_sphere", vnl_vector_fixed_float_16) \
  X(BOXM2_POINT, "boxm2_point", vnl_vector_fixed_float_4) \
  X(BOXM2_VIS_SCORE, "boxm2_vis_score", float) \
  X(BOXM2_GAUSS_GREY, "boxm2_gauss_grey", vnl_vector_fixed_unsigned_char_2) \
  X(BOXM2_NORMAL_ALBEDO_ARRAY, "boxm2_normal_albedo_array", boxm2_normal_albedo_array) \
  X(BOXM2_NORMAL, "boxm2_normal", vnl_vector_fixed_float_4) \
  X(BOXM2_COVARIANCE, "boxm2_covariance", vnl_vector_fixed_float_9) \
  X(BOXM2_FEATURE_VECTOR, "boxm2_feature_vector", boxm2_feature_vector) \
  X(BOXM2_PIXEL, "boxm2_pixel", unsigned char) \
  X(BOXM2_EXPECTATION, "boxm2_expectation", float) \
  X(BOXM2_DATA_INDEX, "boxm2_data_index", unsigned int) \
  X(BOXM2_RAY_DIR, "boxm2_ray_dir", vnl_vector_fixed_float_4) \
  X(BOXM2_CHAR8, "char8", vnl_vector_fixed_unsigned_char_8) \
  X(BOXM2_VEC3D, "boxm2_vec3d", vnl_vector_fixed_float_4) \
  X(BOXM2_VECF_EYELID, "boxm2_vecf_eyelid", vnl_vector_fixed_float_16)



// define the boxm2_data_type enum using the X macro pattern and the table above.
#define X(enum_val, string_val, datatype_val) enum_val,
enum boxm2_data_type {
  BOXM2_DATATYPE_TABLE
  BOXM2_UNKNOWN // last enum value, also avoids trialing comma problem
};
#undef X

//: voxel datatype traits: will be specialized for each relevant boxm2_data_type val
template <boxm2_data_type type>
class boxm2_data_traits;

// specialize the traits class for each boxm2_data_type value in the table above using the X macro pattern
#define X(enum_val, string_val, datatype_val) \
  template<> \
  class boxm2_data_traits<enum_val> \
  { \
  public: \
    typedef datatype_val datatype; \
    static std::size_t datasize() { return sizeof(datatype); } \
    static std::string prefix(const std::string& identifier = "") \
    { if (!identifier.size()) return string_val; else return string_val + std::string("_") + identifier; } \
  };
BOXM2_DATATYPE_TABLE
#undef X

// A Collection of functions mapping datatypes to properties.
// There are handy if you don't know the enum val at compile time.
class boxm2_data_info
{
public:

// map string prefix to enum val
// note that some prefixes are substrings of others.
// this could be made more efficient by requiring that the table is sorted
// such that types that are substrings are listed last, but that is probably asking for trouble.
static boxm2_data_type data_type(std::string const& prefix) {
  boxm2_data_type retval = BOXM2_UNKNOWN;
#define X(enum_val, string_val, datatype_val) \
  if (prefix.find(boxm2_data_traits<enum_val>::prefix()) == 0) {\
    return enum_val; \
  }
  BOXM2_DATATYPE_TABLE
#undef X
  return retval;
}

// map enum val to string prefix
static std::string prefix(boxm2_data_type data_type, std::string const& identifier="") {
  switch(data_type) {
#define X(enum_val, string_val, datatype_val) \
    case enum_val: \
      return boxm2_data_traits<enum_val>::prefix(identifier);
    BOXM2_DATATYPE_TABLE
    default:
      // switch fell through with no match
      return "unknown";
#undef X
  }
  // should be unreachable.
  return "unknown";
}

// map enum to datasize
static std::size_t datasize(boxm2_data_type data_type) {
  switch (data_type) {
#define X(enum_val, string_val, datatype_val) \
    case enum_val: \
      return boxm2_data_traits<enum_val>::datasize();
    BOXM2_DATATYPE_TABLE
    default:
      // switch fell through with no match
      return 0;
#undef X
  }
  // should be unreachable.
  return 0;
};

// map string prefix to datasize
static std::size_t datasize(std::string const& prefix) {
  return datasize(data_type(prefix));
}


// TODO: create a table mapping enum to print function, or just require that all types have a stream operator.
static void print_data(std::string const& prefix, char *cell)
{
  if (prefix.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(cell)[0];
    return;
  }
  if (prefix.find(boxm2_data_traits<BOXM2_AUX0>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX0>::datatype*>(cell)[0];
    return;
  }
  if (prefix.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX>::datatype*>(cell)[0];
    return;
  }

  if (prefix.find(boxm2_data_traits<BOXM2_INTENSITY>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_INTENSITY>::datatype*>(cell)[0];
    return;
  }

  if (prefix.find(boxm2_data_traits<BOXM2_POINT>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(cell)[0];
    return;
  }

  if (prefix.find(boxm2_data_traits<BOXM2_COVARIANCE>::prefix()) != std::string::npos) {
    std::cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_COVARIANCE>::datatype*>(cell)[0];
    return;
  }

  std::cerr << "In boxm2_data_info::print_data() -- type: " << prefix << " could not be identified!\n";
  return;
}

};


// A table mapping boxm2 appearance model types to the class resposible for processing them.
// Use the "X macro" pattern to generate the traits-style boxm2_processor_type specializations from the table
#define BOXM2_PROCESSOR_TABLE \
  X(BOXM2_MOG3_GREY, boxm2_mog3_grey_processor) \
  X(BOXM2_MOG6_VIEW, boxm2_mog6_view_processor) \
  X(BOXM2_MOG6_VIEW_COMPACT, boxm2_mog6_view_compact_processor) \
  X(BOXM2_MOG3_GREY_16, boxm2_mog3_grey_processor) \
  X(BOXM2_GAUSS_RGB, boxm2_gauss_rgb_processor) \
  X(BOXM2_GAUSS_GREY, boxm2_gauss_grey_processor) \
  X(BOXM2_GAUSS_UV_VIEW, boxm2_mog6_view_processor) \
  X(BOXM2_GAUSS_RGB_VIEW, boxm2_mog6_view_processor) \
  X(BOXM2_GAUSS_RGB_VIEW_COMPACT, boxm2_mog6_view_processor)

// the empty base declaration.  This template will be specialized for each row in the table above.
template <boxm2_data_type type>
class boxm2_processor_type;

// define the specialization generically
#define X(enum_val, processor_type) \
  template<> \
  class boxm2_processor_type<enum_val> \
  { \
    public: \
    typedef processor_type type; \
  };

// declare the specializations via the "X macro" pattern.
BOXM2_PROCESSOR_TABLE
#undef X

#endif
