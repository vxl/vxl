// This is oxl/mvl/TriTensor.h
#ifndef TriTensor_h_
#define TriTensor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
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
// \endverbatim
//
//------------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include <vbl/vbl_array_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_algo_fwd.h>
#include <vgl/algo/vgl_p_matrix.h>

#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgPoint2D.h>

#include <mvl/PMatrix.h>
class HMatrix2D;
class FMatrix;
class PMatrix;
class FManifoldProject;

class TriTensor
{
  // Data Members------------------------------------------------------------

  vbl_array_3d<double> T;

  // Caches for various computed quantities
  mutable const HomgPoint2D* e12_;
  mutable const HomgPoint2D* e13_;

  mutable const FManifoldProject* fmp12_;
  mutable const FManifoldProject* fmp13_;
  mutable const FManifoldProject* fmp23_;

 public:

  // Constructors/Initializers/Destructors-----------------------------------

  TriTensor();
  TriTensor(const TriTensor&);
  // Construct from 27-element vector
  TriTensor(const double *tritensor_array);
  TriTensor(const PMatrix& P1, const PMatrix& P2, const PMatrix& P3);
  TriTensor(const PMatrix& P2, const PMatrix& P3);
  TriTensor(const vnl_matrix<double>& T1, const vnl_matrix<double>& P2, const vnl_matrix<double>& P3);
 ~TriTensor();

  // Data Access-------------------------------------------------------------

  TriTensor& operator=(const TriTensor&);
  bool operator==(TriTensor const& p) const {
    for (int i=0;i<3;++i) for (int j=0;j<3;++j) for (int k=0;k<3;++k) if (p(i,j,k)!=T(i,j,k)) return false;
    return true; }
  double& operator() (unsigned int i1, unsigned int i2, unsigned int i3) { return T(i1,i2,i3); }
  double operator() (unsigned int i1, unsigned int i2, unsigned int i3) const { return T(i1,i2,i3); }

  void set(unsigned int i1, unsigned int i2, unsigned int i3, double value);

  void set(const double* vec);
  void set(const vnl_matrix<double>& tvector); // 27x1 matrix
  void convert_to_vector(vnl_matrix<double>* tvector) const; // 27x1 matrix

  void set(const PMatrix& P1, const PMatrix& P2, const PMatrix& P3);
  void set(const PMatrix& P2, const PMatrix& P3);
  void set(const vnl_matrix<double>& T1, const vnl_matrix<double>& T2, const vnl_matrix<double>& T3);

  // Data Control------------------------------------------------------------

  vgl_homg_point_2d<double> image1_transfer(vgl_homg_point_2d<double> const& point2,
                                            vgl_homg_point_2d<double> const& point3,
                                            vgl_homg_point_2d<double> corrected[] = 0) const;
  vgl_homg_point_2d<double> image2_transfer(vgl_homg_point_2d<double> const& point1,
                                            vgl_homg_point_2d<double> const& point3,
                                            vgl_homg_point_2d<double> corrected[] = 0) const;
  vgl_homg_point_2d<double> image3_transfer(vgl_homg_point_2d<double> const& point1,
                                            vgl_homg_point_2d<double> const& point2,
                                            vgl_homg_point_2d<double> corrected[] = 0) const;

  HomgPoint2D image1_transfer(HomgPoint2D const& point2,
                              HomgPoint2D const& point3,
                              HomgPoint2D corrected[] = 0) const;
  HomgPoint2D image2_transfer(HomgPoint2D const& point1,
                              HomgPoint2D const& point3,
                              HomgPoint2D corrected[] = 0) const;
  HomgPoint2D image3_transfer(HomgPoint2D const& point1,
                              HomgPoint2D const& point2,
                              HomgPoint2D corrected[] = 0) const;

  vgl_homg_point_2d<double> image1_transfer_qd(vgl_homg_point_2d<double> const& point2,
                                               vgl_homg_point_2d<double> const& point3) const;
  vgl_homg_point_2d<double> image2_transfer_qd(vgl_homg_point_2d<double> const& point1,
                                               vgl_homg_point_2d<double> const& point3) const;
  vgl_homg_point_2d<double> image3_transfer_qd(vgl_homg_point_2d<double> const& point1,
                                               vgl_homg_point_2d<double> const& point2) const;

