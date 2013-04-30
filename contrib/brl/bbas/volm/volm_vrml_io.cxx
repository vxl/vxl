#include <volm/volm_vrml_io.h>
#include <vcl_cmath.h>
#include <vsph/vsph_unit_sphere.h>
#include <vgl/vgl_point_3d.h>

void volm_vrml_io::display_spherical_region_by_attrbute(vcl_ostream& str ,
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

void volm_vrml_io::display_spherical_region_layer_by_attrbute(vcl_ostream& str ,
                                                           volm_spherical_regions_layer layer,
                                                           spherical_region_attributes att,
                                                           double factor,                //: this is multiplied by 1.0 and is used to display outwards or inwards of the unit sphere.
                                                           double tol)
{
    vcl_map<unsigned char, vcl_vector<unsigned int> > ids = layer.attributed_regions_by_type(att);
    vcl_vector<volm_spherical_region> regions = layer.regions();
    for (vcl_map<unsigned char, vcl_vector<unsigned int> >::iterator iter = ids.begin() ; iter!= ids.end(); ++iter)
        for (unsigned i = 0; i < iter->second.size(); ++i)
             volm_vrml_io::display_spherical_region_by_attrbute(str,regions[iter->second[i]],att,factor,tol);

 
}

void volm_vrml_io::display_segment_sphere(vcl_ostream& str ,
                                          vsph_segment_sphere * sph,
                                          double factor)
{
    vcl_map<int,  vcl_vector<int> >::const_iterator rit = sph->regions().begin();
    vsph_unit_sphere usph = sph->unit_sphere();
    vcl_vector<vgl_vector_3d<double> > unit_vectors = usph.cart_vectors();
    vcl_vector<vgl_vector_3d<double> > points;
    vcl_vector<vgl_point_3d<double> > colors;
    for (; rit != sph->regions().end(); ++rit) {
        const vcl_vector<int>& pt_ids = rit->second;
        unsigned char val =(unsigned char) vcl_floor(sph->region_median(rit->first));
        
        vgl_point_3d<double> pc((double) volm_vrml_io_data::color_codes[(val)%8][0], (double) volm_vrml_io_data::color_codes[(val)%8][1], (double) volm_vrml_io_data::color_codes[(val)%8][2]);
        if(val > 100)
        {
            pc.x()=0.5;
            pc.y()=1.0;
            pc.z()=1.0;
        }
        for(unsigned i = 0 ; i < pt_ids.size(); i++)
        {
            points.push_back(unit_vectors[pt_ids[i]]);
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
 
    for(unsigned i = 0; i < points.size(); i++)
        str<<points[i].x()<<" "<<points[i].y()<<" "<<points[i].z()<<",\n";

    str<<"       ]\n"
       <<"      }\n"
       <<"color Color {\n"
       <<" color [ \n";
    for(unsigned i = 0; i < colors.size(); i++)
        str<<colors[i].x()<<" "<<colors[i].y()<<" "<<colors[i].z()<<",\n"; 

    str<<"                    ]\n"
       <<"         }\n"
       <<"     }\n"
       <<" }\n"
       <<"]\n"
       <<"}\n";
}
