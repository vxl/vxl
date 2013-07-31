#ifndef boxm2_data_traits_h_
#define boxm2_data_traits_h_
//:
// \file
// \brief traits for data types
//
// \author Vishal Jain
// \date nov 17, 2010

#include <vcl_string.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>

#include "boxm2_normal_albedo_array.h"
#include "boxm2_feature_vector.h"

class boxm2_mog3_grey_processor;
class boxm2_gauss_grey_processor;
class boxm2_gauss_rgb_processor;
class boxm2_mog6_view_processor;
class boxm2_mog6_view_compact_processor;

enum boxm2_data_type
{
  BOXM2_ALPHA=0,
  BOXM2_GAMMA,
  BOXM2_MOG3_GREY,
  BOXM2_MOG3_GREY_16,
  BOXM2_MOG6_VIEW,
  BOXM2_MOG6_VIEW_COMPACT,
  BOXM2_BATCH_HISTOGRAM,
  BOXM2_GAUSS_RGB,
  BOXM2_GAUSS_RGB_VIEW,
  BOXM2_GAUSS_RGB_VIEW_COMPACT,
  BOXM2_GAUSS_UV_VIEW,
  BOXM2_MOG2_RGB,
  BOXM2_NUM_OBS,
  BOXM2_NUM_OBS_SINGLE,
  BOXM2_NUM_OBS_SINGLE_INT,
  BOXM2_NUM_OBS_VIEW,
  BOXM2_NUM_OBS_VIEW_COMPACT,
  BOXM2_LABEL_SHORT,
  BOXM2_AUX,
  BOXM2_INTENSITY,
  BOXM2_AUX0,
  BOXM2_AUX1,
  BOXM2_AUX2,
  BOXM2_AUX3,
  BOXM2_AUX4,
  BOXM2_FLOAT,
  BOXM2_FLOAT8,
  BOXM2_FLOAT16,
  BOXM2_VIS_SPHERE,
  BOXM2_NORMAL,
  BOXM2_POINT,
  BOXM2_VIS_SCORE,
  BOXM2_GAUSS_GREY,
  BOXM2_NORMAL_ALBEDO_ARRAY,
  BOXM2_COVARIANCE,
  BOXM2_FEATURE_VECTOR,
  BOXM2_PIXEL,
  BOXM2_EXPECTATION,
  BOXM2_DATA_INDEX,
  BOXM2_RAY_DIR,
  BOXM2_UNKNOWN
};

//: Pixel properties for templates.
template <boxm2_data_type type>
class boxm2_data_traits;