  HomgPoint2D image1_transfer_qd(const HomgPoint2D& point2, const HomgPoint2D& point3) const;
  HomgPoint2D image2_transfer_qd(const HomgPoint2D& point1, const HomgPoint2D& point3) const;
  HomgPoint2D image3_transfer_qd(const HomgPoint2D& point1, const HomgPoint2D& point2) const;

  vgl_homg_point_2d<double> image1_transfer(vgl_homg_point_2d<double> const& point1,
                                            vgl_homg_line_2d <double> const& line2) const;
  vgl_homg_point_2d<double> image2_transfer(vgl_homg_point_2d<double> const& point1,
                                            vgl_homg_line_2d <double> const& line3) const;
  vgl_homg_point_2d<double> image3_transfer(vgl_homg_point_2d<double> const& point2,
                                            vgl_homg_line_2d <double> const& line3) const;

  HomgPoint2D image1_transfer(HomgPoint2D const& point1,
                              HomgLine2D  const& line2) const;
  HomgPoint2D image2_transfer(HomgPoint2D const& point1,
                              HomgLine2D  const& line3) const;
  HomgPoint2D image3_transfer(HomgPoint2D const& point2,
                              HomgLine2D  const& line3) const;

  vgl_homg_line_2d<double> image1_transfer(vgl_homg_line_2d<double> const& line2,
                                           vgl_homg_line_2d<double> const& line3) const;
  vgl_homg_line_2d<double> image2_transfer(vgl_homg_line_2d<double> const& line2,
                                           vgl_homg_line_2d<double> const& line3) const;
  vgl_homg_line_2d<double> image3_transfer(vgl_homg_line_2d<double> const& line2,
                                           vgl_homg_line_2d<double> const& line3) const;

  HomgLine2D image1_transfer(const HomgLine2D& line2, const HomgLine2D& line3) const;
  HomgLine2D image2_transfer(const HomgLine2D& line2, const HomgLine2D& line3) const;
  HomgLine2D image3_transfer(const HomgLine2D& line2, const HomgLine2D& line3) const;

  vgl_h_matrix_2d<double> get_hmatrix_31(vgl_homg_line_2d<double> const& line2) const;
  vgl_h_matrix_2d<double> get_hmatrix_21(vgl_homg_line_2d<double> const& line3) const;

  HMatrix2D get_hmatrix_31(const HomgLine2D& line2) const;
  HMatrix2D get_hmatrix_21(const HomgLine2D& line3) const;

  bool get_epipoles(vgl_homg_point_2d<double>& e2, vgl_homg_point_2d<double>& e3) const;
  bool get_epipoles(HomgPoint2D* e2, HomgPoint2D* e3) const;
  bool compute_epipoles() const; // mutable const

  HomgPoint2D get_epipole_12() const;
  HomgPoint2D get_epipole_13() const;

  FMatrix get_fmatrix_13() const;
  FMatrix get_fmatrix_12() const;

  FMatrix compute_fmatrix_23() const;

  const FManifoldProject* get_fmp12() const;
  const FManifoldProject* get_fmp23() const;
  const FManifoldProject* get_fmp13() const;

  void compute_P_matrices(const vnl_vector<double>& x, double alpha, double beta, PMatrix* P2, PMatrix* P3) const;
  void compute_P_matrices(const vnl_vector<double>& x, double alpha, PMatrix* P2, PMatrix* P3) const {
    compute_P_matrices(x,alpha,alpha, P2, P3);
  }
  void compute_P_matrices(const vnl_vector<double>& x, PMatrix* P2, PMatrix* P3) const {
    compute_P_matrices(x, 1, 1, P2, P3);
  }
  void compute_P_matrices(PMatrix* P2, PMatrix* P3) const {
    compute_P_matrices(vnl_double_3(1,1,1), 1, 1, P2, P3);
  }
  void compute_P_matrices(vgl_p_matrix<double> &P2, vgl_p_matrix<double> &P3) const {
    PMatrix Ptemp2, Ptemp3;
    compute_P_matrices(vnl_double_3(1,1,1), 1, 1, &Ptemp2, &Ptemp3);
    P2.set(Ptemp2.get_matrix());
    P3.set(Ptemp3.get_matrix());
  }
  void compute_caches();
  void clear_caches();

