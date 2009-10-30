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

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_float_3.h>
#include <vbl/vbl_ref_count.h>
#include "bvpl_kernel_iterator.h"
#include <vgl/vgl_vector_3d.h>

//: A simple class to hold bvpl_kernel_iterator and its bounding cube
class bvpl_kernel: public vbl_ref_count
{
 public:
  //: Default constructor
  bvpl_kernel() {id_=++id_cnt;}
  //: Constructor
  bvpl_kernel(bvpl_kernel_iterator kernel, vnl_float_3 axis, float angle, vgl_vector_3d<int> dim, vgl_point_3d<int> min_pt, vgl_point_3d<int> max_pt)
  : kernel_(kernel),axis_(axis), angle_(angle),dim_(dim),min_point_(min_pt),max_point_(max_pt)
  {
    vcl_cout << "Creating kernel with axis, anle, dim, max, min =\n" << axis_ << '\n' << angle_<< '\n' << dim_<< '\n' <<max_point_<< '\n' << min_point_ << '\n';
    id_=++id_cnt;
  }
  //: Destructor
  ~bvpl_kernel() {}
  bvpl_kernel_iterator iterator(){return kernel_;}
  vnl_float_3 axis(){return axis_;}
  float angle(){return angle_;}
  vnl_float_3 scale(){return scale_;}
  vgl_vector_3d<int> dim()const {return dim_;}
  vgl_point_3d<int> min_point() const {return min_point_;}
  vgl_point_3d<int> max_point() const {return max_point_;}
  vgl_vector_3d<int> offset()
  {
    int x=0;
    if (min_point_.x() < 0)
      x = -1 *min_point_.x();

    int y=0;
    if (min_point_.y() < 0)
      y=-1*min_point_.y();

    int z=max_point_.z();

    return vgl_vector_3d<int>(x,y,z);
  }

  void print()
  {
    kernel_.begin();
    while (!kernel_.isDone()) {
      vgl_point_3d<int> coord =kernel_.index();
      float val= ((*kernel_).c_);

      vcl_cout.precision(2);
      vcl_cout << coord << "  " << val<< vcl_endl;
      ++kernel_;
    }
  }

  void print_to_file(vcl_string filename);

  bool save_raw(vcl_string filename);

  // Returns a sum of kernel values. Useful to check if they add up to zero
  float cum_sum()
  {
    float val = 0.0f;
    kernel_.begin();
    while (!kernel_.isDone()) {
      val += ((*kernel_).c_);
      ++kernel_;
    }
    vcl_cout << "Kernel sums to : " << val << vcl_endl;
    return val;
  }

  unsigned id(){return id_;}
  static unsigned id_cnt;

 private:
  bvpl_kernel_iterator kernel_;
  vnl_float_3 axis_;
  float angle_;
  vnl_float_3 scale_;
  vgl_vector_3d<int> dim_;
  vgl_point_3d<int> min_point_;
  vgl_point_3d<int> max_point_;
  unsigned int id_;
};

typedef vbl_smart_ptr<bvpl_kernel> bvpl_kernel_sptr;

//: A simple class to hold a vector of kernels
class bvpl_kernel_vector : public vbl_ref_count
{
 public:
  typedef vcl_vector< bvpl_kernel_sptr >::iterator iterator;
  //: Default constructor
  bvpl_kernel_vector() {}

  iterator begin() { return kernels_.begin(); }
  iterator end()   { return kernels_.end(); }
  int size() {return kernels_.size(); }

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
    return vgl_vector_3d<int>(max_x, max_y, max_z);
  }

  vgl_point_3d<int> max(){
    iterator it =  kernels_.begin();
    int max_x = (*it)->max_point().x();
    int max_y = (*it)->max_point().y();
    int max_z = (*it)->max_point().z();
    for (; it!= kernels_.end(); ++it)
    {
      vgl_point_3d<int> max = (*it)->max_point();
      if (max.x() > max_x)
        max_x = max.x();
      if (max.y() > max_y)
        max_y = max.y();
      if (max.z() > max_z)
        max_z = max.z();
    }
    return vgl_point_3d<int>(max_x, max_y, max_z);
  }

  vgl_point_3d<int> min() {
    iterator it =  kernels_.begin();
    int min_x = (*it)->min_point().x();
    int min_y = (*it)->min_point().y();
    int min_z = (*it)->min_point().z();
    for (; it!= kernels_.end(); ++it)
    {
      vgl_point_3d<int> min = (*it)->min_point();
      if (min.x() < min_x)
        min_x = min.x();
      if (min.y() < min_y)
        min_y = min.y();
      if (min.z() < min_z)
        min_z = min.z();
    }
    return vgl_point_3d<int>(min_x, min_y, min_z);
  }

  //: vector of kernel
  vcl_vector< bvpl_kernel_sptr> kernels_;
};

typedef vbl_smart_ptr<bvpl_kernel_vector> bvpl_kernel_vector_sptr;

#endif // bvpl_kernel_h
