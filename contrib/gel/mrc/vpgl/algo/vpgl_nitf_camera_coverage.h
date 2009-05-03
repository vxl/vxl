// This is gel/mrc/vpgl/algo/vpgl_nitf_camera_coverage.h
#ifndef vpgl_nitf_camera_coverage_h_
#define vpgl_nitf_camera_coverage_h_
//:
// \file
// \brief A set of algorithms related to the area covered by a nitf camera
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date August 08, 2008
//
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vgl/vgl_point_2d.h>
//:
// There are several algorithms useful when determining which geografical regions
// are covered by a set of cameras. This class contains a set of algorithms that
// provides such answer in different ways.
class vpgl_nitf_camera_coverage
{
 public:
   vpgl_nitf_camera_coverage() {}
  ~vpgl_nitf_camera_coverage() {}

 //: Saves to an input file, the filenames of the NITF images which contain the specified geografic position.
 //  The inputs to this function are: A vector of vgl_point_3d<double> with the
 //  geographic points of interest (x-longitude, y-latitud, z-elevation). The filename of a
 //  file containing a list of NITF images to go through. A filename to write the image for which
 //  the points projects within the image
  static bool coverage_list(vcl_vector<vgl_point_2d<double> > geo_pts,
                            vcl_string camera_list,
                            vcl_string img_coverage_list);

  //: Saves an image where the areas covered by the cameras on the "camera_list" file are colored as red
  static bool compute_coverage_region(vcl_string camera_list, vcl_string out_imfile);
};


#endif // vpgl_nitf_camera_coverage_h_
