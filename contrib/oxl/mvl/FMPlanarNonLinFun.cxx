// This is oxl/mvl/FMPlanarNonLinFun.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "FMPlanarNonLinFun.h"

#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vgl/vgl_homg_line_2d.h>
#include <mvl/ImageMetric.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/FMatrixPlanar.h>

const int FMPlanarNonLinFun_nparams = 9;

//-----------------------------------------------------------------------------
//: Constructor
//
FMPlanarNonLinFun::FMPlanarNonLinFun(const ImageMetric* image_metric1,
                                     const ImageMetric* image_metric2,
                                     double /*outlier_distance_squared*/,
                                     vcl_vector<vgl_homg_point_2d<double> >& points1,
                                     vcl_vector<vgl_homg_point_2d<double> >& points2)
: vnl_least_squares_function(FMPlanarNonLinFun_nparams, points1.size(), no_gradient)
, data_size_(points1.size())
, points1_(points1)
, points2_(points2)
, normalized_(2*data_size_)
, image_metric1_(image_metric1)
, image_metric2_(image_metric2)
{
  // Form single array from both points1 and points2
  vcl_vector<vgl_homg_point_2d<double> > points(points1);
  for (unsigned i = 0; i < points2.size(); ++i)
    points.push_back(points2[i]);

  // Condition points
  normalized_.normalize(points);

  // Set up contitioning matrices
  denorm_matrix_     = normalized_.get_C();
  denorm_matrix_inv_ = normalized_.get_C_inverse();
}

FMPlanarNonLinFun::FMPlanarNonLinFun(const ImageMetric* image_metric1,
                                     const ImageMetric* image_metric2,
                                     double /*outlier_distance_squared*/,
                                     vcl_vector<HomgPoint2D>& points1,
                                     vcl_vector<HomgPoint2D>& points2):
   vnl_least_squares_function(FMPlanarNonLinFun_nparams, points1.size(), no_gradient),
  data_size_(points1.size()),
  normalized_(2*data_size_),
  image_metric1_(image_metric1),
  image_metric2_(image_metric2)
{
  for (unsigned i = 0; i < points1.size(); ++i)
    points1_.push_back(vgl_homg_point_2d<double>(points1[i].x(),points1[i].y(),points1[i].w()));
  for (unsigned i = 0; i < points2.size(); ++i)
    points2_.push_back(vgl_homg_point_2d<double>(points2[i].x(),points2[i].y(),points2[i].w()));
  // Form single array
  vcl_vector<HomgPoint2D> points(points1);
  for (unsigned i = 0; i < points2.size(); ++i)
    points.push_back(points2[i]);

  // Condition points
  normalized_.normalize(points);

  // Set up contitioning matrices
  denorm_matrix_     = normalized_.get_C();
  denorm_matrix_inv_ = normalized_.get_C_inverse();
}

//-----------------------------------------------------------------------------
//: Compute the planar F matrix and returns true if successful.
//
bool FMPlanarNonLinFun::compute(FMatrixPlanar* F)
{
  // fm_fmatrix_nagmin
  vcl_cerr << "FMPlanarNonLinFun: matches = "<<data_size_<<", using "<<FMPlanarNonLinFun_nparams<<" parameters\n";

  /* transform F to well-conditioned frame. */
  const vnl_matrix<double>& post = denorm_matrix_inv_.as_ref();
  const vnl_matrix<double>& pre  = denorm_matrix_inv_.transpose().as_ref();
  FMatrixPlanar norm_F(pre * F->get_matrix() * post);

  /* parameterise it. */
  vnl_vector<double> f_params(FMPlanarNonLinFun_nparams);
  fmatrix_to_params (norm_F, f_params);

  vnl_levenberg_marquardt lm(*this);
  if (!lm.minimize(f_params))
       return false;

  vcl_cerr<<"FMPlanarNonLinFun: minimisation start error "
          << lm.get_start_error() / vcl_sqrt(double(data_size_))
          <<" end error "
          << lm.get_end_error() / vcl_sqrt(double(data_size_))
          <<vcl_endl;

  norm_F = params_to_fmatrix (f_params);

  F->set(denorm_matrix_.transpose() * norm_F.get_matrix() * denorm_matrix_);

  vcl_cerr << "fm_fmatrix_nagmin: accepted " << data_size_ << '/' << data_size_
           << " rms point-epipolar error " << lm.get_end_error() / vcl_sqrt(double(data_size_))
           << vcl_endl;

  return true;
}

