#ifndef volm_vrml_io_h_
#define volm_vrml_io_h_
//:
// \file

#include <volm/volm_spherical_region.h>
#include <vcl_iosfwd.h>

static float color_codes[8][3] ={ {1.0,0.5,0.5},
                                  {0.0,1.0,0.0},
                                  {1.0,0.0,1.0},
                                  {1.0,1.0,0.0},
                                  {1.0,0.5,1.0},
                                  {1.0,0.0,0.0},
                                  {0.5,0.5,1.0},
                                  {0.0,0.0,1.0}};
class volm_vrml_io
{
 public:

    //: currently displays bounding box
    static void display_spherical_region_by_attrbute(vcl_ostream& str ,
                                                     volm_spherical_region & r,
                                                     spherical_region_attributes att,
                                                     double factor,                // this mis multipled by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                     double tol);

    static void display_spherical_region_layer_by_attrbute(vcl_ostream& str ,
                                                           volm_spherical_regions_layer & r,
                                                           spherical_region_attributes att,
                                                           double factor,                // this mis multipled by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                           double tol);
};

#endif
