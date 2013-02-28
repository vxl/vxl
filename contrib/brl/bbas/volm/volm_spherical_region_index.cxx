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

volm_spherical_region_index::
volm_spherical_region_index(vcl_string index_file_path,
                            vcl_string usph_file_path)
{
  vcl_vector<unsigned char> cdata;
  vsph_utils::read_ray_index_data(index_file_path, cdata);
  data_.clear();
  int n = cdata.size();
  for (int i = 0; i<n; ++i)
    data_.push_back(static_cast<double>(cdata[i]));

  load_unitsphere(usph_file_path);
  double dpr = vnl_math::deg_per_rad;
  double sigma = (0.1*usph_->point_angle())/dpr,  c =0.0;
  int min_size = 8;
  bool dosmoothing = false;
  seg = new vsph_segment_sphere(*usph_.ptr(),  c, min_size,sigma,dosmoothing);
  seg->set_data(data_);
  seg->segment();
  seg->extract_region_bounding_boxes();
  const vcl_map<int, vsph_sph_box_2d>& boxes = seg->region_boxes();
  vcl_map<int, vsph_sph_box_2d>::const_iterator bit = boxes.begin();

  for (; bit != boxes.end(); ++bit) {
    volm_spherical_region r(bit->second);
    r.set_attribute(spherical_region_attributes::ORIENTATION,(unsigned char) seg->region_median(bit->first));
    sph_regions_.add_region(r);
  }

}
void volm_spherical_region_index::load_unitsphere(vcl_string usph_file_path)
{
  vsl_b_ifstream is(usph_file_path);
  if(!is)
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

