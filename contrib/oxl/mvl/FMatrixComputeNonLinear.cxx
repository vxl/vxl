// This is oxl/mvl/FMatrixComputeNonLinear.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "FMatrixComputeNonLinear.h"

#include <vcl_iostream.h>

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <mvl/HomgNorm2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgInterestPointSet.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/FMatrix.h>
#include <mvl/FManifoldProject.h>
#include <mvl/FMatrixCompute7Point.h>


// Seven parameters for a minimal parametrization of the F matrix
const int FMatrixComputeNonLinear_nparams = 7;

//-----------------------------------------------------------------------------
//: Constructor
//
FMatrixComputeNonLinear::FMatrixComputeNonLinear(PairMatchSetCorner* matches) :
   vnl_least_squares_function(FMatrixComputeNonLinear_nparams, matches->compute_match_count(), no_gradient),
  data_size_(matches->compute_match_count()),
  matches_(*matches),
  one_(true)
{
  // Copy matching points from matchset.
  // Set up some initial variables
  HomgInterestPointSet const* points1 = matches_.get_corners1();
  HomgInterestPointSet const* points2 = matches_.get_corners2();
  vcl_vector<HomgPoint2D> dead1, dead2;
  vcl_vector<int> point1_int, point2_int;
  matches_.extract_matches(dead1, point1_int, dead2, point2_int);
  data_size_ = matches_.count();
  points1_.resize(points1->size());
  points2_.resize(points2->size());

  // Get the actual image points
  for (int a = 0; a < data_size_; a++) {
    vnl_double_2 temp1;
    temp1 = points1->get_2d(point1_int[a]);
    points1_[a] = HomgPoint2D(temp1[0], temp1[1], 1.0);
  }

  for (int a = 0; a < data_size_; a++) {
    vnl_double_2 temp2;
    temp2 = points2->get_2d(point2_int[a]);
    points2_[a] = HomgPoint2D(temp2[0], temp2[1], 1.0);
  }
}

//-----------------------------------------------------------------------------
//: Compute the F Matrix by augmenting a 7 point basis

bool FMatrixComputeNonLinear::compute_basis(FMatrix* F, vcl_vector<int> basis) {
  one_ = false;
  vcl_vector<HomgPoint2D> basis1(7), basis2(7);
  for (int i = 0; i < 7; i++) {
    int other = matches_.get_match_12(basis[i]);
    if (other == -1)
      vcl_cerr << "The basis index doesn't include a match for " << i << ".\n";
    else {
      vnl_double_2 p1 = matches_.get_corners1()->get_2d(basis[i]);
      vnl_double_2 p2 = matches_.get_corners2()->get_2d(other);
        basis1[i] = HomgPoint2D(p1[0], p1[1], 1.0);
        basis2[i] = HomgPoint2D(p2[0], p2[1], 1.0);
    }
  }
  basis1_ = basis1;
  basis2_ = basis2;
  return compute(F);
}


