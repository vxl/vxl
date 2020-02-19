// This is gel/vifa/vifa_parallel.h
#ifndef VIFA_PARALLEL_H
#define VIFA_PARALLEL_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Class for parallel line calculations.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vifa/vifa_histogram.h>
#include <vifa/vifa_parallel_params.h>
#include <vifa/vifa_typedefs.h>


//: Compute the parallel-ness of a set of line segments
// NOTE: All angles are in degrees.
class vifa_parallel: public vifa_parallel_params
{
 private:
  //: Raw histogram
  vifa_histogram*  raw_h_;

  //: Normalized histogram
  vifa_histogram*  norm_h_;

 public:
  //: Face-based constructor
  vifa_parallel(iface_list&      faces,
                bool             contrast_weighted,
                vifa_parallel_params*  params = nullptr
               );

  //: Orientation-based constructor
  vifa_parallel(std::vector<float>&  pixel_orientations,
                vifa_parallel_params*  params = nullptr
               );

  //: Constructor based on line statistics
  vifa_parallel(float  center_angle,
                float  std_dev
               );

  //: Destructor
  ~vifa_parallel() override;

  void reset();

  vifa_histogram *get_raw_hist();
  vifa_histogram *get_norm_hist();

  void  map_gaussian(float&  max_angle,
                     float&  std_dev,
                     float&  scale
                    );
  void  remove_gaussian(float  max_angle,
                        float  std_dev,
                        float  scale
                       );
  void  snapshot(char* fname);
  float area();
  float bin_variance();

private:
  float           map_x(float  raw_x);
  vifa_histogram* normalize_histogram(vifa_histogram*  h);
  float           find_peak(float&  max_value);
  vtol_intensity_face_sptr get_adjacent_iface(const vtol_intensity_face_sptr& known_face,
                                              vtol_edge_2d*            e);
};


#endif  // VIFA_PARALLEL_H
