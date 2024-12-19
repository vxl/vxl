// This is vpgl/vpgl_affine_tri_focal_tensor.h
#ifndef vpgl_affine_tri_focal_tensor_h_
#define vpgl_affine_tri_focal_tensor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
//:
// \file
// \brief The affine trifocal tensor
//
// A class to hold an affine trifocal tensor and perform common operations, such as
// point and line transfer, coordinate-frame transformation and I/O.
// A subclass of tri_focal_tensor
//
// \author  J.L. Mundy
// \date April 3, 2018
//
// \verbatim
//  Modifications:
// \endverbatim
//
// The previous implementation was based on the 1998 paper by Mendonca and R. Cipolla
//  Analysis and Computation of an Affine Trifocal Tensor
//
//  The current implementation directly computes the tensor from three camera matrices and
//  is considerably more numerically stable. The computation is from H&Z
//
//                           | ~a^i |
//        T_iqr = (-1)^(i+1) |  b^q |  <-determinant of 4x4 matrix
//                           |  c^r |
// i, q, r in {1, 2, 3}
// the notation a^i, b^q, c^r  indicates rows of the 3x4 camera matrices A, B, C.
// and ~a^i indicates two rows of camera matrix A with row i left out.
//------------------------------------------------------------------------------

#include <utility>
#include <vector>
#include <iostream>
#include <iosfwd>
#include <stdexcept>
#include <math.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vgl/vgl_fwd.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/algo/vgl_algo_fwd.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include "vpgl_proj_camera.h"
#include "vpgl_affine_camera.h"
#include "vpgl_fundamental_matrix.h"
#include "vpgl_affine_fundamental_matrix.h"
#include "vpgl_tri_focal_tensor.h"

template <class Type>
class vpgl_affine_tri_focal_tensor : public vpgl_tri_focal_tensor<Type>
{
  // Data Members------------------------------------------------------------
protected:
  // scale the image point locations to the range [-1, 1] for improved tensor accuracy
  std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms_;

  // scaling transforms from image dimensions
  static std::vector<vgl_h_matrix_2d<Type>>
  img_pt_transforms_from_dims(const std::vector<std::pair<size_t, size_t>> & dims);

  // reset internal state
  void
  clear_img_pt_transforms();

  void
  clear()
  {
    vpgl_tri_focal_tensor<Type>::clear();
    clear_img_pt_transforms();
  }

  // setup using 3 affine cameras, tensor, optional scaling transforms
  void
  set_cams_and_tensor(const vpgl_affine_camera<Type> & c1,
                      const vpgl_affine_camera<Type> & c2,
                      const vpgl_affine_camera<Type> & c3,
                      const vbl_array_3d<Type> & T,
                      std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms = {});

public:
  // Constructors/Initializers/Destructors-----------------------------------

  vpgl_affine_tri_focal_tensor() { this->clear(); }

  vpgl_affine_tri_focal_tensor(const vbl_array_3d<Type> & T)
    : vpgl_tri_focal_tensor<Type>(T)
  {
    this->clear_img_pt_transforms();
  }

  //: Construct from projective tri focal tensor
  vpgl_affine_tri_focal_tensor(const vpgl_tri_focal_tensor<Type> & T)
    : vpgl_tri_focal_tensor<Type>(T)
  {
    this->clear_img_pt_transforms();
  }

  //: Construct from 27-element vector
  vpgl_affine_tri_focal_tensor(const Type * affine_tri_focal_tensor_array)
    : vpgl_tri_focal_tensor<Type>(affine_tri_focal_tensor_array)
  {
    this->clear_img_pt_transforms();
  }

  //: Construct from three cameras
  vpgl_affine_tri_focal_tensor(const vpgl_affine_camera<Type> & c1,
                               const vpgl_affine_camera<Type> & c2,
                               const vpgl_affine_camera<Type> & c3)
  {
    this->set(c1, c2, c3);
  }

  //: Construct from two remaining cameras, the first camera is already canonical, i.e. [1 0 0 | 0]
  //                                                                                    [0 1 0 | 0]
  //                                                                                    [0 0 0 | 1]
  vpgl_affine_tri_focal_tensor(const vpgl_affine_camera<Type> & c2, const vpgl_affine_camera<Type> & c3)
  {
    this->set(c2, c3);
  }

