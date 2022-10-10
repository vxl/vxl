// This is vpgl/vpgl_tri_focal_tensor.h
#ifndef vpgl_tri_focal_tensor_h_
#define vpgl_tri_focal_tensor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
//:
// \file
// \brief The trifocal tensor
//
// A class to hold a Trifocal Tensor and perform common operations, such as
// point and line transfer, coordinate-frame transformation and I/O.
//
// \author
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// \verbatim
//  Modifications:
//   AWF - Added composition, transformation, homography generation.
//   Peter Vanroose - 11 Mar 97 - added operator==
//   Peter Vanroose - 22 Jun 03 - added vgl interface
//   JL Mundy       - 30 Mar 18 - moved to vpgl, templated
// \endverbatim
//
//------------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_algo_fwd.h>
#include "vpgl_proj_camera.h"
#include "vpgl_fundamental_matrix.h"

template <class Type>
class vpgl_tri_focal_tensor
{
  // Data Members------------------------------------------------------------
 protected:

  vbl_array_3d<Type> T_;

  // Epipoles
  // e12 is the epipole corresponding to the center of camera 1 projected into image 2
  // e13 is the epipole corresponding to the center of camera 1 projected into image 3
  bool epipoles_valid_ = false;
  vgl_homg_point_2d<Type> e12_;
  vgl_homg_point_2d<Type> e13_;

  // cameras
  bool cameras_valid_ = false;
  vpgl_proj_camera<Type> c1_;
  vpgl_proj_camera<Type> c2_;
  vpgl_proj_camera<Type> c3_;

  // fundamental matrices
  // f12 is the fundamental matrix mapping a point in image 1 into a line in image 2
  // f13 is the fundamental matrix mapping a point in image 1 into a line in image 3
  // (note this order is opposite to H&Z but seems more intuitive)
  //
  bool f_matrices_1213_valid_ = false;
  vpgl_fundamental_matrix<Type> f12_;
  vpgl_fundamental_matrix<Type> f13_;
  bool f_matrix_23_valid_ = false;
  vpgl_fundamental_matrix<Type> f23_;

  // set flags false and results invalid
  void
  init();

  // make the Frobenius norm == 1
  void
  normalize();

 public:

  // Constructors/Initializers/Destructors-----------------------------------

  vpgl_tri_focal_tensor()
    : T_(vbl_array_3d<Type>(3, 3, 3, Type(0)))
  {
    for (size_t i = 0; i < 3; ++i)
      T_[i][i][i] = Type(1);
    this->init();
  }

  vpgl_tri_focal_tensor(const vbl_array_3d<Type> & T)
    : T_(T)
  {
    this->init();
  }

  //: Construct from 27-element vector
  vpgl_tri_focal_tensor(const Type * tri_focal_tensor_array)
    : T_(vbl_array_3d<Type>(3, 3, 3, tri_focal_tensor_array))
  {
    this->init();
  }

  //: Construct from three cameras
  vpgl_tri_focal_tensor(const vpgl_proj_camera<Type> & c1,
                        const vpgl_proj_camera<Type> & c2,
                        const vpgl_proj_camera<Type> & c3)
    : T_(vbl_array_3d<Type>(3, 3, 3, Type(0)))
  {
    this->set(c1, c2, c3);
    this->init(); // init must be second to avoid writing over cameras
  }

  //: Construct from two remaining cameras, the first camera is already canonical, i.e. [I | 0]
  vpgl_tri_focal_tensor(const vpgl_proj_camera<Type> & c2,
                        const vpgl_proj_camera<Type> & c3)
    : T_(vbl_array_3d<Type>(3, 3, 3, Type(0)))
  {
    this->set(vpgl_proj_camera<Type>(), c2, c3);
    this->init(); // init must be second to avoid writing over cameras
  }

  //: Construct from three camera matrices
  vpgl_tri_focal_tensor(const vnl_matrix_fixed<Type, 3, 4> & m1,
                        const vnl_matrix_fixed<Type, 3, 4> & m2,
                        const vnl_matrix_fixed<Type, 3, 4> & m3)
  {
    this->set(vpgl_proj_camera<Type>(m1),
              vpgl_proj_camera<Type>(m2),
              vpgl_proj_camera<Type>(m3));
    this->init(); // init must be second to avoid writing over cameras
  }

