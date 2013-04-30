#include "volm_spherical_region_index.h"
#include "volm_camera_space.h"
#include <vsph/vsph_unit_sphere.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include "volm_spherical_container.h"
#include <bsol/bsol_algs.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vsph/vsph_utils.h>
#include <vsph/vsph_segment_sphere.h>
#include <volm/volm_io.h>

volm_spherical_region_index::
volm_spherical_region_index(vcl_map<vcl_string,vcl_string> & index_file_paths,
                            vcl_string usph_file_path)
{
    vcl_map<vcl_string,vcl_string>::iterator iter = index_file_paths.begin();
    for (;iter!=index_file_paths.end(); iter++)
    {
        bool keep_sky = false;
        spherical_region_attributes att;
        if (iter->first=="DEPTH_INTERVAL")
        {
            keep_sky = true;
            att = DEPTH_INTERVAL;
        }
        else if (iter->first=="ORIENTATION")
        {
            keep_sky = false;
            att = ORIENTATION;
        }
        else if (iter->first=="NLCD")
        {
             keep_sky = false;
             att = NLCD;
        }
        else
            continue;

        vcl_vector<unsigned char> cdata;
        vsph_utils::read_ray_index_data(iter->second, cdata);
        data_.clear();
        int n = cdata.size();
        for (int i = 0; i<n; ++i)
          data_.push_back(static_cast<double>(cdata[i]));

        load_unitsphere(usph_file_path);
        double dpr = vnl_math::deg_per_rad;
        double sigma = (0.1*usph_->point_angle())/dpr,  c =0.0;
        int min_size = 3;
        bool dosmoothing = true;
        seg_ = new vsph_segment_sphere(*usph_.ptr(),  c, min_size,sigma,dosmoothing);
        seg_->set_data(data_);
        seg_->segment();
        seg_->extract_region_bounding_boxes();
        const vcl_map<int, vsph_sph_box_2d>& boxes = seg_->region_boxes();
        vcl_map<int, vsph_sph_box_2d>::const_iterator bit = boxes.begin();

        for (; bit != boxes.end(); ++bit) {
            volm_spherical_region r(bit->second);
            if (keep_sky && (unsigned char) seg_->region_median(bit->first) == 254)
                r.set_attribute(SKY,(unsigned char) 254);
            else
            {
                if (att==ORIENTATION)
                {
                    unsigned char ival = (unsigned char) seg_->region_median(bit->first);
                    if (ival >=2 && ival <=9)
                        r.set_attribute(att,(unsigned char) 2);
                    else if (ival == 1)
                        r.set_attribute(att,ival);
                }
                else
                    r.set_attribute(att,(unsigned char) seg_->region_median(bit->first));
            }
            sph_regions_.add_region(r);
        }
    }
}

float volm_spherical_region_index::check_phi_bounds(float  phi)
{
    double pye = vnl_math::pi;
    double two_pye = 2* vnl_math::pi;
    if (phi<-pye) phi += two_pye;
    if (phi> pye) phi -= two_pye;

    return phi;
}

volm_spherical_region_index::volm_spherical_region_index(float * boxes,int num_depth_regions, int num_orientation_regions, int num_nlcd_regions, int sky_regions)
{
    unsigned count = 0;
    for (int i = 0 ; i < num_depth_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;

        box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
        volm_spherical_region r(box2d);
        unsigned char ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(DEPTH_INTERVAL,ival);
        sph_regions_.add_region(r);
    }
        for (int i = 0 ; i < num_orientation_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
       box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
        volm_spherical_region r(box2d);
        unsigned char ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(ORIENTATION,ival);
        sph_regions_.add_region(r);
    }
    for (int i = 0 ; i < num_nlcd_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
        box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
       volm_spherical_region r(box2d);
        unsigned char ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(NLCD,ival);
        sph_regions_.add_region(r);
    }
    for (int i = 0 ; i < sky_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
        box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
        volm_spherical_region r(box2d);
        unsigned char ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(SKY,ival);
        sph_regions_.add_region(r);
    }
}

volm_spherical_region_index::volm_spherical_region_index(vcl_map<vcl_string,vcl_vector<unsigned char> > & index_buffers,
                                                         vsph_unit_sphere_sptr & usph)
{
    vcl_map<vcl_string,vcl_vector<unsigned char> >::iterator iter = index_buffers.begin();
    for (;iter!=index_buffers.end(); iter++)
    {
        bool keep_sky = false;
        spherical_region_attributes att;
        if (iter->first=="DEPTH_INTERVAL")
        {
            keep_sky = true;
            att = DEPTH_INTERVAL;
        }
        else if (iter->first=="ORIENTATION")
        {
            keep_sky = false;
            att = ORIENTATION;
        }
        else if (iter->first=="NLCD")
        {
            keep_sky = false;
            att = NLCD;
        }
        else
            continue;
        vcl_vector<unsigned char> cdata = iter->second;
        vcl_cout<<"Size of the data is "<<cdata.size()<<vcl_endl;
        data_.clear();
        int n = cdata.size();
        for (int i = 0; i<n; ++i)
            data_.push_back(static_cast<double>(cdata[i]));
        usph_ = usph;
        double dpr = vnl_math::deg_per_rad;
        double sigma = (0.1*usph_->point_angle())/dpr,  c =0.0;
        int min_size = 3;
        bool dosmoothing = false;
        seg_ = new vsph_segment_sphere(*usph_.ptr(),  c, min_size,sigma,dosmoothing);
        seg_->set_data(data_);
        seg_->segment();
        seg_->extract_region_bounding_boxes();
        const vcl_map<int, vsph_sph_box_2d> boxes = seg_->region_boxes();
        vcl_map<int, vsph_sph_box_2d>::const_iterator bit = boxes.begin();

        for (; bit != boxes.end(); ++bit) {
            volm_spherical_region r(bit->second);
            if (keep_sky && (unsigned char) seg_->region_median(bit->first) == 254)
                r.set_attribute(SKY,(unsigned char) 254);
            else if ( (unsigned char) seg_->region_median(bit->first) < 253 )
            {
                if (att==ORIENTATION)
                {
                    unsigned char ival = (unsigned char) seg_->region_median(bit->first);
                    if (ival >=2 && ival <=9)
                        r.set_attribute(att,(unsigned char) 2);
                    else if (ival == 1)
                        r.set_attribute(att,ival);
                }
                else
                    r.set_attribute(att,(unsigned char) seg_->region_median(bit->first));
            }
            sph_regions_.add_region(r);
        }
        delete seg_;
    }
}