//: traits for a mixture of gaussian appearance model of gray-scale images
template<>
class boxm2_data_traits<BOXM2_ALPHA>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "alpha"; else return "alpha_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAMMA>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "gamma"; else return "gamma_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog3_grey"; else return "boxm2_mog3_grey_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG6_VIEW>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<float, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog6_view"; else return "boxm2_mog6_view_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>
{
 public:
  typedef boxm2_mog6_view_compact_processor processor;
  typedef vnl_vector_fixed<unsigned char, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog6_view_compact"; else return "boxm2_mog6_view_compact_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_MOG3_GREY_16>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned short, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog3_grey_16"; else return "boxm2_mog3_grey_16_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAUSS_RGB>
{
 public:
  typedef boxm2_gauss_rgb_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_rgb"; else return "boxm2_gauss_rgb_"+identifier; }
};

//: simple gaussian with a sigma and std dev
template<>
class boxm2_data_traits<BOXM2_GAUSS_GREY>
{
 public:
  typedef boxm2_gauss_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 2> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_grey"; else return "boxm2_gauss_grey_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAUSS_UV_VIEW>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<int, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_uv_view"; else return "boxm2_gauss_UV_view_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<int, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_rgb_view"; else return "boxm2_gauss_rgb_view_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<int, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_gauss_rgb_view_compact"; else return "boxm2_gauss_rgb_view_compact_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_MOG2_RGB>
{
 public:
  typedef vnl_vector_fixed<unsigned char, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_mog2_rgb"; else return "boxm2_mog2_rgb_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_NORMAL>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_normal"; else return "boxm2_normal_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_RAY_DIR>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_ray_dir"; else return "boxm2_ray_dir_"+identifier; }
};



template<>
class boxm2_data_traits<BOXM2_POINT>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_point"; else return "boxm2_point_"+identifier; }
};

// 3x3 covariance matrix for a 3d point
template<>
class boxm2_data_traits<BOXM2_COVARIANCE>
{
 public:
  typedef vnl_vector_fixed<float, 9> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_covariance"; else return "boxm2_covariance_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_NUM_OBS>
{
 public:
  typedef vnl_vector_fixed<unsigned short, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_num_obs"; else return "boxm2_num_obs_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS_VIEW>
{
 public:
  typedef vnl_vector_fixed<float, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_num_obs_view"; else return "boxm2_num_obs_view"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS_VIEW_COMPACT>
{
 public:
  typedef vnl_vector_fixed<unsigned short, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_num_obs_view_compact"; else return "boxm2_num_obs_view_compact"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>
{
 public:
  typedef unsigned short datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_num_obs_single"; else return "boxm2_num_obs_single_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>
{
 public:
  typedef unsigned datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_num_obs_single_int"; else return "boxm2_num_obs_single_int_"+identifier; }
};


//: Aux data contains four values to assist update calculations:
// * seg_len_array,   (total cell segment length)
// * mean_obs_array,  (total cell mean obs)
// * vis_array,       (total cell visibility)
// * beta_array,      (total cell bayes update factor)
template<>
class boxm2_data_traits<BOXM2_AUX>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux"; else return "aux_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_FLOAT8>
{
 public:
  typedef vnl_vector_fixed<float, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "float8"; else return "float8_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_FLOAT>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "float"; else return "float_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_FLOAT16>
{
 public:
  typedef vnl_vector_fixed<float, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "float16"; else return "float16_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_VIS_SPHERE>
{
 public:
  typedef vnl_vector_fixed<float, 16> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_vis_sphere"; else return "boxm2_vis_sphere_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_VIS_SCORE>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_vis_score"; else return "boxm2_vis_score_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_AUX0>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux0"; else return "aux0_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_AUX1>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux1"; else return "aux1_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_AUX2>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux2"; else return "aux2_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_AUX3>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux3"; else return "aux3_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_AUX4>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "aux4"; else return "aux4_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>
{
 public:
  typedef vnl_vector_fixed<float, 8> datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix() { return "boxm2_batch_histogram"; }
};

template<>
class boxm2_data_traits<BOXM2_INTENSITY>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(datatype); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_intensity"; else return "boxm2_intensity_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>
{
 public:
  typedef boxm2_normal_albedo_array datatype;
  static vcl_size_t datasize() { return sizeof(boxm2_normal_albedo_array); }
  static vcl_string prefix() { return "boxm2_normal_albedo_array"; }
};

template<>
class boxm2_data_traits<BOXM2_FEATURE_VECTOR>
{
 public:
  typedef boxm2_feature_vector datatype;
  static vcl_size_t datasize() { return sizeof(boxm2_feature_vector); }
  static vcl_string prefix() { return "boxm2_feature_vector"; }
};
template<>
class boxm2_data_traits<BOXM2_LABEL_SHORT>
{
 public:
  typedef short datatype;
  static vcl_size_t datasize() { return sizeof(short); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_label_short"; else return "boxm2_label_short_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_PIXEL>
{
 public:
  typedef unsigned char datatype;
  static vcl_size_t datasize() { return sizeof(unsigned char); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_pixel"; else return "boxm2_pixel_"+identifier; }
};


template<>
class boxm2_data_traits<BOXM2_EXPECTATION>
{
 public:
  typedef float datatype;
  static vcl_size_t datasize() { return sizeof(float); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_expectation"; else return "boxm2_expectation_"+identifier; }
};

template<>
class boxm2_data_traits<BOXM2_DATA_INDEX>
{
 public:
  typedef unsigned int datatype;
  static vcl_size_t datasize() { return sizeof(unsigned int); }
  static vcl_string prefix(const vcl_string& identifier = "")
  { if (!identifier.size()) return "boxm2_data_index"; else return "boxm2_data_index_"+identifier; }
};



//: HACKY WAY TO GENERICALLY GET DATASIZES -
class boxm2_data_info
{
 public:

  static vcl_size_t datasize(vcl_string prefix)
  {
    // some of them changed to using find method to account for identifiers

    if (prefix.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_ALPHA>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_GAMMA>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_GAMMA>::datasize();

     if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_MOG6_VIEW>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::datasize();

    if (prefix == boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix())
      return boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_MOG2_RGB>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_MOG2_RGB>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_VIEW_COMPACT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NUM_OBS_VIEW_COMPACT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_VIEW>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NUM_OBS_VIEW>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NUM_OBS>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_AUX0>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX0>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_AUX1>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX1>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_AUX2>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX2>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_AUX3>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX3>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_AUX4>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX4>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_AUX>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_FLOAT16>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_FLOAT16>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_FLOAT8>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_FLOAT8>::datasize();
    if (prefix.find(boxm2_data_traits<BOXM2_FLOAT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_FLOAT>::datasize();

    if (prefix == boxm2_data_traits<BOXM2_VIS_SPHERE>::prefix())
      return boxm2_data_traits<BOXM2_VIS_SPHERE>::datasize();
    if (prefix == boxm2_data_traits<BOXM2_VIS_SCORE>::prefix())
      return boxm2_data_traits<BOXM2_VIS_SCORE>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_INTENSITY>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_INTENSITY>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_GAUSS_RGB>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_GAUSS_GREY>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_NORMAL>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_NORMAL>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_POINT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_POINT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_COVARIANCE>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_COVARIANCE>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_FEATURE_VECTOR>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_FEATURE_VECTOR>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_LABEL_SHORT>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_PIXEL>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_PIXEL>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_EXPECTATION>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_EXPECTATION>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_DATA_INDEX>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_DATA_INDEX>::datasize();

    if (prefix.find(boxm2_data_traits<BOXM2_RAY_DIR>::prefix()) != vcl_string::npos)
      return boxm2_data_traits<BOXM2_RAY_DIR>::datasize();
    return 0;
  }


  static boxm2_data_type data_type(vcl_string prefix)
  {
    // some of them changed to using find method to account for identifiers
    if (prefix.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != vcl_string::npos)
      return BOXM2_ALPHA ;
    else if (prefix.find(boxm2_data_traits<BOXM2_GAMMA>::prefix()) != vcl_string::npos)
      return BOXM2_GAMMA ;
    else if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix()) != vcl_string::npos)
      return  BOXM2_MOG3_GREY_16 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos)
      return  BOXM2_MOG3_GREY ;
    else if (prefix == boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix())
      return  BOXM2_BATCH_HISTOGRAM ;
    else if (prefix.find(boxm2_data_traits<BOXM2_MOG2_RGB>::prefix()) != vcl_string::npos)
      return  BOXM2_MOG2_RGB ;
    else if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix()) != vcl_string::npos)
      return  BOXM2_NUM_OBS_SINGLE_INT ;
    else if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix()) != vcl_string::npos)
      return  BOXM2_NUM_OBS_SINGLE ;
    else if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS>::prefix()) != vcl_string::npos)
      return  BOXM2_NUM_OBS ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX0>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX0 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX1>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX1 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX2>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX2 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX3>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX3 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX4>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX4 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != vcl_string::npos)
      return  BOXM2_AUX ;
    else if (prefix.find(boxm2_data_traits<BOXM2_FLOAT16>::prefix()) != vcl_string::npos)
      return  BOXM2_FLOAT16 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_FLOAT8>::prefix()) != vcl_string::npos)
      return  BOXM2_FLOAT8 ;
    else if (prefix.find(boxm2_data_traits<BOXM2_FLOAT>::prefix()) != vcl_string::npos)
      return  BOXM2_FLOAT;
    else if (prefix.find(boxm2_data_traits<BOXM2_INTENSITY>::prefix()) != vcl_string::npos)
      return  BOXM2_INTENSITY ;
    else if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != vcl_string::npos)
      return  BOXM2_GAUSS_RGB ;
    else if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix()) != vcl_string::npos)
      return  BOXM2_GAUSS_GREY ;
    else if (prefix.find(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix()) != vcl_string::npos)
      return  BOXM2_NORMAL_ALBEDO_ARRAY ;
    else if (prefix.find(boxm2_data_traits<BOXM2_POINT>::prefix()) != vcl_string::npos)
      return  BOXM2_POINT ;
    else if (prefix.find(boxm2_data_traits<BOXM2_COVARIANCE>::prefix()) != vcl_string::npos)
      return  BOXM2_COVARIANCE ;
    else if (prefix.find(boxm2_data_traits<BOXM2_FEATURE_VECTOR>::prefix()) != vcl_string::npos)
      return  BOXM2_FEATURE_VECTOR ;
    else if (prefix.find(boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix()) != vcl_string::npos)
      return  BOXM2_LABEL_SHORT ;
    else if (prefix.find(boxm2_data_traits<BOXM2_PIXEL>::prefix()) != vcl_string::npos)
      return  BOXM2_PIXEL ;
    else if (prefix.find(boxm2_data_traits<BOXM2_EXPECTATION>::prefix()) != vcl_string::npos)
      return  BOXM2_EXPECTATION ;
    else if (prefix.find(boxm2_data_traits<BOXM2_DATA_INDEX>::prefix()) != vcl_string::npos)
      return  BOXM2_DATA_INDEX ;
    else if (prefix.find(boxm2_data_traits<BOXM2_RAY_DIR>::prefix()) != vcl_string::npos)
          return  BOXM2_RAY_DIR ;
    else
      return BOXM2_UNKNOWN;
  }

