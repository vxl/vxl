#ifndef bstm_data_traits_h_
#define bstm_data_traits_h_

#include <string>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>

#include <boxm2/boxm2_data_traits.h>

class boxm2_mog3_grey_processor;
class boxm2_gauss_grey_processor;
class boxm2_gauss_rgb_processor;
class boxm2_mog6_view_processor;
class boxm2_mog6_view_compact_processor;

enum bstm_data_type
{
  BSTM_ALPHA=0,
  BSTM_CHANGE,
  BSTM_MOG3_GREY,
  BSTM_MOG3_GREY_16,
  BSTM_MOG6_VIEW,
  BSTM_NUM_OBS_VIEW,
  BSTM_NUM_OBS_VIEW_COMPACT,
  BSTM_MOG6_VIEW_COMPACT,
  BSTM_GAUSS_RGB,
  BSTM_GAUSS_RGB_VIEW,
  BSTM_GAUSS_RGB_VIEW_COMPACT,
  BSTM_NUM_OBS_SINGLE,
  BSTM_NUM_OBS,
  BSTM_LABEL,
  BSTM_RAY_DIR,
  BSTM_POINT,
  BSTM_AUX0,
  BSTM_AUX1,
  BSTM_AUX2,
  BSTM_AUX3,
  BSTM_UNKNOWN
};

// Pixel properties for templates.
template <bstm_data_type type>
class bstm_data_traits;

