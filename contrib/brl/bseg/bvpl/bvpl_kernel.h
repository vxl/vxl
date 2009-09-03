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
    bvpl_kernel(bvpl_kernel_iterator kernel, vnl_float_3 axis, float angle, vgl_vector_3d<int> dim, vgl_point_3d<int> max_pt, vgl_point_3d<int> min_pt)
    : kernel_(kernel),axis_(axis), angle_(angle),dim_(dim),min_(min_pt),max_(max_pt) {id_=++id_cnt;}
    //: Destructor
    ~bvpl_kernel() {}
    bvpl_kernel_iterator iterator(){return kernel_;}
    vnl_float_3 axis(){return axis_;}
    float angle(){return angle_;}
    vnl_float_3 scale(){return scale_;}
    vgl_vector_3d<int> dim()const {return dim_;}
    vgl_point_3d<int> min() const {return min_;}
    vgl_point_3d<int> max() const {return max_;}

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
    vgl_point_3d<int> min_;
    vgl_point_3d<int> max_;
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

    //: vector of kernel
    vcl_vector< bvpl_kernel_sptr> kernels_;
};

typedef vbl_smart_ptr<bvpl_kernel_vector> bvpl_kernel_vector_sptr;

#endif // bvpl_kernel_h