// General compute method, will alternate between either of the
// selected options
bool FMatrixComputeNonLinear::compute(FMatrix* F)
{
  FMatrix* F_final = new FMatrix();
  // fm_fmatrix_nagmin
  vcl_cerr << "FMatrixComputeNonLinear: matches = "<< data_size_ <<", using "<< FMatrixComputeNonLinear_nparams <<" parameters \n";
  double so_far = 1e+8;
  FMatrix norm_F = *F;
  if (one_) {
    for (p_ = 0; p_ < 3; p_++) {
      for (q_ = 0; q_ < 3; q_++) {
        for (r_ = 0; r_ < 4; r_++) {
          FMatrix norm_F = *F;
          int r1 = 0, c1 = 0, r2 = 0, c2 = 0;
          get_plan(r1, c1, r2, c2);
          vnl_matrix<double> mat;
          norm_F.get(&mat);

          switch(r_) {
            case 0 :
              mat /= mat.get(r1, c1);
              break;
            case 1 :
              mat /= mat.get(r1, c2);
              break;
            case 2 :
              mat /= mat.get(r2, c1);
              break;
            case 3 :
              mat /= mat.get(r2, c2);
              break;
          }

          vnl_vector<double> f_params(FMatrixComputeNonLinear_nparams);
          fmatrix_to_params(FMatrix(mat), f_params);
          //FMatrix res = params_to_fmatrix(f_params);
          vnl_levenberg_marquardt lm(*this);

          // Adjusting these parameters will alter the
          // convergence properties of the minimisation
          lm.set_max_function_evals(200);
          lm.set_f_tolerance(1e-6);
          lm.set_x_tolerance(1e-6);
//        lm.set_epsilon_function(1e-6);
          lm.minimize(f_params);

          if (lm.get_end_error() < so_far) {
            so_far = lm.get_end_error();
            vcl_cerr << "so_far : " << so_far << vcl_endl;
            norm_F = params_to_fmatrix(f_params);
            F_final->set(norm_F);
            HomgPoint2D e1, e2;
            F_final->get_epipoles(&e1, &e2);
            double x1, y1, x2, y2;
            e1.get_nonhomogeneous(x1, y1);
            e2.get_nonhomogeneous(x2, y2);
            HomgPoint2D c1(x1, y1, 1.0);
            HomgPoint2D c2(x2, y2, 1.0);
            vcl_cerr << "Epipole locations 1 : " << c1 << " 2 : " << c2 << vcl_endl;
          }
        }
      }
    }
  } else {
    F_orig_ = norm_F;
    vnl_vector<double> f_params(FMatrixComputeNonLinear_nparams, 0.0);
    FMatrix res = params_to_fmatrix(f_params);
    vnl_matrix<double> mat1 = norm_F.get_matrix();
    vnl_matrix<double> mat2 = res.get_matrix();
    mat1 /= mat1.get(2, 2);
    mat2 /= mat2.get(2, 2);

    vnl_levenberg_marquardt lm(*this);

    // Adjusting these parameters will alter the
    // convergence properties of the minimisation
    lm.set_max_function_evals(100);
    lm.set_f_tolerance(1e-4);
    lm.set_x_tolerance(1e-3);
    lm.set_epsilon_function(1e-4);
    lm.minimize(f_params);

    if (lm.get_end_error() < so_far) {
      so_far = lm.get_end_error();
      vcl_cerr << "so_far : " << so_far << vcl_endl;
      for (int l = 0; l < 7; l++)
        vcl_cerr << f_params(l) << vcl_endl;
      norm_F = params_to_fmatrix(f_params);
      F_final->set(norm_F);
      lm.diagnose_outcome();
      HomgPoint2D e1, e2;
      F_final->get_epipoles(&e1, &e2);
      double x1, y1, x2, y2;
      e1.get_nonhomogeneous(x1, y1);
      e2.get_nonhomogeneous(x2, y2);
      HomgPoint2D c1(x1, y1, 1.0);
      HomgPoint2D c2(x2, y2, 1.0);
      vcl_cerr << "Epipole locations 1 : " << c1 << " 2 : " << c2 << vcl_endl;
    }
  }
  F = F_final;
  return true;
}

//-----------------------------------------------------------------------------
//: The virtual function from vnl_levenberg_marquardt which returns the RMS epipolar error and a vector of residuals.
void FMatrixComputeNonLinear::f(const vnl_vector<double>& f_params, vnl_vector<double>& fx)
{
  FMatrix F = params_to_fmatrix(f_params);

  fx = calculate_residuals(&F);
}

