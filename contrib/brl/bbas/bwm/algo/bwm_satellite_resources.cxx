#include "bwm_satellite_resources.h"

#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <volm/volm_tile.h>
#include <volm/volm_geo_index2.h>

void add_directories(vcl_string root, vcl_vector<vcl_string>& directories) {
  if (vul_file::is_directory(root))
    directories.push_back(root);
  
  vcl_string glob = root + "/*"; // get everything directory or not
  vul_file_iterator file_it(glob.c_str());
  ++file_it;  // skip .
  ++file_it;  // skip ..
  while (file_it) {
    vcl_string name(file_it());
    if (vul_file::is_directory(name))
      add_directories(name, directories);
    ++file_it;
  }

}

void bwm_satellite_resources::add_resource(vcl_string name)
{
  bwm_satellite_resource res;
  res.full_path_ = name;
  res.name_ = vul_file::strip_directory(name);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata(name, "");
  resources_.push_back(res);
}

bwm_satellite_resources::bwm_satellite_resources(vcl_string path, 
                                                 double lower_left_lat, 
                                                 double lower_left_lon, 
                                                 double upper_right_lat, 
                                                 double upper_right_lon)
{
  vcl_vector<vcl_string> directories;
  add_directories(path, directories);
  if (!directories.size())
    return;
  vcl_cout << "found " << directories.size() << " directories!\n"; 
  
  for (unsigned i = 0; i < directories.size(); i++) {
    vcl_string glob = directories[i] + "/*.NTF";
    vul_file_iterator file_it(glob.c_str());
    while (file_it) {
      vcl_string name(file_it());
      //vcl_cout << name << "\n";
      this->add_resource(name);    
      ++file_it;
    }  
    glob = directories[i] + "/*.ntf";
    vul_file_iterator file_it2(glob.c_str());
    while (file_it2) {
      vcl_string name(file_it2());
      //vcl_cout << name << "\n";
      this->add_resource(name);    
      ++file_it2;
    }
  }
  
  // construct volm_geo_index2 quad tree with 1.0 degree leaves - satellite images are pretty large, so the leaves need to be large
  vgl_box_2d<double> bbox(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
  root_ = volm_geo_index2::construct_tree<vcl_vector<unsigned> >(bbox, 1.0f);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root_, leaves);
  vcl_cout << " the number of leaves in the quad tree of satellite resources: " << leaves.size() << '\n';

  // insert the ids of the resources
  for (unsigned i = 0; i < resources_.size(); i++) {
    vcl_vector<volm_geo_index2_node_sptr> leaves;
    double xmin = resources_[i].meta_->lower_left_[1];  // lon
    double ymin = resources_[i].meta_->lower_left_[0];  // lat
    double xmax = resources_[i].meta_->upper_right_[1];
    double ymax = resources_[i].meta_->upper_right_[0];
    vgl_box_2d<double> satellite_footprint(xmin, xmax, ymin, ymax);
    volm_geo_index2::get_leaves(root_, leaves, satellite_footprint);
    for (unsigned j = 0; j < leaves.size(); j++) {
      volm_geo_index2_node<vcl_vector<unsigned> >* leaf_ptr = dynamic_cast<volm_geo_index2_node<vcl_vector<unsigned> >* >(leaves[j].ptr());
      leaf_ptr->contents_.push_back(i);  // push this satellite image to this leave that intersects its footprint
    }
  }

}

//dummy vsl io functions to allow bwm_satellite_resources to be inserted into
//brdb as a dbvalue
void vsl_b_write(vsl_b_ostream & os, bwm_satellite_resources const &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream & is, bwm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream &os, const bwm_satellite_resources &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, bwm_satellite_resources* tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const bwm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const bwm_satellite_resources* &tc)
{ /* do nothing */ }
void vsl_b_read(vsl_b_istream& is, bwm_satellite_resources_sptr& tc)
{ /* do nothing */ }
void vsl_b_write(vsl_b_ostream& os, const bwm_satellite_resources_sptr &tc)
{ /* do nothing */ }
void vsl_print_summary(vcl_ostream& os, const bwm_satellite_resources_sptr &tc)
{ /* do nothing */ }