template<>
class bstm_data_traits<BSTM_ALPHA>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "alpha"; else return "alpha_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_CHANGE>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_change"; else return "bstm_change_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_NUM_OBS>
{
 public:
  typedef vnl_vector_fixed<unsigned short, 4> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_num_obs"; else return "bstm_num_obs_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_NUM_OBS_SINGLE>
{
 public:
  typedef unsigned short datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_num_obs_single"; else return "bstm_num_obs_single_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_GAUSS_RGB>
{
 public:
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_gauss_rgb"; else return "bstm_gauss_rgb_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_GAUSS_RGB_VIEW>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<int, 16> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_gauss_rgb_view"; else return "bstm_gauss_rgb_view_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>
{
 public:
  typedef vnl_vector_fixed<int, 8> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_gauss_rgb_view_compact"; else return "bstm_gauss_rgb_view_compact_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_MOG3_GREY>
{
 public:
  typedef boxm2_mog3_grey_processor processor;
  typedef vnl_vector_fixed<unsigned char, 8> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_mog3_grey"; else return "bstm_mog3_grey_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_MOG6_VIEW>
{
 public:
  typedef boxm2_mog6_view_processor processor;
  typedef vnl_vector_fixed<float, 16> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_mog6_view"; else return "bstm_mog6_view_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>
{
 public:
  typedef boxm2_mog6_view_compact_processor processor;
  typedef vnl_vector_fixed<unsigned char, 16> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_mog6_view_compact"; else return "bstm_mog6_view_compact_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_NUM_OBS_VIEW>
{
 public:
  typedef vnl_vector_fixed<float, 8> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_num_obs_view"; else return "bstm_num_obs_view"+identifier; }
};

template<>
class bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>
{
 public:
  typedef vnl_vector_fixed<unsigned short, 8> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_num_obs_view_compact"; else return "bstm_num_obs_view_compact"+identifier; }
};


template<>
class bstm_data_traits<BSTM_LABEL>
{
 public:
  typedef unsigned char datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_label"; else return "bstm_label_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_AUX0>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "aux0"; else return "aux0_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_AUX1>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "aux1"; else return "aux1_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_AUX2>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "aux2"; else return "aux2_"+identifier; }
};

template<>
class bstm_data_traits<BSTM_AUX3>
{
 public:
  typedef float datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "aux3"; else return "aux3_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_RAY_DIR>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_ray_dir"; else return "bstm_ray_dir_"+identifier; }
};


template<>
class bstm_data_traits<BSTM_POINT>
{
 public:
  typedef vnl_vector_fixed<float, 4> datatype;
  static std::size_t datasize() { return sizeof(datatype); }
  static std::string prefix(const std::string& identifier = "")
  { if (!identifier.size()) return "bstm_point"; else return "bstm_point_"+identifier; }
};


// HACKY WAY TO GENERICALLY GET DATASIZES -
class bstm_data_info
{
 public:

  static std::size_t datasize(std::string prefix)
  {
    // some of them changed to using find method to account for identifiers

    if (prefix.find(bstm_data_traits<BSTM_ALPHA>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_ALPHA>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_MOG3_GREY>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_MOG3_GREY>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_NUM_OBS_VIEW_COMPACT>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_NUM_OBS_VIEW>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_NUM_OBS_VIEW>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_NUM_OBS_SINGLE>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_NUM_OBS_SINGLE>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_NUM_OBS>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_NUM_OBS>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_GAUSS_RGB>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_MOG6_VIEW>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_MOG6_VIEW>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_AUX0>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_AUX0>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_AUX1>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_AUX1>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_AUX2>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_AUX2>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_AUX3>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_AUX3>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_RAY_DIR>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_RAY_DIR>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_LABEL>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_LABEL>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_POINT>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_POINT>::datasize();
    if (prefix.find(bstm_data_traits<BSTM_CHANGE>::prefix()) != std::string::npos)
      return bstm_data_traits<BSTM_CHANGE>::datasize();
    return 0;
  }

  static bstm_data_type data_type(std::string prefix)
  {
    // some of them changed to using find method to account for identifiers
    if (prefix.find(bstm_data_traits<BSTM_ALPHA>::prefix()) != std::string::npos)
      return BSTM_ALPHA ;
    else if (prefix.find(bstm_data_traits<BSTM_MOG3_GREY>::prefix()) != std::string::npos)
      return BSTM_MOG3_GREY ;
    else if (prefix.find(bstm_data_traits<BSTM_NUM_OBS>::prefix()) != std::string::npos)
      return  BSTM_NUM_OBS ;
    else if (prefix.find(bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) != std::string::npos)
      return  BSTM_GAUSS_RGB ;
    else if (prefix.find(bstm_data_traits<BSTM_LABEL>::prefix()) != std::string::npos)
      return  BSTM_LABEL ;
    else if (prefix.find(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix()) != std::string::npos)
      return  BSTM_GAUSS_RGB_VIEW_COMPACT ;
    else if (prefix.find(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()) != std::string::npos)
      return  BSTM_MOG6_VIEW_COMPACT ;
    else if (prefix.find(bstm_data_traits<BSTM_MOG6_VIEW>::prefix()) != std::string::npos)
      return  BSTM_MOG6_VIEW ;
    else
      return BSTM_UNKNOWN;
  }
};

template <boxm2_data_type BOXM2_TYPE> struct boxm2_to_bstm_type;

template <> struct boxm2_to_bstm_type<BOXM2_ALPHA> {
  static const bstm_data_type bstm_type = BSTM_ALPHA;
};
template <> struct boxm2_to_bstm_type<BOXM2_MOG3_GREY> {
  static const bstm_data_type bstm_type = BSTM_MOG3_GREY;
};
template <> struct boxm2_to_bstm_type<BOXM2_MOG3_GREY_16> {
  static const bstm_data_type bstm_type = BSTM_MOG3_GREY_16;
};
template <> struct boxm2_to_bstm_type<BOXM2_MOG6_VIEW> {
  static const bstm_data_type bstm_type = BSTM_MOG6_VIEW;
};
template <> struct boxm2_to_bstm_type<BOXM2_NUM_OBS_VIEW> {
  static const bstm_data_type bstm_type = BSTM_NUM_OBS_VIEW;
};
template <> struct boxm2_to_bstm_type<BOXM2_NUM_OBS_VIEW_COMPACT> {
  static const bstm_data_type bstm_type = BSTM_NUM_OBS_VIEW_COMPACT;
};
template <> struct boxm2_to_bstm_type<BOXM2_MOG6_VIEW_COMPACT> {
  static const bstm_data_type bstm_type = BSTM_MOG6_VIEW_COMPACT;
};
template <> struct boxm2_to_bstm_type<BOXM2_GAUSS_RGB> {
  static const bstm_data_type bstm_type = BSTM_GAUSS_RGB;
};
template <> struct boxm2_to_bstm_type<BOXM2_GAUSS_RGB_VIEW> {
  static const bstm_data_type bstm_type = BSTM_GAUSS_RGB_VIEW;
};
template <> struct boxm2_to_bstm_type<BOXM2_GAUSS_RGB_VIEW_COMPACT> {
  static const bstm_data_type bstm_type = BSTM_GAUSS_RGB_VIEW_COMPACT;
};
template <> struct boxm2_to_bstm_type<BOXM2_NUM_OBS_SINGLE> {
  static const bstm_data_type bstm_type = BSTM_NUM_OBS_SINGLE;
};
template <> struct boxm2_to_bstm_type<BOXM2_NUM_OBS> {
  static const bstm_data_type bstm_type = BSTM_NUM_OBS;
};
template <> struct boxm2_to_bstm_type<BOXM2_RAY_DIR> {
  static const bstm_data_type bstm_type = BSTM_RAY_DIR;
};
template <> struct boxm2_to_bstm_type<BOXM2_POINT> {
  static const bstm_data_type bstm_type = BSTM_POINT;
};
template <> struct boxm2_to_bstm_type<BOXM2_AUX0> {
  static const bstm_data_type bstm_type = BSTM_AUX0;
};
template <> struct boxm2_to_bstm_type<BOXM2_AUX1> {
  static const bstm_data_type bstm_type = BSTM_AUX1;
};
template <> struct boxm2_to_bstm_type<BOXM2_AUX2> {
  static const bstm_data_type bstm_type = BSTM_AUX2;
};
template <> struct boxm2_to_bstm_type<BOXM2_AUX3> {
  static const bstm_data_type bstm_type = BSTM_AUX3;
};
template <> struct boxm2_to_bstm_type<BOXM2_UNKNOWN> {
  static const bstm_data_type bstm_type = BSTM_UNKNOWN;
};

template <bstm_data_type BSTM_TYPE> struct bstm_to_boxm2_type;

template <> struct bstm_to_boxm2_type<BSTM_ALPHA> {
  static const boxm2_data_type boxm2_type = BOXM2_ALPHA;
};
template <> struct bstm_to_boxm2_type<BSTM_MOG3_GREY> {
  static const boxm2_data_type boxm2_type = BOXM2_MOG3_GREY;
};
template <> struct bstm_to_boxm2_type<BSTM_MOG3_GREY_16> {
  static const boxm2_data_type boxm2_type = BOXM2_MOG3_GREY_16;
};
template <> struct bstm_to_boxm2_type<BSTM_MOG6_VIEW> {
  static const boxm2_data_type boxm2_type = BOXM2_MOG6_VIEW;
};
template <> struct bstm_to_boxm2_type<BSTM_NUM_OBS_VIEW> {
  static const boxm2_data_type boxm2_type = BOXM2_NUM_OBS_VIEW;
};
template <> struct bstm_to_boxm2_type<BSTM_NUM_OBS_VIEW_COMPACT> {
  static const boxm2_data_type boxm2_type = BOXM2_NUM_OBS_VIEW_COMPACT;
};
template <> struct bstm_to_boxm2_type<BSTM_MOG6_VIEW_COMPACT> {
  static const boxm2_data_type boxm2_type = BOXM2_MOG6_VIEW_COMPACT;
};
template <> struct bstm_to_boxm2_type<BSTM_GAUSS_RGB> {
  static const boxm2_data_type boxm2_type = BOXM2_GAUSS_RGB;
};
template <> struct bstm_to_boxm2_type<BSTM_GAUSS_RGB_VIEW> {
  static const boxm2_data_type boxm2_type = BOXM2_GAUSS_RGB_VIEW;
};
template <> struct bstm_to_boxm2_type<BSTM_GAUSS_RGB_VIEW_COMPACT> {
  static const boxm2_data_type boxm2_type = BOXM2_GAUSS_RGB_VIEW_COMPACT;
};
template <> struct bstm_to_boxm2_type<BSTM_NUM_OBS_SINGLE> {
  static const boxm2_data_type boxm2_type = BOXM2_NUM_OBS_SINGLE;
};
template <> struct bstm_to_boxm2_type<BSTM_NUM_OBS> {
  static const boxm2_data_type boxm2_type = BOXM2_NUM_OBS;
};
template <> struct bstm_to_boxm2_type<BSTM_RAY_DIR> {
  static const boxm2_data_type boxm2_type = BOXM2_RAY_DIR;
};
template <> struct bstm_to_boxm2_type<BSTM_POINT> {
  static const boxm2_data_type boxm2_type = BOXM2_POINT;
};
template <> struct bstm_to_boxm2_type<BSTM_AUX0> {
  static const boxm2_data_type boxm2_type = BOXM2_AUX0;
};
template <> struct bstm_to_boxm2_type<BSTM_AUX1> {
  static const boxm2_data_type boxm2_type = BOXM2_AUX1;
};
template <> struct bstm_to_boxm2_type<BSTM_AUX2> {
  static const boxm2_data_type boxm2_type = BOXM2_AUX2;
};
template <> struct bstm_to_boxm2_type<BSTM_AUX3> {
  static const boxm2_data_type boxm2_type = BOXM2_AUX3;
};
template <> struct bstm_to_boxm2_type<BSTM_UNKNOWN> {
  static const boxm2_data_type boxm2_type = BOXM2_UNKNOWN;
};

#endif