  //: Construct from two camera matrices
  vpgl_tri_focal_tensor(const vnl_matrix_fixed<Type, 3, 4> & m2,
                        const vnl_matrix_fixed<Type, 3, 4> & m3)
  {
    this->set(vpgl_proj_camera<Type>(),
              vpgl_proj_camera<Type>(m2),
              vpgl_proj_camera<Type>(m3));
    this->init(); // init must be second to avoid writing over cameras
  }

  //: destructor
  virtual ~vpgl_tri_focal_tensor() = default;

  //: compute all derivative quantities
  virtual bool
  compute()
  {
    return (this->compute_epipoles() &&
            this->compute_f_matrices() &&
            this->compute_proj_cameras() &&
            this->compute_f_matrix_23());
  }

  // Data Access-------------------------------------------------------------

  virtual bool
  operator==(vpgl_tri_focal_tensor<Type> const & T) const
  {
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        for (size_t k = 0; k < 3; ++k)
          if (T_(i, j, k) != T(i, j, k))
            return false;
    return true;
  }

  Type &
  operator()(size_t i1, size_t i2, size_t i3)
  {
    return T_(i1, i2, i3);
  }

  Type
  operator()(size_t i1, size_t i2, size_t i3) const
  {
    return T_(i1, i2, i3);
  }

  void
  set(size_t i1, size_t i2, size_t i3, Type value)
  {
    T_(i1, i2, i3) = value;
  }

  void
  set(vbl_array_3d<Type> & array)
  {
    *this = vpgl_tri_focal_tensor<Type>(array);
  }

  void
  set(const vpgl_proj_camera<Type> & c1,
      const vpgl_proj_camera<Type> & c2,
      const vpgl_proj_camera<Type> & c3);

  void
  set(const vpgl_proj_camera<Type> & c2,
      const vpgl_proj_camera<Type> & c3)
  {
    this->set(vpgl_proj_camera<Type>(), c2, c3);
  }

  void
  set(const vnl_matrix_fixed<Type, 3, 4> & m1,
      const vnl_matrix_fixed<Type, 3, 4> & m2,
      const vnl_matrix_fixed<Type, 3, 4> & m3)
  {
    this->set(vpgl_proj_camera<Type>(m1),
              vpgl_proj_camera<Type>(m2),
              vpgl_proj_camera<Type>(m3));
  }

  // set cameras used when tri_focal tensor array is set directly
  void
  set_cams_and_tensor(const vpgl_proj_camera<Type> & c1,
                      const vpgl_proj_camera<Type> & c2,
                      const vpgl_proj_camera<Type> & c3,
                      vbl_array_3d<Type> T)
  {
    T_ = T;
    c1_ = c1;
    c2_ = c2;
    c3_ = c3;
    cameras_valid_ = true;
    this->init();
  }

  // Data Control------------------------------------------------------------
  //: tri focal tensor point constraint (should be a 3x3 array of all zeros if points correspond)
  virtual vnl_matrix_fixed<Type, 3, 3>
  point_constraint_3x3(vgl_homg_point_2d<Type> const & point1,
                       vgl_homg_point_2d<Type> const & point2,
                       vgl_homg_point_2d<Type> const & point3) const;

  //:tri focal tensor scalar point constraint (should == 0 if points correspond)
  virtual Type
  point_constraint(vgl_homg_point_2d<Type> const & point1,
                   vgl_homg_point_2d<Type> const & point2,
                   vgl_homg_point_2d<Type> const & point3) const;

  //: tri focal tensor line constraint (should be a 3 vector all zeros if lines correspond)
  virtual vnl_vector_fixed<Type, 3>
  line_constraint_3(vgl_homg_line_2d<Type> const & line1,
                    vgl_homg_line_2d<Type> const & line2,
                    vgl_homg_line_2d<Type> const & line3) const;