// Convert an F Matrix to the parameters to be minimised
void FMatrixComputeNonLinear::fmatrix_to_params(const FMatrix& F, vnl_vector<double>& params)
{
  // Leaving d as 1.0 on all the rotations
  int c1= 0, r1 = 0, c2 = 0, r2 = 0;
  get_plan(r1, c1, r2, c2);
  double b = 0.0, a = 0.0, c = 0.0;
  switch(r_) {
    case 0 :
      a = F.get(r1, c2);
      b = -F.get(r2, c1);
      c = -F.get(r2, c2);
      break;
    case 1 :
      a = F.get(r1, c1);
      b = -F.get(r2 ,c1);
      c = -F.get(r2, c2);
      break;
    case 2 :
      a = F.get(r1, c1);
      b = F.get(r1, c2);
      c = -F.get(r2, c2);
      break;
    case 3 :
      a = F.get(r1, c1);
      b = F.get(r1, c2);
      c = -F.get(r2, c1);
      break;
  }
  HomgPoint2D one, two;
  vnl_double_2 e1, e2;
  F.get_epipoles(&one, &two);
  vnl_double_3 e1h(one.get_x(), one.get_y(), one.get_w());
  vnl_double_3 e2h(two.get_x(), two.get_y(), two.get_w());
  e1h = e1h.normalize();
  e2h = e2h.normalize();

  switch(p_) {
    case 0 :
      e1 = vnl_double_2(e1h[1]/e1h[0], e1h[2]/e1h[0]);
      break;
    case 1 :
      e1 = vnl_double_2(e1h[0]/e1h[1], e1h[2]/e1h[1]);
      break;
    case 2 :
      e1 = vnl_double_2(e1h[0]/e1h[2], e1h[1]/e1h[2]);
      break;
  }
  switch(q_) {
    case 0 :
      e2 = vnl_double_2(e2h[1]/e2h[0], e2h[2]/e2h[0]);
      break;
    case 1 :
      e2 = vnl_double_2(e2h[0]/e2h[1], e2h[2]/e2h[1]);
      break;
    case 2 :
      e2 = vnl_double_2(e2h[0]/e2h[2], e2h[1]/e2h[2]);
      break;
  }
  params.put(0, a);
  params.put(1, b);
  params.put(2, c);
  params.put(3, e1[0]);
  params.put(4, e1[1]);
  params.put(5, e2[0]);
  params.put(6, e2[1]);
}


// Convert the parameters to an F Matrix
FMatrix FMatrixComputeNonLinear::params_to_fmatrix(const vnl_vector<double>& params)
{
  FMatrix ret;

  if (one_) {
    vnl_matrix<double> ref = ret.get_matrix();
    // Again the d is moved about through the different parametrizations
    int c1= 0, r1 = 0, c2 = 0, r2 = 0;
    get_plan(r1, c1, r2, c2);
    double a = params.get(0);
    double b = params.get(1);
    double c = params.get(2);
    double x = -params.get(3);
    double y = -params.get(4);
    double xd = -params.get(5);
    double yd = -params.get(6);
    double d;
    switch(r_) {
      case 0 :
        d = 1.0;
        ref.put(r1, c2, a);
        ref.put(r2, c1, -b);
        ref.put(r2, c2, -c);
        ref.put(r1, c1, d);
        ref.put(r1, p_, d*x + a*y);
        ref.put(r2, p_, -b*x - c*y);
        ref.put(q_, c1, d*xd - b*yd);
        ref.put(q_, c2, a*xd - c*yd);
        ref.put(q_, p_, xd*(d*x + a*y) - yd*(b*x + c*y));
        break;
      case 1 :
        d = 1.0;
        ref.put(r1, c1, a);
        ref.put(r2, c1, -b);
        ref.put(r2, c2, -c);
        ref.put(r1, c2, d);
        ref.put(r1, p_, a*x + d*y);
        ref.put(r2, p_, -b*x - c*y);
        ref.put(q_, c1, a*xd - b*yd);
        ref.put(q_, c2, d*xd - c*yd);
        ref.put(q_, p_, xd*(a*x + d*y) - yd*(b*x + c*y));
        break;
      case 2 :
        d = -1.0;
        ref.put(r1, c1, a);
        ref.put(r1, c2, b);
        ref.put(r2, c2, -c);
        ref.put(r2, c1, -d);
        ref.put(r1, p_, a*x + b*y);
        ref.put(r2, p_, -d*x - c*y);
        ref.put(q_, c1, a*xd - d*yd);
        ref.put(q_, c2, b*xd - c*yd);
        ref.put(q_, p_, xd*(a*x + b*y) - yd*(d*x + c*y));
        break;
      case 3 :
        d = -1.0;
        ref.put(r1, c1, a);
        ref.put(r1, c2, b);
        ref.put(r2, c1, -c);
        ref.put(r2, c2, -d);
        ref.put(r1, p_, a*x + b*y);
        ref.put(r2, p_, -c*x - d*y);
        ref.put(q_, c1, a*xd - c*yd);
        ref.put(q_, c2, b*xd - d*yd);
        ref.put(q_, p_, xd*(a*x + b*y) - yd*(c*x + d*y));
        break;
    }
    ret.set(ref);
    return ret;
  } else {
    vcl_vector<HomgPoint2D> new_points1(7), new_points2(7);
    HomgPoint2D e1, e2;
    F_orig_.get_epipoles(&e1, &e2);
    vnl_double_2 e1n = e1.get_double2();
    vnl_double_2 e2n = e2.get_double2();
    double grads1, grads2;
    for (int i = 0; i < 7; i++) {
      vnl_double_2 t1 = basis1_[i].get_double2();
      vnl_double_2 t2 = basis2_[i].get_double2();
      grads1 = -(e1n[1] - t1[1])/(e1n[0] - t1[0]);
      grads2 = -(e2n[1] - t2[1])/(e2n[0] - t2[0]);
      new_points1[i] = HomgPoint2D(params[i]*(1/grads1) + t1[0], params[i]*grads1 + t1[1], 1.0);
      new_points2[i] = HomgPoint2D(params[i]*(1/grads2) + t2[0], params[i]*grads2 + t2[1], 1.0);
    }
    FMatrixCompute7Point* computor = new FMatrixCompute7Point(true, true);
    vcl_vector<FMatrix*> ref;
    if (!computor->compute(new_points1, basis2_, ref))
      vcl_cerr << "FMatrixCompute7Point Failure\n";
    double final = 0.0;
    unsigned int num = 0;
    for (unsigned int l = 0; l < ref.size(); l++) {
      vnl_vector<double> res = calculate_residuals(ref[l]);
      double so_far = 0.0;
      for (unsigned int m = 0; m < res.size(); m++)
        so_far += res[m];
//      vcl_cerr << "so_far : " << so_far << vcl_endl;
      if (so_far < final) {
        final = so_far;
        num = l;
      }
    }
    return *ref[num];
  }
}

