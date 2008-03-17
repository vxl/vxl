// This is brl/bseg/bvxm/bvxm_voxel_traits.h
#ifndef bvxm_voxel_traits_h_
#define bvxm_voxel_traits_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Templated component for voxel datatype details
//
// In most cases it is probably better to use vil_pixel_format.
//
// \author Isabel Restrepo
// \date   February 24, 2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//-----------------------------------------------------------------------------
#include "bvxm_mog_grey_processor.h"
#include "bvxm_mog_rgb_processor.h"

enum bvxm_voxel_type
{
  OCCUPANCY = 0,
  APM_MOG_GREY,
  APM_MOG_RGB,
  EDGES,
  UNKNOWN,
};


//: Pixel properties for templates.
template <bvxm_voxel_type>
class bvxm_voxel_traits;


template<>
class bvxm_voxel_traits<OCCUPANCY>
{
 public:
  //:Datatype of the occupancy probabilities
  typedef float voxel_datatype;

  static inline vcl_string filename_prefix(){ return "ocp"; }
  static inline bool is_multibin() { return false; }
  static inline voxel_datatype initial_val() { return 0.01f; }
};

template<>
class bvxm_voxel_traits<APM_MOG_GREY>
{
 public:
  //:Datatype of the occupancy probabilities
  typedef bvxm_mog_grey_processor appearance_processor;
  typedef bvxm_mog_grey_processor::apm_datatype voxel_datatype;
  typedef bvxm_mog_grey_processor::obs_datatype obs_datatype;
  typedef bvxm_mog_grey_processor::obs_mathtype obs_mathtype;

  static inline vcl_string filename_prefix() { return "apm_mog_grey"; }
  static inline bool is_multibin() { return true; }
  static inline voxel_datatype initial_val()
  {
    voxel_datatype init_val;
    return init_val;
  }
};


template<>
class bvxm_voxel_traits<APM_MOG_RGB>
{
 public:
  //:Datatype of the occupancy probabilities
  typedef bvxm_mog_rgb_processor appearance_processor;
  typedef bvxm_mog_rgb_processor::apm_datatype voxel_datatype;
  typedef bvxm_mog_rgb_processor::obs_datatype obs_datatype;
  typedef bvxm_mog_rgb_processor::obs_mathtype obs_mathtype;

  static inline vcl_string filename_prefix(){ return "apm_mog_rgb"; }
  static inline bool is_multibin() { return true; }
  static inline voxel_datatype initial_val()
  {
    voxel_datatype init_val;
    return init_val;
  }
};

template<>
class bvxm_voxel_traits<EDGES>
{
 public:
  //:Datatype of the occupancy probabilities
  typedef float voxel_datatype;

  static inline vcl_string filename_prefix() { return "edges"; }
  static inline bool is_multibin() { return false; }
  static inline voxel_datatype initial_val() { return 1.0f; }
};

#endif // bvxm_voxel_traits_h_