  //: Construct from three affine camera matrices
  vpgl_affine_tri_focal_tensor(const vnl_matrix_fixed<Type, 2, 4> & m1,
                               const vnl_matrix_fixed<Type, 2, 4> & m2,
                               const vnl_matrix_fixed<Type, 2, 4> & m3)
  {
    this->set(m1, m2, m3);
  }

  //: Construct from two camera matrices
  vpgl_affine_tri_focal_tensor(const vnl_matrix_fixed<Type, 3, 4> & m2, const vnl_matrix_fixed<Type, 3, 4> & m3)
  {
    this->set(m2, m3);
  }

  //: Construct from three cameras with scaling transforms
  vpgl_affine_tri_focal_tensor(const vpgl_affine_camera<Type> & c1,
                               const vpgl_affine_camera<Type> & c2,
                               const vpgl_affine_camera<Type> & c3,
                               std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms)
  {
    this->set(c1, c2, c3, img_pt_transforms);
  }

  //: Construct from three cameras with image dimensions
  vpgl_affine_tri_focal_tensor(const vpgl_affine_camera<Type> & c1,
                               const vpgl_affine_camera<Type> & c2,
                               const vpgl_affine_camera<Type> & c3,
                               const std::vector<std::pair<size_t, size_t>> & image_dims_ni_nj)
  {
    this->set(c1, c2, c3, image_dims_ni_nj);
  }

  //: destructor
  ~vpgl_affine_tri_focal_tensor() override = default;

  // Data Access-------------------------------------------------------------

  //: setup from cameras or camera matrices
  void
  set(const vpgl_affine_camera<Type> & c1, const vpgl_affine_camera<Type> & c2, const vpgl_affine_camera<Type> & c3);

  void
  set(const vpgl_affine_camera<Type> & c2, const vpgl_affine_camera<Type> & c3)
  {
    set(vpgl_affine_camera<Type>(), c2, c3);
  }

  void
  set(const vnl_matrix_fixed<Type, 2, 4> & m1,
      const vnl_matrix_fixed<Type, 2, 4> & m2,
      const vnl_matrix_fixed<Type, 2, 4> & m3)
  {
    this->set(vpgl_affine_camera<Type>(m1), vpgl_affine_camera<Type>(m2), vpgl_affine_camera<Type>(m3));
  }

  void
  set(const vnl_matrix_fixed<Type, 2, 4> & m2, const vnl_matrix_fixed<Type, 2, 4> & m3)
  {
    this->set(vpgl_affine_camera<Type>(), vpgl_affine_camera<Type>(m2), vpgl_affine_camera<Type>(m3));
  }

  //: setup from three cameras with scaling transforms
  void
  set(const vpgl_affine_camera<Type> & c1,
      const vpgl_affine_camera<Type> & c2,
      const vpgl_affine_camera<Type> & c3,
      std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms);

  //: setup from three cameras with image dimensions
  void
  set(const vpgl_affine_camera<Type> & c1,
      const vpgl_affine_camera<Type> & c2,
      const vpgl_affine_camera<Type> & c3,
      const std::vector<std::pair<size_t, size_t>> & image_dims_ni_nj);

  // Data Control------------------------------------------------------------
  vnl_matrix_fixed<Type, 3, 3>
  point_constraint_3x3(const vgl_homg_point_2d<Type> & point1,
                       const vgl_homg_point_2d<Type> & point2,
                       const vgl_homg_point_2d<Type> & point3) const override
  {
    vgl_homg_point_2d<Type> p1t = img_pt_transforms_[0] * point1;
    vgl_homg_point_2d<Type> p2t = img_pt_transforms_[1] * point2;
    vgl_homg_point_2d<Type> p3t = img_pt_transforms_[2] * point3;
    return vpgl_tri_focal_tensor<Type>::point_constraint_3x3(p1t, p2t, p3t);
  }

  Type
  point_constraint(const vgl_homg_point_2d<Type> & point1,
                   const vgl_homg_point_2d<Type> & point2,
                   const vgl_homg_point_2d<Type> & point3) const override
  {
    vgl_homg_point_2d<Type> p1t = img_pt_transforms_[0] * point1;
    vgl_homg_point_2d<Type> p2t = img_pt_transforms_[1] * point2;
    vgl_homg_point_2d<Type> p3t = img_pt_transforms_[2] * point3;
    return vpgl_tri_focal_tensor<Type>::point_constraint(p1t, p2t, p3t);
  }