//-----------------------------------------------------------------------------
//: The virtual function from vnl_levenberg_marquardt which returns the RMS epipolar error and a vector of residuals.
//
void FMPlanarNonLinFun::f(vnl_vector<double> const& f_params, vnl_vector<double>& fx)
{
     FMatrixPlanar norm_F = params_to_fmatrix(f_params);

     FMatrixPlanar F(denorm_matrix_.transpose() * norm_F.get_matrix() * denorm_matrix_);

     for (int i = 0; i < data_size_; ++i) {
          const vgl_homg_point_2d<double>& p1 = points1_[i];
          const vgl_homg_point_2d<double>& p2 = points2_[i];

          vgl_homg_line_2d<double> l12 = F.image2_epipolar_line(p1);
          double r1 = image_metric2_.perp_dist_squared(p2, l12);

          vgl_homg_line_2d<double> l21 = F.image1_epipolar_line(p2);
          double r2 = image_metric1_.perp_dist_squared(p1, l21);

          fx[i] = vcl_sqrt((r1 + r2) / 2.0);
     }
     // vcl_cerr << "Err = " << vcl_sqrt (distance_squared / (data_size_ * 2)) << vcl_endl;

//   return vcl_sqrt (distance_squared / (data_size_ * 2)); // void function cannot return
}

void FMPlanarNonLinFun::fmatrix_to_params(const FMatrixPlanar& F,
                                          vnl_vector<double>& params)
{
  if (FMPlanarNonLinFun_nparams == 9)
    fmatrix_to_params_awf(F, params);
  else
    fmatrix_to_params_mna(F, params);
}

FMatrixPlanar FMPlanarNonLinFun::params_to_fmatrix(const vnl_vector<double>& params)
{
  if (FMPlanarNonLinFun_nparams == 9)
    return params_to_fmatrix_awf(params);
  else
    return params_to_fmatrix_mna(params);
}

//-----------------------------------------------------------------------------
// Private Function: Converts from a 3x3 F matrix to the 6 parameters
// for the planar form. See FMatrixPlanar.init(const vnl_matrix<double>&)
// for algorithm details.
//
void FMPlanarNonLinFun::fmatrix_to_params_mna(const FMatrixPlanar& F,
                                              vnl_vector<double>& params)
{
  // this converts to [e2]x[l]x[e1] form - see A Zisserman
  HomgPoint2D e1,e2;
  F.get_epipoles(&e1,&e2);

  vnl_symmetric_eigensystem<double>  symm_eig(F.get_matrix()+F.get_matrix().transpose());

  double eig0 = symm_eig.D(0,0);
  double eig1 = symm_eig.D(2,2);

  if (eig0 > 0 || eig1 < 0) {
    vcl_cerr << "ERROR in FMPlanarNonLinFun: vnl_symmetric_eigensystem<double>  is unsorted: " << symm_eig.D << vcl_endl;
    vcl_abort();
  }

  if (vcl_fabs(symm_eig.D(1,1)) > 1e-12)
    vcl_cerr << "FMPlanarNonLinFun: WARNING: middle eigenvalue not 0: " << symm_eig.D << vcl_endl;

  vnl_vector<double> v0(symm_eig.get_eigenvector(0));
  vnl_vector<double> v1(symm_eig.get_eigenvector(2));

  vnl_double_3 f1 = vcl_sqrt(eig1)*v1 + vcl_sqrt(-eig0)*v0;
  vnl_double_3 f2 = vcl_sqrt(eig1)*v1 - vcl_sqrt(-eig0)*v0;

  vnl_double_3 ls;
  if (vcl_fabs(HomgOperator2D::dot(e1,f1)/e1.w()) + vcl_fabs(HomgOperator2D::dot(e2,f1)/e2.w()) >
      vcl_fabs(HomgOperator2D::dot(e1,f2)/e1.w()) + vcl_fabs(HomgOperator2D::dot(e2,f2)/e2.w()) )
    ls = f1;
  else
    ls = f2;

  ls /= ls.magnitude();

  double ls_thi = vcl_acos(ls[2]);
  if (ls_thi < 0) ls_thi += vnl_math::pi;

  double ls_theta;
  if (ls[1] >= 0)
    ls_theta =  vcl_acos(ls[0]/vcl_sin(ls_thi));
  else
    ls_theta = -vcl_acos(ls[0]/vcl_sin(ls_thi));

  params[0] = ls_theta;
  params[1] = ls_thi;
  params[2] = e1.x()/e1.w();
  params[3] = e1.y()/e1.w();
  params[4] = e2.x()/e2.w();
  params[5] = e2.y()/e2.w();

#ifdef PARANOID
  // Check parameterization
  {
    FMatrixPlanar back = params_to_fmatrix_mna(params);
    double norm = vnl_matops::homg_diff(back.get_matrix(), F.get_matrix());
    if (norm > 1e-12) {
      vcl_cerr << "FMPlanarNonLinFun: WARNING! deparameterization diff = " << norm << vcl_endl
               << "b = [" << back << "];\n"
               << "n = [" << F << "];\n";
    }
  }
#endif
}