  //: point transfer
  //  point in image 1 corresponding to points in images 2 and 3 and etc.
  virtual vgl_homg_point_2d<Type>
  image1_transfer(vgl_homg_point_2d<Type> const & point2,
                  vgl_homg_point_2d<Type> const & point3) const;

  virtual vgl_homg_point_2d<Type>
  image2_transfer(vgl_homg_point_2d<Type> const & point1,
                  vgl_homg_point_2d<Type> const & point3) const;

  virtual vgl_homg_point_2d<Type>
  image3_transfer(vgl_homg_point_2d<Type> const & point1,
                  vgl_homg_point_2d<Type> const & point2) const;

  //: line transfer
  //  line in image 1 corresponding to lines in images 2 and 3 and etc.
  virtual vgl_homg_line_2d<Type>
  image1_transfer(vgl_homg_line_2d<Type> const & line2,
                  vgl_homg_line_2d<Type> const & line3) const;

  virtual vgl_homg_line_2d<Type>
  image2_transfer(vgl_homg_line_2d<Type> const & line1,
                  vgl_homg_line_2d<Type> const & line3) const;

  virtual vgl_homg_line_2d<Type>
  image3_transfer(vgl_homg_line_2d<Type> const & line1,
                  vgl_homg_line_2d<Type> const & line2) const;

  //: homographies induced by a line
  // homography between images 3 and 1 given a line in image 2 and etc.
  virtual vgl_h_matrix_2d<Type>
  hmatrix_13(vgl_homg_line_2d<Type> const & line2) const;

  virtual vgl_h_matrix_2d<Type>
  hmatrix_12(vgl_homg_line_2d<Type> const & line3) const;

  //: epipoles
  bool
  compute_epipoles();

  virtual bool
  get_epipoles(vgl_homg_point_2d<Type> & e12,
               vgl_homg_point_2d<Type> & e13)
  {
    if (!epipoles_valid_)
      compute_epipoles();
    e12 = e12_;
    e13 = e13_;
    return epipoles_valid_;
  }

  virtual vgl_homg_point_2d<Type>
  epipole_12()
  {
    if (!epipoles_valid_)
      compute_epipoles();
    return e12_;
  }

  virtual vgl_homg_point_2d<Type>
  epipole_13()
  {
    if (!epipoles_valid_)
      compute_epipoles();
    return e13_;
  }

  //: fundamental matrices
  // The fundamental matrix between image 1 and image 2 is given by:
  // ${\tt F12}_{jk} = \left [e12 \right ]_\times T_{ijk} e13_k$.  //note the use of Einstein notation for summation
  // and between image 1 and image 3 is given by:
  // ${\tt F13}_{jk} = \left [e13 \right]_\times T_{ijk} e12_j$.
  // Note that $\left [ e13 \right ]_\times$ is just the skew-symmetric matrix corresponding to e13.
  //
  bool
  compute_f_matrices();

  bool
  compute_f_matrix_23();

  vpgl_fundamental_matrix<Type>
  fmatrix_12()
  {
    if (!f_matrices_1213_valid_)
      compute_f_matrices();
    return f12_;
  }

  vpgl_fundamental_matrix<Type>
  fmatrix_13()
  {
    if (!f_matrices_1213_valid_)
      compute_f_matrices();
    return f13_;
  }

  vpgl_fundamental_matrix<Type>
  fmatrix_23()
  {
    if (!f_matrix_23_valid_)
      compute_f_matrix_23();
    return f23_;
  }

  //: projective cameras
  bool
  compute_proj_cameras();

  vpgl_proj_camera<Type>
  proj_camera_1()
  {
    if (!cameras_valid_)
      compute_proj_cameras();
    return c1_;
  }

  vpgl_proj_camera<Type>
  proj_camera_2()
  {
    if (!cameras_valid_)
      compute_proj_cameras();
    return c2_;
  }

  vpgl_proj_camera<Type>
  proj_camera_3()
  {
    if (!cameras_valid_)
      compute_proj_cameras();
    return c3_;
  }

  // Utility Methods---------------------------------------------------------

  void
  get_constraint_lines_image1(vgl_homg_point_2d<Type> const & p2,
                              vgl_homg_point_2d<Type> const & p3,
                              std::vector<vgl_homg_line_2d<Type>> & lines) const;

