// This is oxl/mvl/FMPlanarNonLinFun.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "FMPlanarNonLinFun.h"

#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

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
                                     vcl_vector<HomgPoint2D>& points1,
                                     vcl_vector<HomgPoint2D>& points2):
   vnl_least_squares_function(FMPlanarNonLinFun_nparams, points1.size(), no_gradient),
  _data_size(points1.size()),
  _points1(points1),
  _points2(points2),
  _normalized(2*_data_size),
  _image_metric1(image_metric1),
  _image_metric2(image_metric2)
{
  // Form single array
  vcl_vector<HomgPoint2D> points(points1);
  for (unsigned i = 0; i < points2.size(); ++i)
    points.push_back(points2[i]);

  // Condition points
  _normalized.normalize(points);

  // Set up contitioning matrices
  _denorm_matrix     = _normalized.get_C();
  _denorm_matrix_inv = _normalized.get_C_inverse();
}

//-----------------------------------------------------------------------------
//: Compute the planar F matrix and returns true if succesful.
//
bool FMPlanarNonLinFun::compute(FMatrixPlanar* F)
{
  // fm_fmatrix_nagmin
  vcl_cerr << "FMPlanarNonLinFun: matches = "<<_data_size<<", using "<<FMPlanarNonLinFun_nparams<<" parameters \n";

  /* transform F to well-conditioned frame. */
  const vnl_matrix<double>& post = _denorm_matrix_inv;
  const vnl_matrix<double>& pre  = _denorm_matrix_inv.transpose();
  FMatrixPlanar norm_F(pre * F->get_matrix() * post);

  /* parameterise it. */
  vnl_vector<double> f_params(FMPlanarNonLinFun_nparams);
  fmatrix_to_params (norm_F, f_params);

  vnl_levenberg_marquardt lm(*this);
  if (!lm.minimize(f_params))
       return false;

  vcl_cerr<<"FMPlanarNonLinFun: minimisation start error "
          << lm.get_start_error() / vcl_sqrt(double(_data_size))
          <<" end error "
          << lm.get_end_error() / vcl_sqrt(double(_data_size))
          <<vcl_endl;

  norm_F = params_to_fmatrix (f_params);

  F->set(_denorm_matrix.transpose() * norm_F.get_matrix() * _denorm_matrix);

  vcl_cerr << "fm_fmatrix_nagmin: accepted " << _data_size << '/' << _data_size
           << " rms point-epipolar error " << lm.get_end_error() / vcl_sqrt(double(_data_size))
           << vcl_endl;;

  return true;
}

//-----------------------------------------------------------------------------
//: The virtual function from vnl_levenberg_marquardt which returns the RMS epipolar error and a vector of residuals.
//
void FMPlanarNonLinFun::f(const vnl_vector<double>& f_params, vnl_vector<double>& fx)
{
     FMatrixPlanar norm_F = params_to_fmatrix(f_params);

     FMatrixPlanar F(_denorm_matrix.transpose() * norm_F.get_matrix() * _denorm_matrix);

     for (int i = 0; i < _data_size; ++i) {
          const HomgPoint2D& p1 = _points1[i];
          const HomgPoint2D& p2 = _points2[i];

          HomgLine2D l12 = F.image2_epipolar_line(p1);
          double r1 = _image_metric2.perp_dist_squared(p2, l12);

          HomgLine2D l21 = F.image1_epipolar_line(p2);
          double r2 = _image_metric1.perp_dist_squared(p1, l21);

          fx[i] = vcl_sqrt((r1 + r2) / 2.0);
     }
     // vcl_cerr << "Err = " << vcl_sqrt (distance_squared / (_data_size * 2)) << vcl_endl;

//   return vcl_sqrt (distance_squared / (_data_size * 2)); // void function cannot return
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

  double ls_thi = vcl_acos(ls.z());
  if (ls_thi < 0) ls_thi += vnl_math::pi;

  double ls_theta;
  if (ls.y() >= 0)
    ls_theta =  vcl_acos(ls.x()/vcl_sin(ls_thi));
  else
    ls_theta = -vcl_acos(ls.x()/vcl_sin(ls_thi));

  params[0] = ls_theta;
  params[1] = ls_thi;
  params[2] = e1.x()/e1.w();
  params[3] = e1.y()/e1.w();
  params[4] = e2.x()/e2.w();
  params[5] = e2.y()/e2.w();

#if PARANOID
  // Check parameterization
  {
    FMatrixPlanar back = params_to_fmatrix_mna(params);
    double norm = vnl_matops::homg_diff(back.get_matrix(), F.get_matrix());
    if (norm > 1e-12) {
      vcl_cerr << "FMPlanarNonLinFun: WARNING! deparameterization diff = " << norm << vcl_endl;
      vcl_cerr << "b = [" << back << "];\n";
      vcl_cerr << "n = [" << F << "];\n";
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
  vnl_double_3 e1n = e1.get_vector() / e1.get_vector().magnitude();
  vnl_double_3 e2n = e2.get_vector() / e2.get_vector().magnitude();

  params[0] = ls[0];
  params[1] = ls[1];
  params[2] = ls[2];
  params[3] = e1n[0];
  params[4] = e1n[1];
  params[5] = e1n[2];
  params[6] = e2n[0];
  params[7] = e2n[1];
  params[8] = e2n[2];

  // Check parameterization
  {
    FMatrixPlanar back = params_to_fmatrix_awf(params);
    double norm = vnl_matops::homg_diff(back.get_matrix(), F.get_matrix());
    if (norm > 1e-12) {
      vcl_cerr << "FMPlanarNonLinFun: WARNING! deparameterization diff = " << norm << vcl_endl;
      vcl_cerr << "b = [" << back << "];\n";
      vcl_cerr << "n = [" << F << "];\n";
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

  vnl_matrix<double> fmat = E2 * L * E1;

  fmat /= fmat.fro_norm();

  return FMatrixPlanar(fmat);
}