  //: tri focal tensor line constraint (should be a 3 vector all zeros if lines correspond)
  vnl_vector_fixed<Type, 3>
  line_constraint_3(const vgl_homg_line_2d<Type> & line1,
                    const vgl_homg_line_2d<Type> & line2,
                    const vgl_homg_line_2d<Type> & line3) const override
  {
    vgl_homg_line_2d<Type> line1t = img_pt_transforms_[0] * line1;
    vgl_homg_line_2d<Type> line2t = img_pt_transforms_[1] * line2;
    vgl_homg_line_2d<Type> line3t = img_pt_transforms_[2] * line3;
    return vpgl_tri_focal_tensor<Type>::line_constraint_3(line1t, line2t, line3t);
  }

  //: point transfer
  vgl_homg_point_2d<Type>
  image1_transfer(const vgl_homg_point_2d<Type> & point2, const vgl_homg_point_2d<Type> & point3) const override
  {
    vgl_homg_point_2d<Type> p2t = img_pt_transforms_[1] * point2;
    vgl_homg_point_2d<Type> p3t = img_pt_transforms_[2] * point3;
    vgl_homg_point_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image1_transfer(p2t, p3t);
    return img_pt_transforms_[0].preimage(ret);
  }

  vgl_homg_point_2d<Type>
  image2_transfer(const vgl_homg_point_2d<Type> & point1, const vgl_homg_point_2d<Type> & point3) const override
  {
    vgl_homg_point_2d<Type> p1t = img_pt_transforms_[0] * point1;
    vgl_homg_point_2d<Type> p3t = img_pt_transforms_[2] * point3;
    vgl_homg_point_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image2_transfer(p1t, p3t);
    return img_pt_transforms_[1].preimage(ret);
  }

  vgl_homg_point_2d<Type>
  image3_transfer(const vgl_homg_point_2d<Type> & point1, const vgl_homg_point_2d<Type> & point2) const override
  {
    vgl_homg_point_2d<Type> p1t = img_pt_transforms_[0] * point1;
    vgl_homg_point_2d<Type> p2t = img_pt_transforms_[1] * point2;
    vgl_homg_point_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image3_transfer(p1t, p2t);
    return img_pt_transforms_[2].preimage(ret);
  }

  //: line transfer
  //  line in image 1 corresponding to lines in images 2 and 3 and etc.
  vgl_homg_line_2d<Type>
  image1_transfer(const vgl_homg_line_2d<Type> & line2, const vgl_homg_line_2d<Type> & line3) const override
  {
    vgl_homg_line_2d<Type> l2t = img_pt_transforms_[1] * line2;
    vgl_homg_line_2d<Type> l3t = img_pt_transforms_[2] * line3;
    vgl_homg_line_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image1_transfer(l2t, l3t);
    return img_pt_transforms_[0].preimage(ret);
  }

  vgl_homg_line_2d<Type>
  image2_transfer(const vgl_homg_line_2d<Type> & line1, const vgl_homg_line_2d<Type> & line3) const override
  {
    vgl_homg_line_2d<Type> l1t = img_pt_transforms_[0] * line1;
    vgl_homg_line_2d<Type> l3t = img_pt_transforms_[2] * line3;
    vgl_homg_line_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image2_transfer(l1t, l3t);
    return img_pt_transforms_[1].preimage(ret);
  }

  vgl_homg_line_2d<Type>
  image3_transfer(const vgl_homg_line_2d<Type> & line1, const vgl_homg_line_2d<Type> & line2) const override
  {
    vgl_homg_line_2d<Type> l1t = img_pt_transforms_[0] * line1;
    vgl_homg_line_2d<Type> l2t = img_pt_transforms_[1] * line2;
    vgl_homg_line_2d<Type> ret = vpgl_tri_focal_tensor<Type>::image3_transfer(l1t, l2t);
    return img_pt_transforms_[2].preimage(ret);
  }