//-----------------------------------------------------------------------------
// Private Function: Construct the fundamental matrix from the 6 parameters.
// See FMatrixPlanar for more details.
//
FMatrixPlanar FMPlanarNonLinFun::params_to_fmatrix_mna(const vnl_vector<double>& params)
{
     double ls1 = vcl_cos(params[0])*vcl_sin(params[1]);
     double ls2 = vcl_sin(params[0])*vcl_sin(params[1]);
     double ls3 = vcl_cos(params[1]);

     double list1[9] = {0,              -1.0,           params[5],
                        1,              0,              -params[4],
                        -params[5],     params[4],      0};
     double list2[9] = {0,-ls3,ls2,ls3,0,-ls1,-ls2,ls1,0};
     double list3[9] = {0,-1.0,params[3],1,0,-params[2],-params[3],params[2],0};

     vnl_matrix<double> mat1(3,3,9,list1),mat2(3,3,9,list2),mat3(3,3,9,list3);

     vnl_matrix<double> fmat = mat1*mat2*mat3;

     fmat /= fmat.fro_norm();

     return FMatrixPlanar(fmat);
}

void FMPlanarNonLinFun::fmatrix_to_params_awf(const FMatrixPlanar& F, vnl_vector<double>& params)
{
  // this converts to [e2]x[l]x[e1] form - see A Zisserman
  HomgPoint2D e1,e2;
  F.get_epipoles(&e1,&e2);

  vnl_symmetric_eigensystem<double>  symm_eig(F.get_matrix()+F.get_matrix().transpose());

  double eig0 = symm_eig.D(0,0);
  double eig1 = symm_eig.D(2,2);

  if (eig0 > 0 || eig1 < 0) {
    vcl_cerr << "ERROR in FMPlanarNonLinFun: vnl_symmetric_eigensystem<double>  is unsorted: " << symm_eig.D << vcl_endl;
    vcl_abort();
  }

  if (vcl_fabs(symm_eig.D(1,1)) > 1e-12)
    vcl_cerr << "FMPlanarNonLinFun: WARNING: middle eigenvalue not 0: " << symm_eig.D << vcl_endl;

  vnl_vector<double> v0(symm_eig.get_eigenvector(0));
  vnl_vector<double> v1(symm_eig.get_eigenvector(2));

  vnl_double_3 f1 = vcl_sqrt(eig1)*v1 + vcl_sqrt(-eig0)*v0;
  vnl_double_3 f2 = vcl_sqrt(eig1)*v1 - vcl_sqrt(-eig0)*v0;

  vnl_double_3 ls;
  if (vcl_fabs(HomgOperator2D::dot(e1,f1)/e1.w()) + vcl_fabs(HomgOperator2D::dot(e2,f1)/e2.w()) >
      vcl_fabs(HomgOperator2D::dot(e1,f2)/e1.w()) + vcl_fabs(HomgOperator2D::dot(e2,f2)/e2.w()) )
    ls = f1;
  else
    ls = f2;

  ls /= ls.magnitude();
  double mag1 = e1.get_vector().magnitude();
  double mag2 = e2.get_vector().magnitude();

  params[0] = ls[0];
  params[1] = ls[1];
  params[2] = ls[2];
  params[3] = e1.x()/mag1;
  params[4] = e1.y()/mag1;
  params[5] = e1.w()/mag1;
  params[6] = e2.x()/mag2;
  params[7] = e2.y()/mag2;
  params[8] = e2.w()/mag2;

  // Check parameterization
  {
    FMatrixPlanar back = params_to_fmatrix_awf(params);
    double norm = vnl_matops::homg_diff(back.get_matrix().as_ref(), F.get_matrix().as_ref());
    if (norm > 1e-12) {
      vcl_cerr << "FMPlanarNonLinFun: WARNING! deparameterization diff = " << norm << vcl_endl
               << "b = [" << back << "];\n"
               << "n = [" << F << "];\n";
    }
  }
}

//-----------------------------------------------------------------------------
// Private Function: Construct the fundamental matrix from the 6 parameters.
// See FMatrixPlanar for more details.
//
FMatrixPlanar FMPlanarNonLinFun::params_to_fmatrix_awf(const vnl_vector<double>& params)
{
  const double* v = params.data_block();
  vnl_cross_product_matrix L(v);
  vnl_cross_product_matrix E1(v+3);
  vnl_cross_product_matrix E2(v+6);

  vnl_matrix<double> fmat = E2.as_ref() * L * E1;

  fmat /= fmat.fro_norm();

  return FMatrixPlanar(fmat);
}