void volm_spherical_region_index::load_unitsphere(vcl_string usph_file_path)
{
  vsl_b_ifstream is(usph_file_path);
  if (!is)
  {
    vcl_cout<<"Cannot Open file "<<usph_file_path<<vcl_endl;
    return;
  }
  vsl_b_read(is, usph_);
}

void volm_spherical_region_index::construct_spherical_regions()
{
}

volm_spherical_regions_layer
volm_spherical_region_index::index_regions()
{
  return sph_regions_;
}

void volm_spherical_region_index::print(vcl_ostream& os)
{
  vcl_vector<volm_spherical_region> regions = sph_regions_.regions();
  vcl_vector<volm_spherical_region>::iterator iter =  regions.begin();
  for (; iter != regions.end(); ++iter) {
    iter->print(os);
  }
  vcl_cout << '\n';
}

void volm_spherical_region_index::write_binary(vcl_ofstream & oconfig,vcl_ofstream & odata)
{
    vcl_vector<volm_spherical_region> regions = sph_regions_.regions();
    vcl_vector<unsigned int> depth_regions =sph_regions_.attributed_regions_by_type_only(DEPTH_INTERVAL);

    int num_regions[5];
    num_regions[1] =depth_regions.size();
    unsigned char ival ;
    for (unsigned i = 0 ; i < depth_regions.size();i++)
    {
        volm_spherical_region r = regions[depth_regions[i]];
        float vals[6];
        vals[0] = r.bbox_ref().a_phi();
        vals[1] = r.bbox_ref().b_phi();
        vals[2] = r.bbox_ref().c_phi();
        vals[3] = r.bbox_ref().min_theta();
        vals[4] = r.bbox_ref().max_theta();
        r.attribute_value(DEPTH_INTERVAL,ival);
        vals[5] = (float) ival;

        odata.write(reinterpret_cast<char*>(&vals[0]),sizeof(float)*6);
    }

    vcl_vector<unsigned int> orientation_regions =sph_regions_.attributed_regions_by_type_only(ORIENTATION);
    num_regions[2] =orientation_regions.size();
    for (unsigned i = 0 ; i < orientation_regions.size();i++)
    {
        volm_spherical_region r = regions[orientation_regions[i]];
        float vals[6];
        vals[0] = r.bbox_ref().a_phi();
        vals[1] = r.bbox_ref().b_phi();
        vals[2] = r.bbox_ref().c_phi();
        vals[3] = r.bbox_ref().min_theta();
        vals[4] = r.bbox_ref().max_theta();
        r.attribute_value(ORIENTATION,ival);
        vals[5] = (float) ival;

        odata.write(reinterpret_cast<char*>(&vals[0]),sizeof(float)*6);
    }
    vcl_vector<unsigned int> nlcd_regions =sph_regions_.attributed_regions_by_type_only(NLCD);
    num_regions[3] =nlcd_regions.size();
    for (unsigned i = 0 ; i < nlcd_regions.size();i++)
    {
        volm_spherical_region r = regions[nlcd_regions[i]];
        float vals[6];
        vals[0] = r.bbox_ref().a_phi();
        vals[1] = r.bbox_ref().b_phi();
        vals[2] = r.bbox_ref().c_phi();
        vals[3] = r.bbox_ref().min_theta();
        vals[4] = r.bbox_ref().max_theta();
        r.attribute_value(NLCD,ival);
        vals[5] = (float) ival;

        odata.write(reinterpret_cast<char*>(&vals[0]),sizeof(float)*6);
    }
    vcl_vector<unsigned int> sky_regions =sph_regions_.attributed_regions_by_type_only(SKY);
    num_regions[4] =sky_regions.size();
    for (unsigned i = 0 ; i < sky_regions.size();i++)
    {
        volm_spherical_region r = regions[sky_regions[i]];
        float vals[6];
        vals[0] = r.bbox_ref().a_phi();
        vals[1] = r.bbox_ref().b_phi();
        vals[2] = r.bbox_ref().c_phi();
        vals[3] = r.bbox_ref().min_theta();
        vals[4] = r.bbox_ref().max_theta();
        r.attribute_value(SKY,ival);
        vals[5] = (float) ival;
        vals[5] = (float) 1;

        odata.write(reinterpret_cast<char*>(&vals[0]),sizeof(float)*6);
    }

    num_regions[0] = num_regions[1]+num_regions[2]+num_regions[3]+num_regions[4];
    oconfig.write(reinterpret_cast<char*>(&num_regions[0]),sizeof(int)*5);
}

volm_spherical_region_index::
~volm_spherical_region_index()
{
}