  //: homographies induced by a line
  // homography between images 3 and 1 given a line in image 2 and etc.
  // note that image normalizing transforms are taken into account
  vgl_h_matrix_2d<Type>
  hmatrix_13(const vgl_homg_line_2d<Type> & line2) const override
  {
    vgl_homg_line_2d<Type> l2t = img_pt_transforms_[1] * line2;
    vgl_h_matrix_2d<Type> Ht = vpgl_tri_focal_tensor<Type>::hmatrix_13(l2t);
    return (img_pt_transforms_[2].get_inverse()) * Ht * img_pt_transforms_[0];
  }

  vgl_h_matrix_2d<Type>
  hmatrix_12(const vgl_homg_line_2d<Type> & line3) const override
  {
    vgl_homg_line_2d<Type> l3t = img_pt_transforms_[2] * line3;
    l3t.normalize();
    vgl_h_matrix_2d<Type> Ht = vpgl_tri_focal_tensor<Type>::hmatrix_12(l3t);
    return (img_pt_transforms_[1].get_inverse()) * Ht * img_pt_transforms_[0];
  }

  //: epipoles
  void
  get_epipoles(vgl_homg_point_2d<Type> & e12, vgl_homg_point_2d<Type> & e13) override
  {
    vgl_homg_point_2d<Type> temp12, temp13;
    vpgl_tri_focal_tensor<Type>::get_epipoles(temp12, temp13);
    e12 = img_pt_transforms_[1].preimage(temp12);
    e13 = img_pt_transforms_[2].preimage(temp13);
  }

  vgl_homg_point_2d<Type>
  epipole_12() override
  {
    vgl_homg_point_2d<Type> temp = vpgl_tri_focal_tensor<Type>::epipole_12();
    return img_pt_transforms_[1].preimage(temp);
  }

  vgl_homg_point_2d<Type>
  epipole_13() override
  {
    vgl_homg_point_2d<Type> temp = vpgl_tri_focal_tensor<Type>::epipole_13();
    return img_pt_transforms_[2].preimage(temp);
  }

  //: affine fundamental matrices
  vpgl_affine_fundamental_matrix<Type>
  affine_fmatrix_12();

  vpgl_affine_fundamental_matrix<Type>
  affine_fmatrix_13();

  vpgl_affine_fundamental_matrix<Type>
  affine_fmatrix_23();

  //: affine cameras
  vpgl_affine_camera<Type>
  affine_camera_1();

  vpgl_affine_camera<Type>
  affine_camera_2();

  vpgl_affine_camera<Type>
  affine_camera_3();


  // INTERNALS---------------------------------------------------------------
private:
  static vbl_array_3d<Type>
  tensor_matrix(const vpgl_affine_camera<Type> & c1,
                const vpgl_affine_camera<Type> & c2,
                const vpgl_affine_camera<Type> & c3);

  static vpgl_affine_fundamental_matrix<Type>
  null_F()
  {
    vnl_matrix_fixed<Type, 3, 3> M(Type(0));
    return vpgl_affine_fundamental_matrix<Type>(M);
  }

  static vpgl_affine_camera<Type>
  null_acam()
  {
    vnl_matrix_fixed<Type, 2, 4> M(Type(0));
    return vpgl_affine_camera<Type>(M);
  }
};

//: stream operators
template <class Type>
std::ostream &
operator<<(std::ostream &, const vpgl_affine_tri_focal_tensor<Type> & aT);

template <class Type>
std::istream &
operator>>(std::istream &, vpgl_affine_tri_focal_tensor<Type> & aT);

//: convert projective camera to affine camera (swap last two cols) check if affine
template <class Type>
bool
affine(const vpgl_proj_camera<Type> & pcam, vpgl_affine_camera<Type> & acam);

template <class Type>
vpgl_affine_camera<Type>
affine(const vpgl_proj_camera<Type> & pcam);

//: convert affine camera to projective camera swap last two cols (check if valid)
template <class Type>
bool
proj(const vpgl_affine_camera<Type> & acam, vpgl_proj_camera<Type> & pcam);

template <class Type>
vpgl_proj_camera<Type>
proj(const vpgl_affine_camera<Type> & acam);

//: convert projective fundamental matrix to affine fundamental matrix - perform check
template <class Type>
bool
affine(const vpgl_fundamental_matrix<Type> & F, vpgl_affine_fundamental_matrix<Type> & aF);

template <class Type>
vpgl_affine_fundamental_matrix<Type>
affine(const vpgl_fundamental_matrix<Type> & F);

#endif
