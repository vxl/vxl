#include <iostream>
#include <cmath>
#include <volm/volm_vrml_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsph/vsph_unit_sphere.h>
#include <vgl/vgl_point_3d.h>

void volm_vrml_io::display_spherical_region_by_attrbute(std::ostream& str ,
                                                        volm_spherical_region & region,
                                                        spherical_region_attributes att,
                                                        double factor,                // this mis multipled by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                        double tol)
{
    float r,g,b ;
    unsigned char val;
    if (!region.attribute_value(att,val))
        return;

    r = volm_vrml_io_data::color_codes[(val)%8 ][0];
    g = volm_vrml_io_data::color_codes[(val)%8][1];
    b = volm_vrml_io_data::color_codes[(val)%8][2];
    region.bbox_ref().display_box(str,r,g,b,tol,factor);

}

void volm_vrml_io::display_spherical_region_layer_by_attrbute(std::ostream& str ,
                                                           volm_spherical_regions_layer layer,
                                                           spherical_region_attributes att,
                                                           double factor,                //: this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                           double tol)
{
    std::map<unsigned char, std::vector<unsigned int> > ids = layer.attributed_regions_by_type(att);
    std::vector<volm_spherical_region> regions = layer.regions();
    for (auto & id : ids)
        for (unsigned i = 0; i < id.second.size(); ++i)
             volm_vrml_io::display_spherical_region_by_attrbute(str,regions[id.second[i]],att,factor,tol);


}

void volm_vrml_io::display_segment_sphere(std::ostream& str ,
                                          vsph_segment_sphere * sph,
                                          double  /*factor*/)
{
    auto rit = sph->regions().begin();
    vsph_unit_sphere usph = sph->unit_sphere();
    std::vector<vgl_vector_3d<double> > unit_vectors = usph.cart_vectors();
    std::vector<vgl_vector_3d<double> > points;
    std::vector<vgl_point_3d<double> > colors;
    for (; rit != sph->regions().end(); ++rit) {
        const std::vector<int>& pt_ids = rit->second;
        auto val =(unsigned char) std::floor(sph->region_median(rit->first));

        vgl_point_3d<double> pc((double) volm_vrml_io_data::color_codes[(val)%8][0], (double) volm_vrml_io_data::color_codes[(val)%8][1], (double) volm_vrml_io_data::color_codes[(val)%8][2]);
        if(val > 100)
        {
            pc.x()=0.5;
            pc.y()=1.0;
            pc.z()=1.0;
        }
        for(int pt_id : pt_ids)
        {
            points.push_back(unit_vectors[pt_id]);
            colors.push_back(pc);
        }
    }

    str<<"Transform {\n"
        <<"translation 0 0  0\n"
        <<"children [\n"
        <<"Shape {\n"
        <<"geometry PointSet {\n"
        <<"coord Coordinate {\n"
        <<"point [\n";

    for(auto & point : points)
        str<<point.x()<<" "<<point.y()<<" "<<point.z()<<",\n";

    str<<"       ]\n"
       <<"      }\n"
       <<"color Color {\n"
       <<" color [ \n";
    for(auto & color : colors)
        str<<color.x()<<" "<<color.y()<<" "<<color.z()<<",\n";

    str<<"                    ]\n"
       <<"         }\n"
       <<"     }\n"
       <<" }\n"
       <<"]\n"
       <<"}\n";
}
