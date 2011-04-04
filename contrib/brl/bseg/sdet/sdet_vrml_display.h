// This is brl/bseg/sdet/sdet_vrml_display.h
#ifndef sdet_vrml_display_h_
#define sdet_vrml_display_h_
//:
// \file
// \brief write out segmentation data as a vrml display
// \author Author J. L. Mundy - August 17, 2008
// \verbatim
// \endverbatim
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vil/vil_image_view.h>
class sdet_vrml_display
{
 public:
  static void write_vrml_header(vcl_ofstream& str);
  static void write_intensity_regions_3d(vcl_ofstream& str,
                                         vcl_vector<vtol_intensity_face_sptr> const& faces);
  static void write_vsol_polys_3d(vcl_ofstream& str,
                                  vcl_vector<vsol_polygon_3d_sptr> const& polys);
  static void write_vrml_height_map(vcl_ofstream& str,
                                    vil_image_view<float> const & z_of_xy,
                                    float r = 0.0, float g = 1.0, float b = 0.0);
 private:
  sdet_vrml_display();

};

#endif // sdet_vrml_display_h_