  // Utility Methods---------------------------------------------------------
  void get_constraint_lines_image3(vgl_homg_point_2d<double> const& p1,
                                   vgl_homg_point_2d<double> const& p2,
                                   vcl_vector<vgl_homg_line_2d<double> >& lines) const;
  void get_constraint_lines_image2(vgl_homg_point_2d<double> const& p1,
                                   vgl_homg_point_2d<double> const& p3,
                                   vcl_vector<vgl_homg_line_2d<double> >& lines) const;
  void get_constraint_lines_image1(vgl_homg_point_2d<double> const& p2,
                                   vgl_homg_point_2d<double> const& p3,
                                   vcl_vector<vgl_homg_line_2d<double> >& lines) const;

  void get_constraint_lines_image3(HomgPoint2D const& p1,
                                   HomgPoint2D const& p2,
                                   vcl_vector<HomgLine2D>* lines) const;
  void get_constraint_lines_image2(HomgPoint2D const& p1,
                                   HomgPoint2D const& p3,
                                   vcl_vector<HomgLine2D>* lines) const;
  void get_constraint_lines_image1(HomgPoint2D const& p2,
                                   HomgPoint2D const& p3,
                                   vcl_vector<HomgLine2D>* lines) const;

  //: Contract Tensor axis tensor_axis with first component of Matrix M.
  // That is:
  // For tensor_axis = 1,  Compute T_ijk = T_pjk M_pi
  // For tensor_axis = 2,  Compute T_ijk = T_ipk M_pj
  // For tensor_axis = 3,  Compute T_ijk = T_ijp M_pk
  TriTensor postmultiply(unsigned tensor_axis, const vnl_matrix<double>& M) const;

  //: Contract Tensor axis tensor_axis with second component of Matrix M.
  // That is:
  // For tensor_axis = 1,  Compute T_ijk = M_ip T_pjk
  // For tensor_axis = 2,  Compute T_ijk = M_jp T_ipk
  // For tensor_axis = 3,  Compute T_ijk = M_kp T_ijp
  TriTensor premultiply(unsigned tensor_axis, const vnl_matrix<double>& M) const;

  TriTensor postmultiply1(const vnl_matrix<double>& M) const;
  TriTensor postmultiply2(const vnl_matrix<double>& M) const;
  TriTensor postmultiply3(const vnl_matrix<double>& M) const;

  TriTensor premultiply1(const vnl_matrix<double>& M) const;
  TriTensor premultiply2(const vnl_matrix<double>& M) const;
  TriTensor premultiply3(const vnl_matrix<double>& M) const;

  vnl_double_3x3 dot1(const vnl_double_3& v) const;
  vnl_double_3x3 dot2(const vnl_double_3& v) const;
  vnl_double_3x3 dot3(const vnl_double_3& v) const;
  vnl_double_3x3 dot1t(const vnl_double_3& v) const;
  vnl_double_3x3 dot2t(const vnl_double_3& v) const;
  vnl_double_3x3 dot3t(const vnl_double_3& v) const;

  bool check_equal_up_to_scale(const TriTensor& that) const;

  // INTERNALS---------------------------------------------------------------

  //: C123 are line conditioning matrices.
  // If C * l = lhat, and l1 = T l2 l3, then lhat1 = That lhat2 lhat3
  TriTensor condition(const vnl_matrix<double>& line_1_denorm,
                      const vnl_matrix<double>& line_2_norm,
                      const vnl_matrix<double>& line_3_norm) const;

  TriTensor decondition(const vnl_matrix<double>& line_1_norm,
                        const vnl_matrix<double>& line_2_denorm,
                        const vnl_matrix<double>& line_3_denorm) const;
 private:
  void delete_caches() const; // mutable const
};

vcl_ostream& operator << (vcl_ostream&, const TriTensor& T);
vcl_istream& operator >> (vcl_istream&, TriTensor& T);

#endif // TriTensor_h_
