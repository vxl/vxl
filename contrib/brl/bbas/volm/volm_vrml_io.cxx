#include "volm_vrml_io.h"

void volm_vrml_io::display_spherical_region_by_attrbute(vcl_ostream& str ,
                                                        volm_spherical_region & region,
                                                        spherical_region_attributes att,
                                                        double factor,  // this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                        double tol)
{
    float r,g,b ;
    unsigned char val;
    if (!region.attribute_value(att,val))
        return;

    r = volm_vrml_io_data::color_codes[val%8][0];
    g = volm_vrml_io_data::color_codes[val%8][1];
    b = volm_vrml_io_data::color_codes[val%8][2];

    region.bbox_ref().display_box(str,r,g,b,tol,factor);
}

void volm_vrml_io::display_spherical_region_layer_by_attrbute(vcl_ostream& str ,
                                                              volm_spherical_regions_layer const& layer,
                                                              spherical_region_attributes att,
                                                              double factor,  // this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                              double tol)
{
    vcl_vector<unsigned int> ids = layer.attributed_regions(att);
    vcl_vector<volm_spherical_region> regions = layer.regions();
    for (unsigned i = 0 ; i < ids.size(); i++)
        volm_vrml_io::display_spherical_region_by_attrbute(str,regions[ids[i]],att,factor,tol);
}