  static void print_data(vcl_string prefix, char *cell)
  {
    if (prefix.find(boxm2_data_traits<BOXM2_ALPHA>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(cell)[0];
      return;
    }
#if 0
    if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY_16>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(cell)[0];
      return;
    }

    if (prefix == boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix()) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_MOG2_RGB>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_MOG2_RGB>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_NUM_OBS>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(cell)[0];
      return;
    }
#endif // 0
    if (prefix.find(boxm2_data_traits<BOXM2_AUX0>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX0>::datatype*>(cell)[0];
      return;
    }
#if 0
    if (prefix.find(boxm2_data_traits<BOXM2_AUX1>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX1>::datatype*>(cell)[0];
      return;
    }
    if (prefix.find(boxm2_data_traits<BOXM2_AUX2>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX2>::datatype*>(cell)[0];
      return;
    }
    if (prefix.find(boxm2_data_traits<BOXM2_AUX3>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX3>::datatype*>(cell)[0];
      return;
    }
#endif // 0
    if (prefix.find(boxm2_data_traits<BOXM2_AUX>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_AUX>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_INTENSITY>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_INTENSITY>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_POINT>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(cell)[0];
      return;
    }

    if (prefix.find(boxm2_data_traits<BOXM2_COVARIANCE>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_COVARIANCE>::datatype*>(cell)[0];
      return;
    }

#if 0
    if (prefix.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != vcl_string::npos) {
      vcl_cout <<  reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(cell)[0];
      return;
    }
#endif // 0

    vcl_cerr << "In boxm2_data_info::print_data() -- type: " << prefix << " could not be identified!\n";
    return;
  }
};


#endif
