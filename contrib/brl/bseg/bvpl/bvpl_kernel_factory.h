// This is brl/bseg/bvpl/bvpl_kernel_factory.h
#ifndef bvpl_kernel_base_h
#define bvpl_kernel_base_h
//:
// \file
// \brief  A base class for 3D kernel. The kernel is fully described by a rotation axis and an angle
//         The children of this classes must create a kernel on a "canonical form" i.e. with a default
//         axis of rotation and a complementary vector here called canonical_paralle_axis_.
//         If this two vector are seen as points on the unit sphere, then they lie on the same
//         parallel (constant polar angle). These vectors determine the zero rotation position as followos:
//         1. The rotation that transforms the canonical_rotation_axis to a new specified rotation axis,
//            is applied to the kernel (Lets call such rotation R).
//         2. The kernel is rotate around its new rototion such that R*canonical_parallel_axis achieves
//            the same same polar angle as the new rotation axis
//         Usage:
//         The user can create a kernel in its canonical form through children constructors
//         Then, the user call set_rotation_axis(), set_angle() and create() and if one whises to use
//         another angle with the same axis, it suffies to call set_angle(angle) and create(). This will save computation
//         time since the rotation_axis has already been set
//         Alternativly, the user can call create(axis, angle). This transforms the canonical kernel according
//         to new parameters
//
// \author Isabel Restrepo mir@lems.brown.edu
// \date  May 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_iterator.h"
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/algo/vgl_rotation_3d.h>

//: A simple class to hold bvpl_kernel_iterator and its bounding cube
class bvpl_kernel:public vbl_ref_count
{
 public:
  //: Default constructor
  bvpl_kernel() {id_=++id_cnt;}
  //: Constructor
  bvpl_kernel(bvpl_kernel_iterator kernel, vgl_vector_3d<int> dim, vgl_point_3d<int> max_pt, vgl_point_3d<int> min_pt)
  : kernel_(kernel),dim_(dim),min_(min_pt),max_(max_pt) {id_=++id_cnt;}
  //: Destructor
  ~bvpl_kernel() {}
  bvpl_kernel_iterator iterator(){return kernel_;}
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
  typedef vcl_vector< vcl_pair<vnl_vector_fixed<float,3>, bvpl_kernel_sptr > >::iterator iterator;
  //: Default constructor
  bvpl_kernel_vector() {}

  iterator begin() { return kernels_.begin(); }
  iterator end()   { return kernels_.end(); }
  //: vector of kernel and their corresponding orientation axis.
  // Note that the magnitude of the vector corresponds to the rotation angle around that axis
  vcl_vector< vcl_pair<vnl_vector_fixed<float,3>, bvpl_kernel_sptr > > kernels_;
};

typedef vbl_smart_ptr<bvpl_kernel_vector> bvpl_kernel_vector_sptr;


//: A factory of bvpl_kernels
class bvpl_kernel_factory
{
 public:
  virtual ~bvpl_kernel_factory() {}

  //: Returns a kernel described by class variables rotation_axis_, \p angle_
  //  The user can modified the axis and angle using set_rotation_axis() and set_angle()
  bvpl_kernel create();

  //: Returns a kernel described by inputs; rotation_axis and angle
  bvpl_kernel create(vnl_vector_fixed<float,3> rotation_axis, float angle);

  //: Sets the toration axis of this kernel
  void set_rotation_axis( vnl_vector_fixed<float,3> rotation_axis);

  //: Sets rotation angle of this kernel
  void set_angle(float angle) { angle_ =angle; }

  //: Returns the current aligments axis
  vnl_vector_fixed<float,3> axis() const { return rotation_axis_; }

  //: Return current rotation angle around axis()
  float angle() const { return angle_; }

  //: Returns angular resolution around rotation axis
  float angular_resolution() const { return angular_resolution_; }

  //: Return x-y-z maximum coordinate values
  vgl_point_3d<int> max_() const { return max3d_; }

  //: Return x-y-z minimum coordinate values
  vgl_point_3d<int> min_() const { return min3d_; }

  //: returns the rectangular dimensions around the kernel center
  vgl_vector_3d<int> dim();

  /******************Batch Methods ***********************/
  //: Creates a vector of kernels with azimuthal and elevation resolutio equal to pi/4. And angle of rotation= angular_resolution_
  template <class F>
  bvpl_kernel_vector_sptr create_kernel_vector(F func);

  ////: Creates a vector of kernels according to given  azimuthal and elevation resolutio, and angle of rotation= angular_resolution_
  //virtual bvpl_kernel_vector_sptr create_kernel_vector(float pi, float phi)=0;

  ////: Creates a vector of kernels  according to given azimuthal, levation resolutio and angle_res
  //virtual bvpl_kernel_vector_sptr create_kernel_vector(float pi, float phi, float angular_res)=0;

 protected:

  typedef vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel_type;

  //:The map of 3d positions and their symbols. This kernel has an axis of rotation, but it is always on zero-rotation position
  vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > kernel_;

  //:The map of 3d positions and their symbols in their cacnonical form (As specified by children)
  vcl_vector<vcl_pair<vgl_point_3d<float>, bvpl_kernel_dispatch> > canonical_kernel_;

  //:The rotation axis for canonical edge
  vnl_vector_fixed<float,3> canonical_rotation_axis_;

  //:Rotation axis of kernel_
  vnl_vector_fixed<float,3> rotation_axis_;

  // parallel_axis_ define a coordinate system for each kernel.
  // The vector (parallel_axis_ - rotation_axis_) defines the direction of the minor axis of the kernel
  // The zero-rotation for any rotation axis is that for which parallel_axis_ and rotation_axis_ have constant polar angle
  vnl_vector_fixed<float,3> canonical_parallel_axis_;

  //: Amounts rotation around rotation_axis_
  float angle_;

  //: Dimensions of the 3D grid
  vgl_point_3d<int> max3d_;

  //: Dimensions of the 3D grid
  vgl_point_3d<int> min3d_;

  //: Angular resolutions
  float angular_resolution_;

  //: Creates canonical(default) kernel.
  //  It is decribed by a canonical axis of rotation and a canonical parallel axis
  //  This is the main function implemented by the children.
  virtual void create_canonical()=0;

  //: Rounds coordinates of kernel to the nearest integer
  bvpl_kernel_iterator interpolate(kernel_type const& kernel);

  //: Rotates "class-kernel_" around "class-rotation_axis_"  by an "angle"
  kernel_type rotate(float angle);

  //: Rotates "class-kernel_" using the given rotation matrix
  kernel_type rotate(vgl_rotation_3d<float> R);
};
template <class F>
bvpl_kernel_vector_sptr bvpl_kernel_factory::create_kernel_vector(F func)
{
    vcl_vector<vnl_float_3> axes=func.get_axes();
    bvpl_kernel_vector_sptr vec_kernel=new bvpl_kernel_vector();

    for(unsigned i=0;i<axes.size();i++)
    {
        this->set_rotation_axis(axes[i]);
        vec_kernel->kernels_.push_back(vcl_make_pair(axes[i], new bvpl_kernel(this->create())));
    }
    return vec_kernel;
}



#endif
