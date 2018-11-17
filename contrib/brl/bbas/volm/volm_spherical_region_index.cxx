#include <iostream>
#include <vector>
#include "volm_spherical_region_index.h"
#include "volm_camera_space.h"
#include <vsph/vsph_unit_sphere.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include "volm_spherical_container.h"
#include <bsol/bsol_algs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsph/vsph_utils.h>
#include <vsph/vsph_segment_sphere.h>
#include <volm/volm_io.h>

volm_spherical_region_index::
volm_spherical_region_index(std::map<std::string,std::string> & index_file_paths,
                            const std::string& usph_file_path)
{
    auto iter = index_file_paths.begin();
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

        std::vector<unsigned char> cdata;
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
        const std::map<int, vsph_sph_box_2d>& boxes = seg_->region_boxes();
        auto bit = boxes.begin();

        for (; bit != boxes.end(); ++bit) {
            volm_spherical_region r(bit->second);
            if (keep_sky && (unsigned char) seg_->region_median(bit->first) == 254)
                r.set_attribute(SKY,(unsigned char) 254);
            else
            {
                if (att==ORIENTATION)
                {
                    auto ival = (unsigned char) seg_->region_median(bit->first);
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
        auto ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(DEPTH_INTERVAL,ival);
        sph_regions_.add_region(r);
    }
        for (int i = 0 ; i < num_orientation_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
       box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
        volm_spherical_region r(box2d);
        auto ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(ORIENTATION,ival);
        sph_regions_.add_region(r);
    }
    for (int i = 0 ; i < num_nlcd_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
        box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
       volm_spherical_region r(box2d);
        auto ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(NLCD,ival);
        sph_regions_.add_region(r);
    }
    for (int i = 0 ; i < sky_regions; ++i,++count)
    {
        vsph_sph_box_2d box2d;
        box2d.set(boxes[count*6+3],boxes[count*6+4],this->check_phi_bounds(boxes[count*6+0]),this->check_phi_bounds(boxes[count*6+1]),this->check_phi_bounds(boxes[count*6+2]));
        volm_spherical_region r(box2d);
        auto ival = (unsigned char) boxes[count*6+5];
        r.set_attribute(SKY,ival);
        sph_regions_.add_region(r);
    }
}

volm_spherical_region_index::volm_spherical_region_index(std::map<std::string,std::vector<unsigned char> > & index_buffers,
                                                         vsph_unit_sphere_sptr & usph)
{
    auto iter = index_buffers.begin();
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
        std::vector<unsigned char> cdata = iter->second;
        std::cout<<"Size of the data is "<<cdata.size()<<std::endl;
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
        const std::map<int, vsph_sph_box_2d> boxes = seg_->region_boxes();
        auto bit = boxes.begin();

        for (; bit != boxes.end(); ++bit) {
            volm_spherical_region r(bit->second);
            if (keep_sky && (unsigned char) seg_->region_median(bit->first) == 254)
                r.set_attribute(SKY,(unsigned char) 254);
            else if ( (unsigned char) seg_->region_median(bit->first) < 253 )
            {
                if (att==ORIENTATION)
                {
                    auto ival = (unsigned char) seg_->region_median(bit->first);
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

void volm_spherical_region_index::load_unitsphere(const std::string& usph_file_path)
{
  vsl_b_ifstream is(usph_file_path);
  if (!is)
  {
    std::cout<<"Cannot Open file "<<usph_file_path<<std::endl;
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

void volm_spherical_region_index::print(std::ostream& os)
{
  std::vector<volm_spherical_region> regions = sph_regions_.regions();
  auto iter =  regions.begin();
  for (; iter != regions.end(); ++iter) {
    iter->print(os);
  }
  std::cout << '\n';
}

void volm_spherical_region_index::write_binary(std::ofstream & oconfig,std::ofstream & odata)
{
    std::vector<volm_spherical_region> regions = sph_regions_.regions();
    std::vector<unsigned int> depth_regions =sph_regions_.attributed_regions_by_type_only(DEPTH_INTERVAL);

    int num_regions[5];
    num_regions[1] =depth_regions.size();
    unsigned char ival ;
    for (unsigned int depth_region : depth_regions)
    {
        volm_spherical_region r = regions[depth_region];
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

    std::vector<unsigned int> orientation_regions =sph_regions_.attributed_regions_by_type_only(ORIENTATION);
    num_regions[2] =orientation_regions.size();
    for (unsigned int orientation_region : orientation_regions)
    {
        volm_spherical_region r = regions[orientation_region];
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
    std::vector<unsigned int> nlcd_regions =sph_regions_.attributed_regions_by_type_only(NLCD);
    num_regions[3] =nlcd_regions.size();
    for (unsigned int nlcd_region : nlcd_regions)
    {
        volm_spherical_region r = regions[nlcd_region];
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
    std::vector<unsigned int> sky_regions =sph_regions_.attributed_regions_by_type_only(SKY);
    num_regions[4] =sky_regions.size();
    for (unsigned int sky_region : sky_regions)
    {
        volm_spherical_region r = regions[sky_region];
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
= default;
