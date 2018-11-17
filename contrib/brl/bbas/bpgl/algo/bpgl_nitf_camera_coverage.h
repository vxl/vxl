// This is bbas/bpgl/algo/bpgl_nitf_camera_coverage.h
#ifndef bpgl_nitf_camera_coverage_h_
#define bpgl_nitf_camera_coverage_h_
//:
// \file
// \brief A set of algorithms related to the area covered by a nitf camera
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date August 08, 2008
//
#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_2d.h>
//:
// There are several algorithms useful when determining which geographical regions
// are covered by a set of cameras. This class contains a set of algorithms that
// provides such answer in different ways.
class bpgl_nitf_camera_coverage
{
 public:
   bpgl_nitf_camera_coverage() = default;
  ~bpgl_nitf_camera_coverage() = default;

  //: Saves, to an input file, the filenames of the NITF images which contain the specified geographic position.
  //  The inputs to this function are:
  //   * A vector of vgl_point_3d<double> with the geographic points of interest (x-longitude, y-latitude, z-elevation).
  //   * The filename of a file containing a list of NITF images to go through.
  //   * A filename to write the image for which the points project within the image
  static bool coverage_list(std::vector<vgl_point_2d<double> > geo_pts,
                            const std::string& camera_list,
                            const std::string& img_coverage_list);

  //: Saves an image where the areas covered by the cameras on the "camera_list" file are colored as red
  static bool compute_coverage_region(const std::string& camera_list, const std::string& out_imfile);
};


#endif // bpgl_nitf_camera_coverage_h_
