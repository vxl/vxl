#ifndef volm_vrml_io_h_
#define volm_vrml_io_h_
//:
// \file

#include <volm/volm_spherical_region.h>
#include <vcl_iosfwd.h>
#include <vsph/vsph_segment_sphere.h>

namespace volm_vrml_io_data
{
    static float color_codes[8][3] ={ {1.0f,0.5f,0.5f},
                                      {0.0f,1.0f,0.0f},
                                      {1.0f,0.0f,1.0f},
                                      {1.0f,1.0f,0.0f},
                                      {1.0f,0.5f,1.0f},
                                      {1.0f,0.0f,0.0f},
                                      {0.5f,0.5f,1.0f},
                                      {0.0f,0.0f,1.0f} };   
}

class volm_vrml_io
{
 public:

    //: currently displays bounding box
    static void display_spherical_region_by_attrbute(vcl_ostream& str ,
                                                     volm_spherical_region & r,
                                                     spherical_region_attributes att,
                                                     double factor,                //: this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                     double tol);

    static void display_spherical_region_layer_by_attrbute(vcl_ostream& str ,
                                                           volm_spherical_regions_layer r,
                                                           spherical_region_attributes att,
                                                           double factor,                //: this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                           double tol);
    static void display_segment_sphere(vcl_ostream& str ,
                                          vsph_segment_sphere * sph,
                                          double factor=1.0);
};

#endif