  void
  get_constraint_lines_image2(vgl_homg_point_2d<Type> const & p1,
                              vgl_homg_point_2d<Type> const & p3,
                              std::vector<vgl_homg_line_2d<Type>> & lines) const;

  void
  get_constraint_lines_image3(vgl_homg_point_2d<Type> const & p1,
                              vgl_homg_point_2d<Type> const & p2,
                              std::vector<vgl_homg_line_2d<Type>> & lines) const;


  //: Contract Tensor axis tensor_axis with first component of Matrix M.
  // That is:
  // For tensor_axis = 1,  Compute T_ijk = T_pjk M_pi
  // For tensor_axis = 2,  Compute T_ijk = T_ipk M_pj
  // For tensor_axis = 3,  Compute T_ijk = T_ijp M_pk
  vpgl_tri_focal_tensor<Type>
  postmultiply(size_t tensor_axis, const vnl_matrix<Type> & M) const;

  //: Contract Tensor axis tensor_axis with second component of Matrix M.
  // That is:
  // For tensor_axis = 1,  Compute T_ijk = M_ip T_pjk
  // For tensor_axis = 2,  Compute T_ijk = M_jp T_ipk
  // For tensor_axis = 3,  Compute T_ijk = M_kp T_ijp
  vpgl_tri_focal_tensor<Type>
  premultiply(size_t tensor_axis, const vnl_matrix<Type> & M) const;

  //: implementations for individual axes
  vpgl_tri_focal_tensor<Type>
  postmultiply1(const vnl_matrix<Type> & M) const;

  vpgl_tri_focal_tensor<Type>
  postmultiply2(const vnl_matrix<Type> & M) const;

  vpgl_tri_focal_tensor<Type>
  postmultiply3(const vnl_matrix<Type> & M) const;

  vpgl_tri_focal_tensor<Type>
  premultiply1(const vnl_matrix<Type> & M) const;

  vpgl_tri_focal_tensor<Type>
  premultiply2(const vnl_matrix<Type> & M) const;

  vpgl_tri_focal_tensor<Type>
  premultiply3(const vnl_matrix<Type> & M) const;

  // contractions involving vectors
  //: ${\tt M}_{jk} = T_{ijk} v_i$
  vnl_matrix_fixed<Type, 3, 3>
  dot1(const vnl_vector_fixed<Type, 3> & v) const;

  //: ${\tt M}_{ik} = T_{ijk} v_j$
  vnl_matrix_fixed<Type, 3, 3>
  dot2(const vnl_vector_fixed<Type, 3> & v) const;

  //: ${\tt M}_{ij} = T_{ijk} v_k$
  vnl_matrix_fixed<Type, 3, 3>
  dot3(const vnl_vector_fixed<Type, 3> & v) const;

  //: ${\tt M}_{kj} = T_{ijk} v_i$ (The transpose of dot1)
  vnl_matrix_fixed<Type, 3, 3>
  dot1t(const vnl_vector_fixed<Type, 3> & v) const;

  //: ${\tt M}_{ki} = T_{ijk} v_j$ (The transpose of dot2).
  vnl_matrix_fixed<Type, 3, 3>
  dot2t(const vnl_vector_fixed<Type, 3> & v) const;

  //: ${\tt M}_{ji} = T_{ijk} v_k$ (The transpose of dot3)
  vnl_matrix_fixed<Type, 3, 3>
  dot3t(const vnl_vector_fixed<Type, 3> & v) const;

  // INTERNALS---------------------------------------------------------------

 private:

};

//: stream operators
template <class Type>
std::ostream &
operator<<(std::ostream &, const vpgl_tri_focal_tensor<Type> & T);

template <class Type>
std::istream &
operator>>(std::istream &, vpgl_tri_focal_tensor<Type> & T);

//: are two tensors within a scale factor of each other, i.e. T1 ~ T2.
template <class Type>
bool
within_scale(const vpgl_tri_focal_tensor<Type> & T1, const vpgl_tri_focal_tensor<Type> & T2);

#endif // vpgl_tri_focal_tensor_h_
