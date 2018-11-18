#include <iostream>
#include <algorithm>
#include "bvpl_edge3d_kernel_factory.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bxml/bxml_find.h>

// Default Constructor
bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory()
{
  min_x_ = 0; max_x_=0; min_y_=0; max_y_=0; min_z_=0; max_z_ = 0;
  angular_resolution_ = 0;
  rotation_axis_ = canonical_rotation_axis_;
  //parallel_axis_ = canonical_parallel_axis_;
  angle_ = 0.0f;
  voxel_length_ = 1.0;
}


bvpl_edge3d_kernel_factory::bvpl_edge3d_kernel_factory (int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, double voxel_length):
min_x_(min_x),max_x_(max_x),min_y_(min_y),max_y_(max_y),min_z_(min_z),max_z_(max_z)
{
  //Determine angular resolution based on size of kernel
  //If this was 2D, then the angular resolution would be 180/(2l -2) (Recusive Binary Dilation... Desikachari Nadadur)
  angular_resolution_=0;
  angle_ = 0.0f;
  voxel_length_ = voxel_length;
  rotation_axis_ = canonical_rotation_axis_;
  parallel_axis_ = canonical_parallel_axis_;
  create_canonical();
}


void bvpl_edge3d_kernel_factory::create_canonical()
{
  typedef vgl_point_3d<float> point_3d;
  typedef bvpl_kernel_dispatch dispatch;

  //count number of + and -, so that voxels weights can be normalized
  int n0=0;
  int n1=0;

  for (int x=min_x_; x<= max_x_; ++x)
    for (int y= min_y_; y<= max_y_; ++y)
      for (int z= min_z_; z<= max_z_; ++z)
      {
        if (x < 0)
          ++n0;
        else
          ++n1;
      }

  for (int x=min_x_; x<= max_x_; ++x)
    for (int y= min_y_; y<= max_y_; ++y)
      for (int z= min_z_; z<= max_z_; ++z)
      {
        if (x < 0)
          canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch(-1.0f / float(n0)));
        else // if (x >= 0)
          canonical_kernel_.emplace_back(point_3d(float(x),float(y),float(z)), dispatch( 1.0f / float(n1)));
      }

  //set the dimension of the 3-d grid
  max_point_.set(max_x_,max_y_,max_z_);
  min_point_.set(min_x_,min_y_,min_z_);

  //set the current kernel
  kernel_ = canonical_kernel_;
  factory_name_ = name();
  return;
}

//: Return an xml element
bxml_data_sptr bvpl_edge3d_kernel_factory::xml_element()
{
  bxml_element *factory = new bxml_element("bvpl_edge3d_kernel_factory");
  factory->append_text("\n");

  factory->set_attribute("min_x" , min_x_);
  factory->set_attribute("max_x" , max_x_);

  factory->set_attribute("min_y" , min_y_);
  factory->set_attribute("max_y" , max_y_);


  factory->set_attribute("min_z" , min_z_);
  factory->set_attribute("max_z" , max_z_);

  factory->set_attribute("axix_x", this->rotation_axis_[0]);
  factory->set_attribute("axix_y", this->rotation_axis_[1]);
  factory->set_attribute("axix_z", this->rotation_axis_[2]);

  factory->set_attribute("angle", this->angle_);
  return factory;
}

//: Read an xml element
bvpl_kernel_sptr bvpl_edge3d_kernel_factory::parse_xml_element(const bxml_data_sptr& d)
{
  bxml_element query("bvpl_edge3d_kernel_factory");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return nullptr;
  }

  auto* gp_root = dynamic_cast<bxml_element*>(root.ptr());

  //get the variables
  int min_x, max_x, min_y, max_y, min_z, max_z;
  float axis_x, axis_y, axis_z, angle;

  gp_root->get_attribute("min_x" , min_x);
  gp_root->get_attribute("max_x" , max_x);

  gp_root->get_attribute("min_y" , min_y);
  gp_root->get_attribute("max_y" , max_y);

  gp_root->get_attribute("min_z" , min_z);
  gp_root->get_attribute("max_z" , max_z);

  gp_root->get_attribute("axix_x", axis_x);
  gp_root->get_attribute("axix_y", axis_y);
  gp_root->get_attribute("axix_z", axis_z);

  gp_root->get_attribute("angle", angle);
  bvpl_edge3d_kernel_factory factory(min_x, max_x, min_y, max_y, min_z, max_z);
  factory.set_rotation_axis(vnl_float_3(axis_x,axis_y,axis_z));
  factory.set_angle(angle);

  bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());
  kernel->set_xml_element(factory.xml_element());
  return kernel;
}