// Forms a map of the different rank-2 structures for the F Matrix
void FMatrixComputeNonLinear::get_plan(int &r1, int &c1, int &r2, int &c2) {
  switch(p_) {
    case 0 :
      c1 = 1;
      c2 = 2;
      break;
    case 1 :
      c1 = 0;
      c2 = 2;
      break;
    case 2 :
      c1 = 0;
      c2 = 1;
      break;
  }
  switch(q_) {
    case 0 :
      r1 = 1;
      r2 = 2;
      break;
    case 1 :
      r1 = 0;
      r2 = 2;
      break;
    case 2 :
      r1 = 0;
      r2 = 1;
      break;
  }
}

vnl_vector<double> FMatrixComputeNonLinear::calculate_residuals(FMatrix* F) {
  vnl_vector<double> fx(data_size_);
  vnl_matrix<double> f(3, 3, 0.0);
  F->get(&f);
  f /= f.rms();
#if 0
  vnl_matrix<double> ft = f.transpose();
  vnl_matrix<double> z(3, 3, 0.0);
  z.put(0, 0, 1.0);z.put(1, 1, 1.0);
  vnl_matrix<double> pre1 = ft*z*f;
  vnl_matrix<double> pre2 = f*z*ft;
#endif

  for (int i = 0; i < data_size_; i++) {
    HomgPoint2D one = points1_[i], two = points2_[i];
    double t = 0.0;
    HomgLine2D l1 = F->image2_epipolar_line(one);
    HomgLine2D l2 = F->image1_epipolar_line(two);
    t += HomgOperator2D::perp_dist_squared(two, l1);
    t += HomgOperator2D::perp_dist_squared(one, l2);
#if 0
    vnl_double_2 od = one.get_double2();
    vnl_double_2 td = two.get_double2();
    vnl_vector<double> oi(3, 1.0); oi[0]=od[0]; oi[1]=od[1];
    vnl_vector<double> ti(3, 1.0); ti[0]=td[0]; ti[1]=td[1];
    vnl_vector<double> p1 = ti*pre1;
    vnl_vector<double> p2 = oi*pre2;
    double p11 = p1[0]*oi[0] + p1[1]*oi[1] + p1[2]*oi[2];
    double p21 = p2[0]*ti[0] + p2[1]*ti[1] + p2[2]*ti[2];
    double factor = 1.0/ p11 + 1.0/ p21;
#endif
    fx[i] = t;//*factor;
  }
  return fx;
}
