// This is//Projects/vxl/src/contrib/brl/bseg/bvpl/bvpl_kernel.h
#ifndef bvpl_kernel_h
#define bvpl_kernel_h
//:
// \file
// \brief A class to hold a kernel created by bvpl_kernel_factory
// \author Isabel Restrepo mir@lems.brown.edu
// \date  August 25, 2009
//
// \verbatim
//  Modifications
//   August 25, 2009 Moved out of bvpl_kernel_factory.h
// \endverbatim

#include <iostream>
#include <string>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_float_3.h>
#include <vbl/vbl_ref_count.h>
#include "bvpl_kernel_iterator.h"
#include <vgl/vgl_vector_3d.h>
#include <bxml/bxml_document.h>

class bvpl_kernel;
typedef vbl_smart_ptr<bvpl_kernel> bvpl_kernel_sptr;

//: A simple class to hold bvpl_kernel_iterator and its bounding cube
class bvpl_kernel: public vbl_ref_count
{
 public:
  //: Default constructor
  bvpl_kernel(){id_=bvpl_kernel::get_next_id();}
  //: Constructor
  bvpl_kernel(bvpl_kernel_iterator kernel, vnl_float_3 axis, vnl_float_3 aux_axis, float angle, vgl_vector_3d<int> dim, vgl_point_3d<int> min_pt, vgl_point_3d<int> max_pt, std::string name = "", double voxel_length = 1.0)
  : kernel_(kernel),axis_(axis),aux_axis_(aux_axis), angle_(angle),dim_(dim),min_point_(min_pt),max_point_(max_pt),name_(std::move(name)),voxel_length_(voxel_length)
  {
//#ifdef DEBUG
    std::cout << "Creating kernel with axis, angle, dim, max, min = " << axis_ << ' ' << angle_<< ' ' << dim_<< ' ' <<max_point_<< ' ' << min_point_ << '\n';
//#endif
    id_=bvpl_kernel::get_next_id();
  }
  //: Destructor
  ~bvpl_kernel() override = default;
  bvpl_kernel_iterator iterator(){return kernel_;}
  vnl_float_3 axis() const {return axis_;}
  vnl_float_3 aux_axis() const {return aux_axis_;}
  float angle() const {return angle_;}
  vnl_float_3 scale() const {return scale_;}
  vgl_vector_3d<int> dim()const {return dim_;}
  vgl_point_3d<int> min_point() const {return min_point_;}
  vgl_point_3d<int> max_point() const {return max_point_;}
  //: Return the length of a voxel in global coordinates
  double voxel_length() const {return voxel_length_;}
  void set_voxel_length(double length) {voxel_length_=length;}
  void set_xml_element(bxml_data_sptr x_data) {factory_data_ = x_data; }
  std::string name() { return name_; }
  vgl_vector_3d<int> offset()
  {
    int x=0;
    if (min_point_.x() < 0)
      x = -1 *min_point_.x();

    int y=0;
    if (min_point_.y() < 0)
      y=-1*min_point_.y();

    int z=max_point_.z();

    return {x,y,z};
  }

  void print()
  {
    std::cout << "***************Printing bvpl_kernel **********************\n"
             << "Axis: " << axis_ << '\n'
             << "Aux-axis: " << aux_axis_ << '\n'
             << "Angle: " << angle_ << '\n';
    kernel_.begin();
    while (!kernel_.isDone()) {
      vgl_point_3d<int> coord =kernel_.index();
      float val= ((*kernel_).c_);

      //std::cout.precision(2);
      std::cout << coord << "  " << val<< std::endl;
      ++kernel_;
    }
  }

  //: Return min and max values (kernel dispatch)
  float max_val();
  float min_val();

  void print_to_file(const std::string& filename);

  bool save_raw(const std::string& filename);

  //: Return an xml element
  bxml_data_sptr xml_element();

  //: Read an xml element
  static bvpl_kernel_sptr parse_xml_element(const bxml_data_sptr& d);

  // Returns a sum of kernel values. Useful to check if they add up to zero
  float cum_sum()
  {
    float val = 0.0f;
    kernel_.begin();
    while (!kernel_.isDone()) {
      val += ((*kernel_).c_);
      ++kernel_;
    }
    std::cout << "Kernel sums to : " << val << std::endl;
    return val;
  }

  unsigned id() const {return id_;}

  static unsigned get_next_id();

  //: Set up access to the floating point kernel -- this should be made more elegantly
  std::vector<std::pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > float_kernel_;

 private:
  bvpl_kernel_iterator kernel_;
  //: Orientation axis
  vnl_float_3 axis_;
  //: Auxiliary axis - used to define a coordinate system
  vnl_float_3 aux_axis_;
  //: Angle of rotation about main axis
  float angle_;
  vnl_float_3 scale_;
  vgl_vector_3d<int> dim_;
  vgl_point_3d<int> min_point_;
  vgl_point_3d<int> max_point_;
  unsigned int id_;
  //: Identifying string
  std::string name_;
  //: Length of a voxel in global coordinates
  double voxel_length_;
  bxml_data_sptr factory_data_;
};


//: A simple class to hold a vector of kernels
class bvpl_kernel_vector : public vbl_ref_count
{
 public:
  typedef std::vector< bvpl_kernel_sptr >::iterator iterator;
  //: Default constructor
  bvpl_kernel_vector() = default;

  iterator begin() { return kernels_.begin(); }
  iterator end()   { return kernels_.end(); }
  int size() const {return kernels_.size(); }

  vgl_vector_3d<int> max_dim(){
    iterator it =  kernels_.begin();
    int max_x = (*it)->dim().x();
    int max_y = (*it)->dim().y();
    int max_z = (*it)->dim().z();
    for (; it!= kernels_.end(); ++it)
    {
      vgl_vector_3d<int> dim = (*it)->dim();
      if (dim.x() > max_x)
        max_x = dim.x();
      if (dim.y() > max_y)
        max_y = dim.y();
      if (dim.z() > max_z)
        max_z = dim.z();
    }
    return {max_x, max_y, max_z};
  }

  vgl_point_3d<int> max(){
    iterator it =  kernels_.begin();
    int max_x = (*it)->max_point().x();
    int max_y = (*it)->max_point().y();
    int max_z = (*it)->max_point().z();
    for (; it!= kernels_.end(); ++it)
    {
      vgl_point_3d<int> max_pt = (*it)->max_point();
      if (max_pt.x() > max_x)
        max_x = max_pt.x();
      if (max_pt.y() > max_y)
        max_y = max_pt.y();
      if (max_pt.z() > max_z)
        max_z = max_pt.z();
    }
    return {max_x, max_y, max_z};
  }

  vgl_point_3d<int> min() {
    iterator it =  kernels_.begin();
    int min_x = (*it)->min_point().x();
    int min_y = (*it)->min_point().y();
    int min_z = (*it)->min_point().z();
    for (; it!= kernels_.end(); ++it)
    {
      vgl_point_3d<int> min_pt = (*it)->min_point();
      if (min_pt.x() < min_x)
        min_x = min_pt.x();
      if (min_pt.y() < min_y)
        min_y = min_pt.y();
      if (min_pt.z() < min_z)
        min_z = min_pt.z();
    }
    return {min_x, min_y, min_z};
  }

  //: vector of kernel
  std::vector< bvpl_kernel_sptr> kernels_;
};

typedef vbl_smart_ptr<bvpl_kernel_vector> bvpl_kernel_vector_sptr;


#endif // bvpl_kernel_h
