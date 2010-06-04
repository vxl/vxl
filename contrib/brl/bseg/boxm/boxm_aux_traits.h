#ifndef boxm_aux_traits_h_
#define boxm_aux_traits_h_
//:
// \file

#include <vcl_string.h>
#include <boxm/sample/boxm_vis_implicit_sample.h>
#include <boxm/sample/boxm_update_sample.h>

enum boxm_aux_type
{
  BOXM_AUX_VIS_IMPLICIT,
  BOXM_AUX_UPDATE,
  BOXM_AUX_OPT_GREY,
  BOXM_AUX_OPT_RT_GREY,
  BOXM_AUX_OPT_RGB,
  BOXM_AUX_OPT_RT_RGB,
  BOXM_AUX_NULL,
  BOXM_AUX_UNKNOWN,
  BOXM_AUX_OPT2_GREY,
  BOXM_AUX_OPT2_RGB,
  BOXM_AUX_OPT3_MOG_GREY,
  BOXM_AUX_OPT3_MOG_RGB,
  BOXM_AUX_SCALAR_SAMPLE
};

//: sample properties for templates.
template <boxm_aux_type>
class boxm_aux_traits;

//: traits for a visibility-based implicit surface sample
template<>
class boxm_aux_traits<BOXM_AUX_VIS_IMPLICIT>
{
 public:
  typedef boxm_vis_implicit_sample sample_datatype;

  static vcl_string storage_subdir() { return "vis_implicit"; }
};

template<>
class boxm_aux_traits<BOXM_AUX_UPDATE>
{
 public:
  typedef boxm_update_sample sample_datatype;

  static vcl_string storage_subdir() { return "update_work";}
};

//: traits for a null sample
template<>
class boxm_aux_traits<BOXM_AUX_NULL>
{
 public:
  typedef float sample_datatype;

  static vcl_string storage_subdir() { return "null"; }
};
template<>
class boxm_aux_traits<BOXM_AUX_SCALAR_SAMPLE>
{
 public:
  typedef float sample_datatype;

  static vcl_string storage_subdir() { return "scalar"; }
};

template<>
class boxm_aux_traits<BOXM_AUX_SCALAR_SAMPLE>
{
 public:
  typedef float sample_datatype;

  static vcl_string storage_subdir() { return "scalar"; }
};
//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>
{
 public:
  typedef boxm_rt_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_grey_work"; }
};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_RGB>
{
 public:
  typedef boxm_rt_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_rgb_work"; }
};

#endif // boxm_aux_traits_h_
