#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include "sdet_curve_model.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include "sdet_sel_utils.h"
#include <bvgl/algo/bvgl_eulerspiral.h>

//*****************************************************************************//
// A simple Linear curve model
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_simple_linear_curve_model::sdet_simple_linear_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                                                 double dpos, double dtheta,
                                                                 double token_len, double /*max_k*/, double /*max_gamma*/,
                                                                 bool /*adaptive*/):
  sdet_linear_curve_model_base(ref_e->pt, ref_e->tangent), min_theta(0.0), max_theta(0.0)
{
  type = LINEAR;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len);
}

//: constructor 3: From the intersection of two curve bundles
sdet_simple_linear_curve_model::sdet_simple_linear_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2):
  sdet_linear_curve_model_base(), min_theta(0.0), max_theta(0.0)
{
  type = LINEAR;
  pt = ((sdet_linear_curve_model_base*)cm1)->pt;
  theta = ((sdet_linear_curve_model_base*)cm1)->theta; //default

  //compute new curve bundle
  sdet_intersect_angle_range(((sdet_simple_linear_curve_model*)cm1)->min_theta, ((sdet_simple_linear_curve_model*)cm1)->max_theta,
                              ((sdet_simple_linear_curve_model*)cm2)->min_theta, ((sdet_simple_linear_curve_model*)cm2)->max_theta,
                              min_theta, max_theta);


}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_simple_linear_curve_model* sdet_simple_linear_curve_model::intersect(sdet_simple_linear_curve_model* cm)
{
  return new sdet_simple_linear_curve_model(this, cm);
}

//: determine if edgel pair is legal
bool sdet_simple_linear_curve_model::edgel_pair_legal(sdet_int_params &params, double tan1, double tan2)
{
  //simple orientation based pruning
  bool ret = (sdet_dot(params.ref_dir, tan1)>0 && sdet_dot(params.ref_dir, tan2)>0);
  return ret;
}

//: compute the linear curve bundle for an edgel pair at the ref edgel
void sdet_simple_linear_curve_model::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double /*token_len*/)
{
  //determine the intrinsic parameters for this edgel pair
  sdet_int_params params = sdet_get_intrinsic_params(e1->pt, e2->pt, e1->tangent, e2->tangent);

  //do the orientation test
  if (edgel_pair_legal(params, e1->tangent, e2->tangent))
  {
    // predict the variation in the intrinsic parameters
    double alpha = std::asin(dpos/params.d);

    if (ref_e==e1)
    {
      // if the geometry is not valid (too close) for this computation, just assign all of dtheta
      // This is not strictly correct as a better bound can be dfined that does not include alpha
      // FIX ME!
      if (params.d < dpos){
        min_theta = sdet_angle0To2Pi(e1->tangent - dtheta);
        max_theta = sdet_angle0To2Pi(e1->tangent + dtheta);
        return;
      }

      //compute the three solution domains as computed in the notes
      double a1, b1, a2, b2;
      //domain 1 intersected with domain 2
      if (sdet_intersect_angle_range(params.t2-dtheta, params.t2+dtheta, -alpha, alpha, a1, b1)){
        //intersected with domain 3
        if (sdet_intersect_angle_range(a1, b1, params.t1-dtheta, params.t1+dtheta, a2, b2))
        {
          min_theta = sdet_angle0To2Pi(e1->tangent + a2 - params.t1);
          max_theta = sdet_angle0To2Pi(e1->tangent + b2 - params.t1);
        }
      }
    }
    else {
      // if the geometry is not valid (too close) for this computation, just assign all of dtheta
      // This is not strictly correct as a better bound can be dfined that does not include alpha
      // FIX ME!
      if (params.d < dpos){
        min_theta = sdet_angle0To2Pi(e2->tangent - dtheta);
        max_theta = sdet_angle0To2Pi(e2->tangent + dtheta);
        return;
      }

      //compute the three solution domains as computed in the notes
      double a1, b1, a2, b2;
      //domain 1 intersected with domain 2
      if (sdet_intersect_angle_range(params.t1-dtheta, params.t1+dtheta, -alpha, alpha, a1, b1)){
        //intersected with domain 3
        if (sdet_intersect_angle_range(a1, b1, params.t2-dtheta, params.t2+dtheta, a2, b2))
        {
          min_theta = sdet_angle0To2Pi(e2->tangent + a2 - params.t2);
          max_theta = sdet_angle0To2Pi(e2->tangent + b2 - params.t2);
        }
      }
    }
  }
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_simple_linear_curve_model::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  //for 1-d bundle: define theta as the middle of the domain
  if (max_theta<min_theta)
    theta = sdet_angle0To2Pi((max_theta+2*vnl_math::pi+min_theta)/2); //the range straddles the 0-crossing
  else
    theta = (min_theta + max_theta)/2;

  return {theta, 0};
}

//: function to check if the curve fit is reasonable
bool sdet_simple_linear_curve_model::curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* /*ref_e*/, double /*dpos*/)
{
  compute_best_fit(edgel_chain);
  return true;
}


//: report accuracy of measurement
void sdet_simple_linear_curve_model::report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
   //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = 0.0;    //curvature
  estimates[2] = 0.0;    //curvature derivative

  min_estimates[0] = min_theta;
  min_estimates[1] = 0.0;
  min_estimates[2] = 0.0;

  max_estimates[0] = max_theta;
  max_estimates[1] = 0.0;
  max_estimates[2] = 0.0;

}

//*****************************************************************************//
// Linear curve model
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_linear_curve_model::sdet_linear_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                                   double dpos, double dtheta,
                                                   double token_len, double /*max_k*/, double /*max_gamma*/,
                                                   bool /*adaptive*/):
sdet_linear_curve_model_base(ref_e->pt, ref_e->tangent), ref_pt(ref_e->pt), ref_theta(ref_e->tangent)
{
  type = LINEAR;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len);
}

//: constructor 3: From the intersection of two curve bundles
sdet_linear_curve_model::sdet_linear_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2):
sdet_linear_curve_model_base()
{
  type = LINEAR;

  ref_pt = ((sdet_linear_curve_model*)cm1)->ref_pt;
  ref_theta = ((sdet_linear_curve_model*)cm1)->ref_theta;

  pt = ((sdet_linear_curve_model_base*)cm1)->pt;
  theta = ((sdet_linear_curve_model_base*)cm1)->theta; //default

  //compute new curve bundle
  cv_bundle = vgl_clip(((sdet_linear_curve_model*)cm1)->cv_bundle, ((sdet_linear_curve_model*)cm2)->cv_bundle, vgl_clip_type_intersect);
}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_linear_curve_model* sdet_linear_curve_model::intersect(sdet_linear_curve_model * cm)
{
  return new sdet_linear_curve_model(this, cm);
}

//: determine if edgel pair is legal
bool sdet_linear_curve_model::edgel_pair_legal(sdet_int_params &params, double tan1, double tan2)
{
  //simple orientation based pruning
  bool ret = (sdet_dot(params.ref_dir, tan1)>0 && sdet_dot(params.ref_dir, tan2)>0);
  return ret;
}

//: compute the linear curve bundle for an edgel pair at the ref edgel
void sdet_linear_curve_model::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double /*token_len*/)
{
  //determine the intrinsic parameters for this edgel pair
  sdet_int_params params = sdet_get_intrinsic_params(e1->pt, e2->pt, e1->tangent, e2->tangent);

  //use -pi - pi range for the intrinsic params
  if (params.t1>vnl_math::pi)
    params.t1-= 2*vnl_math::pi;
  if (params.t2>vnl_math::pi)
    params.t2-= 2*vnl_math::pi;

  // predict the variation in the intrinsic parameters
  double alpha = std::asin(dpos/params.d);

  double omega_coords[] = { -dpos,-dtheta,  -dpos,dtheta,  dpos,dtheta,  dpos,-dtheta };
  vgl_polygon<double> omega(omega_coords, 4);

  if (ref_e==e1)
  {
    // if the geometry is not valid (too close) for this computation, just assign all of dx and dt(omega)
    if (params.d < dpos){
      cv_bundle = omega;
      return;
    }

    //compute the three solution domains as computed in the notes
    double a1, b1, a2, b2;
    //domain 1 intersected with domain 2
    if (sdet_intersect_angle_range_mpi_pi(-dtheta, dtheta, -params.t1-alpha, -params.t1+alpha, a1, b1)){
      //intersected with domain 3
      if (sdet_intersect_angle_range_mpi_pi(a1, b1, params.t2-params.t1-dtheta, params.t2-params.t1+dtheta, a2, b2))
      {
        cv_bundle.new_sheet();
        cv_bundle.push_back( dpos, a2-alpha);
        cv_bundle.push_back(-dpos, a2+alpha);
        cv_bundle.push_back(-dpos, b2+alpha);
        cv_bundle.push_back( dpos, b2-alpha);

        //intersect this with the default bundle to get the final bundle
        cv_bundle = vgl_clip(cv_bundle, omega, vgl_clip_type_intersect);
      }
    }
  }
  else {
    // if the geometry is not valid (too close) for this computation, just assign all of dx and dt(omega)
    if (params.d < dpos){
      cv_bundle = omega;
      return;
    }

    //compute the three solution domains as computed in the notes
    double a1, b1, a2, b2;
    //domain 1 intersected with domain 2
    if (sdet_intersect_angle_range_mpi_pi(-dtheta, dtheta, -params.t2-alpha, -params.t2+alpha, a1, b1)){
      //intersected with domain 3
      if (sdet_intersect_angle_range_mpi_pi(a1, b1, params.t1-params.t2-dtheta, params.t1-params.t2+dtheta, a2, b2))
      {
        cv_bundle.new_sheet();
        cv_bundle.push_back( dpos, a2+alpha);
        cv_bundle.push_back(-dpos, a2-alpha);
        cv_bundle.push_back(-dpos, b2-alpha);
        cv_bundle.push_back( dpos, b2+alpha);

        //intersect this with the default bundle to get the final bundle
        cv_bundle = vgl_clip(cv_bundle, omega, vgl_clip_type_intersect);
      }
    }
  }
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_linear_curve_model::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  //assign the rough centroid of this bundle as the best estimate
  double dx = 0.0;
  double dt = 0.0;
  for (auto & i : cv_bundle[0]){
    dx += i.x();
    dt += i.y();
  }
  dx /= cv_bundle.num_vertices();
  dt /= cv_bundle.num_vertices();

  //extrinsic estimates from the centroid of the bundle
  pt = vgl_point_2d<double>(ref_pt.x()+dx*std::cos(ref_theta+vnl_math::pi_over_2), ref_pt.y()+dx*std::sin(ref_theta+vnl_math::pi_over_2));
  theta = ref_theta + dt;

  return {dx, dt};
}

//: function to check if the curve fit is reasonable
bool sdet_linear_curve_model::curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* /*ref_e*/, double /*dpos*/)
{
  compute_best_fit(edgel_chain);
  return true;
}


//: report accuracy of measurement
void sdet_linear_curve_model::report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
  double d_min=1000.0, d_max=1000.0, dt_min=1000.0, dt_max=-1000.0;

  for (auto & i : cv_bundle[0])
  {
    if (i.x()<d_min) d_min = i.x();
    if (i.x()>d_max) d_max = i.x();
    if (i.y()<dt_min) dt_min = i.y();
    if (i.y()>dt_max) dt_max = i.y();
  }

  //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = 0.0;    //curvature
  estimates[2] = 0.0;    //curvature derivative

  min_estimates[0] = dt_min+ref_theta;
  min_estimates[1] = 0.0;
  min_estimates[2] = 0.0;

  max_estimates[0] = dt_max+ref_theta;
  max_estimates[1] = 0.0;
  max_estimates[2] = 0.0;

}


//*****************************************************************************//
// Circular Arc curve model
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_CC_curve_model::sdet_CC_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                           double dpos, double dtheta,
                                           double token_len, double max_k, double /*max_gamma*/,
                                           bool /*adaptive*/):
  pt(ref_e->pt), tangent(ref_e->tangent), theta(0), k(0.0)
{
  type = CC;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k);
}

//: constructor 3: From the intersection of two curve bundles
sdet_CC_curve_model::sdet_CC_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2)
{
  type = CC;
  pt = ((sdet_CC_curve_model*)cm1)->pt;
  tangent = ((sdet_CC_curve_model*)cm1)->tangent;

  cv_bundle = vgl_clip(((sdet_CC_curve_model*)cm1)->cv_bundle, ((sdet_CC_curve_model*)cm2)->cv_bundle, vgl_clip_type_intersect);
}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_CC_curve_model* sdet_CC_curve_model::intersect(sdet_CC_curve_model* cm)
{
  return new sdet_CC_curve_model(this, cm);
}

//: determine if edgel pair is legal
bool sdet_CC_curve_model::edgel_pair_legal(sdet_int_params &params, double tan1, double tan2)
{
  //simple orientation based pruning
  bool ret = (sdet_dot(params.ref_dir, tan1)>0 && sdet_dot(params.ref_dir, tan2)>0);
  return ret;
}

//: compute the CC curve bundle for an edgel pair at the ref edgel
void sdet_CC_curve_model::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double token_len, double max_k)
{
  //determine the intrinsic parameters for this edgel pair
  sdet_int_params params = sdet_get_intrinsic_params(e1->pt, e2->pt, e1->tangent, e2->tangent);

  //and compute the curve bundle using the intrinsic params
  compute_curve_bundle(cv_bundle, params, ref_e==e1, dpos, dtheta, token_len, max_k);

}

//: compute the CC curve bundle for an edgel pair given intrinsic params
void sdet_CC_curve_model::
compute_curve_bundle(vgl_polygon<double>& bundle, sdet_int_params &params,
                     bool first_is_ref, double dpos, double dtheta,
                     double token_len, double max_k)
{
  //use (-pi, pi] range for the intrinsic params
  if (params.t1>vnl_math::pi)
    params.t1-= 2*vnl_math::pi;
  if (params.t2>vnl_math::pi)
    params.t2-= 2*vnl_math::pi;

  //compute the curvature bounds due to the token length and dpos
  double dk = 2*dpos/(token_len*token_len/4.0 + dpos*dpos);

  //create the default bundle (omega: the restricted curve bundle space)
  vgl_polygon<double> omega;

  max_k = std::min(max_k, dk);

  omega.new_sheet();
  omega.push_back(-(dtheta+1e-3), -max_k);
  omega.push_back( (dtheta+1e-3), -max_k);
  omega.push_back( (dtheta+1e-3), +max_k);
  omega.push_back(-(dtheta+1e-3), +max_k);

  // if the geometry is not valid (too close) for this computation, just assign omega as the curve bundle
  if (params.d < dpos){
    bundle = omega;
    return;
  }

  // compute the curve bundle due to specified perturbations
  vgl_polygon<double> new_bundle;

  if (first_is_ref)
  {
    // predict the variation in the intrinsic parameters
    double alpha = std::asin(dpos/params.d);

    double eps = 1e-5;
    double dphi_min = std::max((-params.t2-dtheta-2*alpha)-params.t1 +eps, -dtheta);
    double dphi_max = std::min((-params.t2+dtheta+2*alpha)-params.t1 -eps, dtheta);

    //make sure curve bundle is legal
    if (dphi_max < dphi_min)
        return;

    //form the curve bundle polygon from these coordinates
    new_bundle.new_sheet();

    //form the polygon with 10 pts
    for (int i=0; i<5; i++)
    {
      double dphi = dphi_min + (dphi_max - dphi_min)*i/4.0;

      double t1 = params.t1 + dphi;
      double t1_min = (t1-params.t2-dtheta)/2.0;
      double t1_max = (t1-params.t2+dtheta)/2.0;

      double t1_sol_min, t1_sol_max;
      sdet_intersect_angle_range_mpi_pi(t1_min, t1_max, t1-alpha, t1+alpha, t1_sol_min , t1_sol_max);

      double k_min = 2*sin(-t1_sol_min)/params.d;
      double th = dphi;

      new_bundle.push_back(th, k_min);
    }

    for (int i=4; i>=0; i--)
    {
      double dphi = dphi_min + (dphi_max - dphi_min)*i/4.0;

      double t1 = params.t1 + dphi;
      double t1_min = (t1-params.t2-dtheta)/2.0;
      double t1_max = (t1-params.t2+dtheta)/2.0;

      double t1_sol_min, t1_sol_max;
      sdet_intersect_angle_range_mpi_pi(t1_min, t1_max, t1-alpha, t1+alpha, t1_sol_min , t1_sol_max);

      double k_max = 2*sin(-t1_sol_max)/params.d;
      double th = dphi;

      new_bundle.push_back(th, k_max);
    }
  }
  else //e2 is the ref
  {
    // predict the variation in the intrinsic parameters
    double alpha = std::asin(dpos/params.d);

    double eps = 1e-5;
    double dphi_min = std::max((-params.t1-dtheta-2*alpha)-params.t2 +eps, -dtheta);
    double dphi_max = std::min((-params.t1+dtheta+2*alpha)-params.t2 -eps,  dtheta);

    //make sure curve bundle is legal
    if (dphi_max < dphi_min)
        return;

    //form the curve bundle polygon from these coordinates
    new_bundle.new_sheet();

    //form the polygon with 10 pts
    for (int i=0; i<5; i++)
    {
      double dphi = dphi_min + (dphi_max - dphi_min)*i/4.0;

      double t2 = params.t2 + dphi;
      double t2_min = (t2-params.t1-dtheta)/2.0;
      double t2_max = (t2-params.t1+dtheta)/2.0;

      double t2_sol_min, t2_sol_max;
      sdet_intersect_angle_range_mpi_pi(t2_min, t2_max, t2-alpha, t2+alpha, t2_sol_min , t2_sol_max);

      double k_min = 2*sin(t2_sol_min)/params.d;
      double th = dphi;

      new_bundle.push_back(th, k_min);
    }

    for (int i=4; i>=0; i--)
    {
      double dphi = dphi_min + (dphi_max - dphi_min)*i/4.0;

      double t2 = params.t2 + dphi;
      double t2_min = (t2-params.t1-dtheta)/2.0;
      double t2_max = (t2-params.t1+dtheta)/2.0;

      double t2_sol_min, t2_sol_max;
      sdet_intersect_angle_range_mpi_pi(t2_min, t2_max, t2-alpha, t2+alpha, t2_sol_min , t2_sol_max);

      double k_max = 2*sin(t2_sol_max)/params.d;
      double th = dphi;

      new_bundle.push_back(th, k_max);
    }
  }

  //intersect the new bundle with omega to get the restricted bundle
  bundle = vgl_clip(omega, new_bundle);
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_CC_curve_model::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  //assign the rough centroid of this bundle as the best estimate
  double dt = 0.0;
  k = 0.0;
  for (auto & i : cv_bundle[0]){
    dt += i.x();
    k += i.y();
  }
  dt /= cv_bundle.num_vertices();
  k /= cv_bundle.num_vertices();

  theta = tangent + dt; //compute extrinsic tangent from dt

  return {dt, k};
}

//: function to check if the curve fit is reasonable
bool sdet_CC_curve_model::curve_fit_is_reasonable(std::deque<sdet_edgel*> & edgel_chain, sdet_edgel* /*ref_e*/, double /*dpos*/)
{
  compute_best_fit(edgel_chain);
  return true;
}

//: report accuracy of measurement
void sdet_CC_curve_model::report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
  double theta_min=1000.0, theta_max=-1000.0, k_min=1000.0, k_max=-1000.0;

  for (auto & i : cv_bundle[0])
  {
    if (i.x()<theta_min) theta_min = i.x();
    if (i.x()>theta_max) theta_max = i.x();
    if (i.y()<k_min) k_min = i.y();
    if (i.y()>k_max) k_max = i.y();
  }

  //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = k;      //curvature
  estimates[2] = 0.0;    //curvature derivative

  min_estimates[0] = theta_min;
  min_estimates[1] = k_min;
  min_estimates[2] = 0.0;

  max_estimates[0] = theta_max;
  max_estimates[1] = k_max;
  max_estimates[2] = 0.0;

}

//: print info
void sdet_CC_curve_model::print_info()
{
  std::cout << " : (th=" << theta << ", k=" << k << ", gamma=0.0 ";
}

//*****************************************************************************//
// Circular Arc curve model (new) with curvature classes
//*****************************************************************************//

//: copy constructor
sdet_CC_curve_model_new::sdet_CC_curve_model_new(const sdet_CC_curve_model_new& other) :
  sdet_curve_model(other)
{
  for (unsigned i=0; i<NkClasses; i++)
    cv_bundles[i] = other.cv_bundles[i];
  ref_pt = other.ref_pt;
  ref_theta = other.ref_theta;

  pt = other.pt;
  theta = other.theta;
  k = other.k;
}

  //: Constructor 1: From a pair of edgels
sdet_CC_curve_model_new::
sdet_CC_curve_model_new(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                         double dpos, double dtheta,
                         double token_len, double max_k, double /*max_gamma*/,
                         bool /*adaptive*/):
  ref_pt(ref_e->pt), ref_theta(ref_e->tangent), pt(0.0,0.0), theta(0), k(0.0)
{
  type = CC2;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k);
}

//: constructor 3: From the intersection of two curve bundles
sdet_CC_curve_model_new::
sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm1, sdet_CC_curve_model_new* cm2):
  pt(0.0,0.0), theta(0), k(0.0)
{
  type = CC2;

  for (int i=0; i<NkClasses; i++)
  {
    cv_bundles[i] = vgl_clip(cm1->cv_bundles[i], cm2->cv_bundles[i], vgl_clip_type_intersect);
    if (cv_bundles[i].num_sheets()>1)
      cv_bundles[i].clear(); //remove these erroneous curve bundles
  }

  ref_pt = cm1->ref_pt;
  ref_theta = cm1->ref_theta;
}

//: constructor 4: By transporting a curve model to another extrinsic anchor point
sdet_CC_curve_model_new::
sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm, vgl_point_2d<double> pt, double theta):
  pt(0.0,0.0), theta(0), k(0.0)
{
  type = CC2;

  //identical reference point so just make a copy of the CBs
  if (pt==cm->ref_pt && theta==cm->ref_theta){
    for (unsigned i=0; i<NkClasses; i++)
      cv_bundles[i] = cm->cv_bundles[i];
  }

  //use the ref of the given curve model for transportation
  ref_pt = cm->ref_pt;
  ref_theta = cm->ref_theta;

  //transport each bundle
  for (int i=0; i<NkClasses; i++){
    bool sgn_change;
    vgl_polygon<double> poly =  transport_bundle_to(cm->cv_bundles[i], sdet_k_classes[i], pt, theta, sgn_change);
    if (sgn_change)
      cv_bundles[NkClasses-1-i] = poly;
    else
      cv_bundles[i] = poly;
  }

  //Now update the extrinsic refs
  ref_pt = pt;
  ref_theta = theta;

}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_CC_curve_model_new* sdet_CC_curve_model_new::intersect(sdet_CC_curve_model_new* cm)
{
  assert(cm->type == CC2);
  return new sdet_CC_curve_model_new(this, cm);
}

//: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
sdet_curve_model* sdet_CC_curve_model_new::transport(vgl_point_2d<double> pt, double theta)
{
  return new sdet_CC_curve_model_new(this, pt, theta);
}

//: construct and return a curve model of the same type by looking for legal transitions with another curve bundle
sdet_curve_model* sdet_CC_curve_model_new::transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double & bundle_size)
{
  if (cm->type!=CC2)
    return nullptr; //type mismatch

  auto* next_cm = (sdet_CC_curve_model_new*)cm; //typecast for easy handling

  //now determine which transition is most likely
  bundle_size = 0;
  trans_k = -1;
  for (int i=0; i<NkClasses; i++){
    if (cv_bundles[i].num_sheets()!=1) continue;

    double size_i = 0;
    for (int j=0; j<NkClasses; j++){
      if (i==j) continue; //model continuations not transitions

      if (next_cm->cv_bundles[j].num_sheets()!=1) continue;

      //try intersecting the bundles to see if there are any legal ones
      vgl_polygon<double> cb = vgl_clip(cv_bundles[i], next_cm->cv_bundles[j], vgl_clip_type_intersect);
      if (cb.num_sheets()==1)
        size_i += vgl_area(cb);
    }
    if (size_i>bundle_size){
      bundle_size = size_i;
      trans_k = i;
    }
  }

  if (trans_k>=0){ //if any legal ones
    //construct a curve model from the best transition
    auto* new_cm = new sdet_CC_curve_model_new(ref_pt, ref_theta);

    for (int j=0; j<NkClasses; j++)
      new_cm->cv_bundles[j] = vgl_clip(cv_bundles[trans_k], next_cm->cv_bundles[j], vgl_clip_type_intersect);

    trans_type = 1;
    return new_cm;
  }

  //If no C^1 transitions are possible, look for possible C^0 transitions
  bundle_size = 0;
  trans_k = -1;
  for (int i=0; i<NkClasses; i++){
    if (cv_bundles[i].num_sheets()!=1) continue;

    //we need to construct new bundles for CB1 which only constrains one of the parameters (dx)
    //determine the bounds on dx
    double dx_min=100, dx_max=-100;
    for (auto & p : cv_bundles[i][0]){
      if (p.x()<dx_min) dx_min = p.x();
      if (p.x()>dx_max) dx_max = p.x();
    }
    //construct a polygon with these bounds
    vgl_polygon<double> cb1(1); cb1.push_back(dx_min, -0.5);cb1.push_back(dx_max, -0.5);cb1.push_back(dx_max, 0.5);cb1.push_back(dx_min, 0.5);

    double size_i = 0;
    for (int j=0; j<NkClasses; j++){
      if (i==j) continue; //model continuations not transitions

      if (next_cm->cv_bundles[j].num_sheets()!=1) continue;

      //try intersecting the bundles to see if there are any legal ones
      vgl_polygon<double> cb = vgl_clip(cb1, next_cm->cv_bundles[j], vgl_clip_type_intersect);
      if (cb.num_sheets()==1)
        size_i += vgl_area(cb);
    }
    if (size_i>bundle_size){
      bundle_size = size_i;
      trans_k = i;
    }
  }

  if (trans_k>=0){ //if any legal ones
    //construct a curve model from the best transition
    auto* new_cm = new sdet_CC_curve_model_new(ref_pt, ref_theta);

    //construct the constraint box for the best model
    double dx_min=100, dx_max=-100;
    for (auto & p : cv_bundles[trans_k][0]){
      if (p.x()<dx_min) dx_min = p.x();
      if (p.x()>dx_max) dx_max = p.x();
    }
    //construct a polygon with these bounds
    vgl_polygon<double> cb1(1); cb1.push_back(dx_min, -0.5);cb1.push_back(dx_max, -0.5);cb1.push_back(dx_max, 0.5);cb1.push_back(dx_min, 0.5);

    for (int j=0; j<NkClasses; j++)
      new_cm->cv_bundles[j] = vgl_clip(cb1, next_cm->cv_bundles[j], vgl_clip_type_intersect);

    trans_type = 0;
    return new_cm;
  }

  return nullptr; // no transition possible
}

//: construct and return a curve bundle of the same type by looking for legal transitions with another curve bundle
sdet_curve_model* sdet_CC_curve_model_new::consistent_transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double &gc_cost)
{
  if (cm->type!=CC2)
    return nullptr; //type mismatch

  auto* next_cm = (sdet_CC_curve_model_new*)cm; //typecast for easy handling

  //now determine which transition is most likely
  gc_cost = 1000.0; //compatibility cost
  trans_k = -1;     //best transition

  //try all combinations of transitions (C^2 and C^1)
  for (int i=0; i<NkClasses; i++)
  {
    if (cv_bundles[i].num_sheets()!=1) continue; //only legal ones

    //for C^0 transition, we need to construct new bundles for CB1 which only constrains one of the parameters (dx)
    //determine the bounds on dx
    double dx_min=100, dx_max=-100;
    for (auto & p : cv_bundles[i][0]){
      if (p.x()<dx_min) dx_min = p.x();
      if (p.x()>dx_max) dx_max = p.x();
    }
    //construct a polygon with these bounds
    vgl_polygon<double> cb1(1); cb1.push_back(dx_min, -0.5);cb1.push_back(dx_max, -0.5);cb1.push_back(dx_max, 0.5);cb1.push_back(dx_min, 0.5);

    for (int j=0; j<NkClasses; j++)
    {
      if (next_cm->cv_bundles[j].num_sheets()!=1) continue; //only legal ones

      //try intersecting the bundles to see if there are any legal ones (C^2 and C^1 transitions)
      vgl_polygon<double> cb = vgl_clip(cv_bundles[i], next_cm->cv_bundles[j], vgl_clip_type_intersect);

      double cost=1000.0;
      int type=-1;

      if (cb.num_sheets()==1){
        cost = std::fabs(sdet_k_classes[i]-sdet_k_classes[j]);
        if (i==j) type = 2;
        else      type = 1;
      }
      else { //no legal bundle intersection so try C^0 intersection
        type = 0;

        vgl_polygon<double> cb = vgl_clip(cb1, next_cm->cv_bundles[j], vgl_clip_type_intersect);

        if (cb.num_sheets()==1){
          double dt1_min=100, dt1_max=-100, dt2_min=100, dt2_max=-100;
          for (auto & p : cb[0]){
            if (p.y()<dt1_min) dt1_min = p.y();
            if (p.y()>dt1_max) dt1_max = p.y();
          }
          for (auto & p : cv_bundles[i][0]){
            if (p.y()<dt2_min) dt2_min = p.y();
            if (p.y()>dt2_max) dt2_max = p.y();
          }
          double dtheta = std::min(std::fabs(dt1_min-dt2_max), std::fabs(dt2_min-dt1_max));

          cost = std::fabs(sdet_k_classes[i]-sdet_k_classes[j]) +
                 dtheta;
        }
      }

      //record transition if this is the best one
      if (cost<gc_cost){
          gc_cost = cost;
          trans_k = i;
          trans_type = type;
      }
    }
  }

  // if any of the combinations were legal, construct a curve model from the best transition
  if (trans_k>=0){
    auto* new_cm = new sdet_CC_curve_model_new(ref_pt, ref_theta);

    if (trans_type==2){//C^2
      for (int i=0; i<NkClasses; i++)
      {
        new_cm->cv_bundles[i] = vgl_clip(cv_bundles[i], next_cm->cv_bundles[i], vgl_clip_type_intersect);
        if (new_cm->cv_bundles[i].num_sheets()>1)
          new_cm->cv_bundles[i].clear(); //remove these erroneous curve bundles
      }
    }
    else if (trans_type==1){ //C^1
      for (int j=0; j<NkClasses; j++)
        new_cm->cv_bundles[j] = vgl_clip(cv_bundles[trans_k], next_cm->cv_bundles[j], vgl_clip_type_intersect);
    }
    else if (trans_type==0){ //C^0
      //construct the constraint box for the best model
      double dx_min=100, dx_max=-100;
      for (auto & p : cv_bundles[trans_k][0]){
        if (p.x()<dx_min) dx_min = p.x();
        if (p.x()>dx_max) dx_max = p.x();
      }
      //construct a polygon with these bounds
      vgl_polygon<double> cb1(1); cb1.push_back(dx_min, -0.5);cb1.push_back(dx_max, -0.5);cb1.push_back(dx_max, 0.5);cb1.push_back(dx_min, 0.5);

      for (int j=0; j<NkClasses; j++)
        new_cm->cv_bundles[j] = vgl_clip(cb1, next_cm->cv_bundles[j], vgl_clip_type_intersect);
    }
    return new_cm;
  }

  return nullptr; // no transition possible
}

//: construct and return a curve bundle of the same type by looking for legal C1 transitions with another curve bundle
sdet_curve_model* sdet_CC_curve_model_new::C1_transition(sdet_curve_model* cm)
{
  //Note: The goal is to find any and all legal C1 transitions between the two curvelets
  // methodology: take the union of all cbs of cvlet1 and intersect it with the cbs of cvlet2

  if (cm->type!=CC2)
    return nullptr; //type mismatch

  auto* next_cm = (sdet_CC_curve_model_new*)cm; //typecast for easy handling

  //first take the union of the curve bundles of the current cvlet
  vgl_polygon<double> cum_cb;
  for (auto & cv_bundle : cv_bundles)
  {
    if (cv_bundle.num_sheets()!=1) continue; //only legal ones

    //take the union of the cbs
    cum_cb = vgl_clip(cum_cb, cv_bundle, vgl_clip_type_union);
  }

  if (cum_cb.num_sheets()!=1)  //sanity check
    return nullptr;

  //intersect the cumulative cb with all the curve bundles of cvlet2
  auto* new_cm = new sdet_CC_curve_model_new(next_cm->ref_pt, next_cm->ref_theta);

  for (int j=0; j<NkClasses; j++)
    new_cm->cv_bundles[j] = vgl_clip(cum_cb, next_cm->cv_bundles[j], vgl_clip_type_intersect);

  return new_cm;
}

//: is this bundle valid?
bool sdet_CC_curve_model_new::bundle_is_valid()
{
  bool valid = false;

  for (auto & cv_bundle : cv_bundles){
    if (cv_bundle.num_sheets()>1){
      //delete all except the largest one (risky business)
      double la = 0;
      vgl_polygon<double>::sheet_t ls;
      for (unsigned j=0; j<cv_bundle.num_sheets(); j++){
        double a = vgl_area(vgl_polygon<double>(cv_bundle[j]));
        if (a>la) {
          la = a;
          ls = cv_bundle[j];
        }
      }

      //delete all sheets except the largest one
      cv_bundle.clear();
      cv_bundle.push_back(ls);
    }
    valid = valid || (cv_bundle.num_sheets()==1);
  }
  return valid;
}

//: determine if edgel pair is legal
bool sdet_CC_curve_model_new::edgel_pair_legal(sdet_int_params &params, double tan1, double tan2)
{
  //simple orientation based pruning
  bool ret = (sdet_dot(params.ref_dir, tan1)>0 && sdet_dot(params.ref_dir, tan2)>0);
  return ret;
}

//: compute the CC curve bundle for an edgel pair at the ref edgel
void sdet_CC_curve_model_new::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double token_len, double /*max_k*/)
{
  //compute the curvature bounds due to the token length and dpos
  double dk = 2*dpos/(token_len*token_len/4.0 + dpos*dpos);

  //1) construct the bundle at the ref edgel and the other edgel

  // create the default bundle (omega: the restricted curve bundle space)
  // (currently assuming independence of the parameters)
  vgl_polygon<double> omega;
  omega.new_sheet();
  omega.push_back(-dpos, -dtheta);
  omega.push_back( dpos, -dtheta);
  omega.push_back( dpos, -dtheta/2.0);
  omega.push_back( dpos,  0);
  omega.push_back( dpos, +dtheta/2.0);
  omega.push_back( dpos, +dtheta);
  omega.push_back(-dpos, +dtheta);
  omega.push_back(-dpos, +dtheta/2.0);
  omega.push_back(-dpos,  0);
  omega.push_back(-dpos, -dtheta/2.0);

  sdet_edgel* neigh_e=nullptr; //relic of older design req.
  if (ref_e == e1)
    neigh_e = e2;
  else
    neigh_e = e1;

  //for each curve class
  for (int i=0; i<NkClasses; i++){
    if (std::fabs(sdet_k_classes[i])<dk){ //only the legal curve classes

      //2) transport the bundle from the other edgel to the current edgel
      bool sgn_change;
      vgl_polygon<double> trans_CB = transport_bundle_from(omega, sdet_k_classes[i], neigh_e->pt, neigh_e->tangent, sgn_change);

      //3) intersect the two bundles to produce the pairwise curve bundle at the ref edgel
      //      If there was a sign change during transportation of the bundle,
      //      assign this bundle to the bundle corresponding to the reversed sign
      if (sgn_change){
        cv_bundles[NkClasses-1-i] = vgl_clip(omega, trans_CB);//reverse index
      }
      else {
        cv_bundles[i] = vgl_clip(omega, trans_CB);
      }
    }
  }
}

//: Transport curve bundle from the given extrinsic point-tangent to the current point-tanget for the given curve model
vgl_polygon<double> sdet_CC_curve_model_new::
transport_bundle_from(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta, bool & sgn_change)
{
  return transport_bundle(cv_bundle, k, pt, theta, ref_pt, ref_theta, sgn_change);
}

//: Transport curve bundle from the given extrinsic point-tangent to the current point-tanget for the given curve model
vgl_polygon<double> sdet_CC_curve_model_new::
transport_bundle_to(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta, bool & sgn_change)
{
  return transport_bundle(cv_bundle, k, ref_pt, ref_theta, pt, theta, sgn_change);
}

//: Transport a curve bundle from one anchor to the current anchor
vgl_polygon<double> sdet_CC_curve_model_new::
transport_bundle(const vgl_polygon<double> & cv_bundle, double k,
                 vgl_point_2d<double> spt, double stheta, vgl_point_2d<double> dpt, double dtheta, bool &sgn_change)
{
  vgl_polygon<double> trans_CB; //transported curve bundle

  //make sure that the incoming curve bundle is valid
  if (cv_bundle.num_sheets()!=1) return trans_CB;

  // transport the hull of the bundle
  trans_CB.new_sheet();
  std::vector<bool> sign(cv_bundle[0].size()); //keep track of the sign changes
  for (unsigned i=0; i<cv_bundle[0].size(); i++){
    bool sgn;
    trans_CB.push_back(transport_CC(cv_bundle[0][i], k, spt, stheta, dpt, dtheta, sgn));
    sign[i] = sgn;
  }

  //now determine if the CB has been split, if so reverse the reverses to
  //keep the bundle intact
  bool sgn_temp = sign[0];
  bool sgn_inconsistent = false;
  for (unsigned i=1; i<sign.size(); i++){
    if (sign[i] != sgn_temp){
      sgn_inconsistent = true;
      break;
    }
  }

  if (sgn_inconsistent){ //signs changed inconsistently
    for (unsigned i=0; i<sign.size(); i++){
      if (sign[i]){
        trans_CB[0][i].x() = -trans_CB[0][i].x();                  // reverse dx
        if (trans_CB[0][i].y()>0)                                  // reverse dt
          trans_CB[0][i].y() =  trans_CB[0][i].y() - vnl_math::pi;
        else
          trans_CB[0][i].y() =  trans_CB[0][i].y() + vnl_math::pi;
      }
    }
    sgn_change = false;
  }
  else
    sgn_change = sign[0]; //sign was consistent so just pick one

  return trans_CB;
}

//: Transport given curve from the given extrinsic point-tangent to the current point-tangent
//: Transport a given curve from one anchor to another
vgl_point_2d<double> sdet_CC_curve_model_new::transport_CC(vgl_point_2d<double>dx_dt, double k,
                                                            vgl_point_2d<double> spt, double stheta,
                                                            vgl_point_2d<double> dpt, double dtheta,
                                                            bool & sgn_change)
{
  double DX = dpt.x()-spt.x();
  double DY = dpt.y()-spt.y();
  double dx1 = dx_dt.x();
  double dt1 = dx_dt.y();

  double tT1 = stheta + dt1;
  double T2 = dtheta;

  sgn_change = false;

  //compute transported model (dx2, dt2)
  double dx2, dt2;

  if (std::fabs(k)<1e-5) //constant curvature case:
  {
    dx2 = dx1 + (DX*-std::sin(tT1)+DY*std::cos(tT1));
    dt2 = tT1-T2;
  }
  else { //regular arc

    double a = DX - (dx1-1/k)*std::sin(tT1);
    double b = DY + (dx1-1/k)*std::cos(tT1);

    double tT2 = std::atan2(a, -b);

    //// alternate solutions based on simple algebra
    //double dx2a = 1/k - A;
    //double dx2b = i/k + A;

    // alternate solutions (based on tT2)
    double dx2a, dx2b;
    if (std::fabs(std::sin(tT2))>1e-5){
      dx2a = +1/k - a/std::sin(tT2);
      dx2b = -1/k - a/std::sin(tT2); //assume tT2 is backwards
    }
    else {
      dx2a = +1/k - b/std::cos(tT2);
      dx2b = -1/k - b/std::cos(tT2); //assume tT2 is backwards
    }

    if (std::fabs(dx2a) < std::fabs(dx2b)){
      dx2 = dx2a;
    }
    else {
      dx2 = dx2b;
      sgn_change = true;
    }
    dt2 = tT2 - T2;
  }

  //correct dt2 to keep it within the -pi/2, pi/2 limits
  if (dt2 < -vnl_math::pi) dt2 += 2*vnl_math::pi;
  if (dt2 >  vnl_math::pi) dt2 -= 2*vnl_math::pi;

  // keep dt2 between -pi/2 and pi/2 (reverse curvature signs if reversed)
  if (dt2 < -vnl_math::pi_over_2){
    dt2 += vnl_math::pi;
    dx2 = -dx2;
    sgn_change = !sgn_change;
  }

  if (dt2 > vnl_math::pi_over_2){
    dt2 -= vnl_math::pi;
    dx2 = -dx2;
    sgn_change = !sgn_change;
  }

  return {dx2, dt2};
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double> sdet_CC_curve_model_new::compute_best_fit()
{
  // Choose the curve class with the least curvature and
  // assign the centroid of its bundle as the best fit curve
  int best_k_ind = -1;
  double least_cost=10.0;
  for (int i=0; i<NkClasses; i++){
    if (cv_bundles[i].num_sheets() != 0 && std::fabs(sdet_k_classes[i])<least_cost){
      best_k_ind=i;
      least_cost = std::fabs(sdet_k_classes[i]);
    }
  }

  if (best_k_ind>-1)
  {
    //compute the centroid of this bundle
    double dx = 0.0;
    double dt = 0.0;
    for (auto & j : cv_bundles[best_k_ind][0]){
      dx += j.x();
      dt += j.y();
    }
    dx /= cv_bundles[best_k_ind].num_vertices();
    dt /= cv_bundles[best_k_ind].num_vertices();

    //compute the extrinsic point and tangent of the centroid
    pt = ref_pt + vgl_vector_2d<double>(-dx*std::cos(ref_theta+dt+vnl_math::pi/2),
                                        -dx*std::sin(ref_theta+dt+vnl_math::pi/2));
    theta = sdet_angle0To2Pi(ref_theta + dt);
    k = sdet_k_classes[best_k_ind];

    return {dx, dt};
  }

  return {0,0}; //no optimal, just the ref
}


//: Compute the best fit curve from the curve bundle
vgl_point_2d<double> sdet_CC_curve_model_new::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  return compute_best_fit(); //no use for the edgel chain in this function
}

//: Set the best fit curve
void sdet_CC_curve_model_new::set_best_fit(vgl_point_2d<double> dx_dt, double kk)
{
  double dx = dx_dt.x();
  double dt = dx_dt.y();

  //compute the extrinsic point and tangent of the centroid
  pt = ref_pt + vgl_vector_2d<double>(-dx*std::cos(ref_theta+dt+vnl_math::pi/2),
                                      -dx*std::sin(ref_theta+dt+vnl_math::pi/2));
  theta = sdet_angle0To2Pi(ref_theta + dt);

  k= kk;
}

//: function to check if the curve fit is reasonable
bool sdet_CC_curve_model_new::
curve_fit_is_reasonable(std::deque<sdet_edgel*> & edgel_chain, sdet_edgel* /*ref_e*/, double /*dpos*/)
{
  //compute LG-ratio and store as the quality

  compute_best_fit(edgel_chain);
  return true;
}

//: report accuracy of measurement
void sdet_CC_curve_model_new::
report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
  double theta_min=1000.0, theta_max=-1000.0, k_min=1000.0, k_max=-1000.0;

  int best_k_ind = -1;
  double least_k=10.0;
  for (int i=0; i<NkClasses; i++){
    if (cv_bundles[i].num_sheets() != 0){
      if (sdet_k_classes[i]<k_min) k_min = sdet_k_classes[i];
      if (sdet_k_classes[i]>k_max) k_max = sdet_k_classes[i];

      if (std::fabs(sdet_k_classes[i])<least_k) best_k_ind=i;
    }
  }

  if (best_k_ind>-1){
    for (auto & i : cv_bundles[best_k_ind][0]){
      if (i.y()<theta_min) theta_min = i.y();
      if (i.y()>theta_max) theta_max = i.y();
    }
  }

  //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = k;      //curvature
  estimates[2] = 0.0;    //curvature derivative

  min_estimates[0] = sdet_angle0To2Pi(theta_min+ref_theta);
  min_estimates[1] = k_min;
  min_estimates[2] = 0.0;

  max_estimates[0] = sdet_angle0To2Pi(theta_max+ref_theta);
  max_estimates[1] = k_max;
  max_estimates[2] = 0.0;

}

//: print info
void sdet_CC_curve_model_new::print_info()
{
  std::cout << " : k= " << k ;
}

//: print central info to file
void sdet_CC_curve_model_new::print(std::ostream& os)
{
  os << "[";
  for (auto & cv_bundle : cv_bundles){
    os << "<";
    if (cv_bundle.num_sheets()==1){
      for (unsigned p=0; p<cv_bundle[0].size(); p++)
        os << "(" << cv_bundle[0][p].x() << " " << cv_bundle[0][p].y() << ")";
    }
    os << ">";
  }
  os << "]";
}

//: read central info from file
void sdet_CC_curve_model_new::read(std::istream& is)
{
  double x, y;
  unsigned char dummy;

  is >> dummy; // "[";

  //read the curve bundles
  for (auto & cv_bundle : cv_bundles)
  {
    vgl_polygon<double> cb;

    //read CB block
    is >> dummy; // "<";
    is >> dummy; //either '>' or '('

    if (dummy == '(') //instantiate a sheet for the polygon
      cb.new_sheet();

    while (dummy != '>'){
      is >> x >> y >> dummy; //x y)
      cb.push_back(x,y);

      is >> dummy; //either '>' or '('
    }
    cv_bundle = cb;
  }

  //make sure that we have reached the end of the block successfully
  is >> dummy; // "]";
  if (dummy != ']') assert(false);
}

//*****************************************************************************//
// Circular Arc curve model with perturbations
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_CC_curve_model_perturbed::sdet_CC_curve_model_perturbed(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                                               double dpos, double dtheta,
                                                               double token_len, double max_k, double /*max_gamma*/,
                                                               bool /*adaptive*/):
  sdet_CC_curve_model(ref_e->pt, ref_e->tangent)
{
  type = CC;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k);

}

//: constructor 3: From the intersection of two curve bundles
sdet_CC_curve_model_perturbed::sdet_CC_curve_model_perturbed(sdet_curve_model* cm1, sdet_curve_model* cm2)
{
  type = CC;

  pt = ((sdet_CC_curve_model_perturbed*)cm1)->pt;
  tangent = ((sdet_CC_curve_model_perturbed*)cm1)->tangent;

  for (int i=0; i<NperturbPCC; i++)
  {
    pts[i] = ((sdet_CC_curve_model_perturbed*)cm1)->pts[i];

    cv_bundles[i] = vgl_clip(((sdet_CC_curve_model_perturbed*)cm1)->cv_bundles[i], ((sdet_CC_curve_model_perturbed*)cm2)->cv_bundles[i], vgl_clip_type_intersect);
    if (cv_bundles[i].num_sheets()>1)
      cv_bundles[i] = vgl_polygon<double>(); //remove these erroneous curve bundles
  }
}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_CC_curve_model_perturbed* sdet_CC_curve_model_perturbed::intersect(sdet_CC_curve_model_perturbed* cm)
{
  return new sdet_CC_curve_model_perturbed(this, cm);
}

//: is this bundle valid?
bool sdet_CC_curve_model_perturbed::bundle_is_valid()
{
  bool valid = false;

  for (auto & cv_bundle : cv_bundles)
    valid = valid || (cv_bundle.num_sheets()==1);

  return valid;
}

//: find the perturbed position and tangent given the index
void sdet_CC_curve_model_perturbed::compute_perturbed_position_of_an_edgel(sdet_edgel* e,
                                                                            int per_ind,
                                                                            double dpos, double /*dtheta*/,
                                                                            vgl_point_2d<double>& pt)
{
  double dp=0;

  if(NperturbPCC>1)
    dp = dpos*(2*per_ind-NperturbPCC+1)/(NperturbPCC-1);

  //compute perturbed point and tangent corresponding to this position
  pt = e->pt + vgl_vector_2d<double>(dp*std::cos(e->tangent+vnl_math::pi/2),
                                     dp*std::sin(e->tangent+vnl_math::pi/2));
}

//: compute the CC curve bundle for an edgel pair at the ref edgel
void sdet_CC_curve_model_perturbed::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double token_len, double max_k)
{
  //for each discrete perturbation compute a curve bundle
  for (int i=0; i<NperturbPCC; i++)
  {
    //compute perturbe position
    compute_perturbed_position_of_an_edgel(ref_e, i, dpos, dtheta, pts[i]);

    //determine the intrinsic parameters for this edgel pair
    sdet_int_params params;
    if (ref_e == e1){
      params = sdet_get_intrinsic_params(pts[i], e2->pt, e1->tangent, e2->tangent);

      //do the energy test
      bool test_passed = edgel_pair_legal(params, e1->tangent, e2->tangent);
      //bool test_passed = edgel_pair_legal2(params, true);

      if (test_passed)
        sdet_CC_curve_model::compute_curve_bundle(cv_bundles[i], params, true, dpos, dtheta, token_len, max_k);
    }
    else {
      params = sdet_get_intrinsic_params(e1->pt, pts[i], e1->tangent, e2->tangent);

      //do the energy test
      bool test_passed = edgel_pair_legal(params, e1->tangent, e2->tangent);
      //bool test_passed = edgel_pair_legal2(params, false);

      if (test_passed)
        sdet_CC_curve_model::compute_curve_bundle(cv_bundles[i], params, false, dpos, dtheta, token_len, max_k);
    }
  }
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_CC_curve_model_perturbed::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  //determine the best perturbation of the edgel by looking at the
  //curve bundles produced by each perturbation
  for (int i=0; i<NperturbPCC; i++){
    //assign the rough centroid of this bundle as the best estimate
    if (cv_bundles[i].num_sheets() != 0){

      //compute centroid
      dts[i] = 0.0;
      ks[i] = 0.0;
      for (unsigned j=0; j<cv_bundles[i][0].size(); j++){
        dts[i] += cv_bundles[i][0][j].x();
        ks[i] += cv_bundles[i][0][j].y();
      }
      dts[i] /= cv_bundles[i].num_vertices();
      ks[i] /= cv_bundles[i].num_vertices();

      //is the centroid within the curve bundle?
      //assert(cv_bundles[i].contains(vgl_point_2d<double>(thetas[i], ks[i])));
    }
    else { //invalid curve bundles
      dts[i] = 1000.0;
      ks[i] = 1000.0;
    }
  }

  //determine the best perturbation by looking at the cv bundles
  int ind = 0;
  //**************************************************
  // CRITERIA 1: The largest surviving curvelet wins
  //
  // There is no direct information about the size of the curvelets in this class
  // But since this object is formed for each curvelet, the largest curvelet probably only has
  // one of the perturbations producing a bundle so assign the surviving one as the best
  //**************************************************
  for (int i=0; i<NperturbPCC; i++){
    if (cv_bundles[i].num_sheets() == 1)
      ind = i; //best bundle
  }

  //Now assign the best curve bundle as the main curve bundle
  cv_bundle = cv_bundles[ind];

  pt = pts[ind];
  theta = dts[ind] + tangent;
  k = ks[ind];
  return {dts[ind], ks[ind]};
}

//: print info
void sdet_CC_curve_model_perturbed::print_info()
{
  std::cout << " : (th=" << theta << ", k=" << k << ", gamma=0.0" << std::endl;

  for (int i=0; i<NperturbPCC; i++){
    std::cout << "                             ";
    std::cout << "pos: " << i << " : (th=" << dts[i]+tangent << ", k=" << ks[i] << ", gamma=0.0" << std::endl;
  }
}

//*****************************************************************************//
// Circular Arc curve model (3d bundle)
//*****************************************************************************//

//: copy constructor
sdet_CC_curve_model_3d::sdet_CC_curve_model_3d(const sdet_CC_curve_model_3d& other) :
  sdet_curve_model(other)
{
  type = CC3d;

  Kmin = other.Kmin;
  Kmax = other.Kmax;

  //Dx = other.Dx;
  //Dt = other.Dt;

  ref_pt = other.ref_pt;
  ref_theta = other.ref_theta;

  pt = other.pt;
  theta = other.theta;
  k = other.k;
}


//: constructor 4: By transporting a curve model to another extrinsic anchor point
sdet_CC_curve_model_3d::
sdet_CC_curve_model_3d(sdet_CC_curve_model_3d* cm, vgl_point_2d<double> pt, double theta):
  pt(0.0,0.0), theta(0), k(0.0)
{
  type = CC3d;

  ref_pt = pt;
  ref_theta = theta;

  //Dx = cm->Dx;
  //Dt = cm->Dt;

  //identical reference point so just make a copy of the bundle
  if (pt==cm->ref_pt && theta==cm->ref_theta){
    Kmin = cm->Kmin;
    Kmax = cm->Kmax;
  }

  //Unfortunately, we don't know how to transport these bundles in the generic case

}

//: constructor 5: Create default bundle only
sdet_CC_curve_model_3d::
sdet_CC_curve_model_3d(sdet_edgel* ref_e, double dpos, double dtheta, double /*token_len*/, double max_k, double /*max_gamma*/, bool adaptive):
  Kmin(1, 1, -max_k), Kmax(1, 1, max_k), ref_pt(ref_e->pt), ref_theta(ref_e->tangent), pt(0.0,0.0), theta(0), k(0.0)
{
  type = CC3d;

  ref_pt = ref_e->pt;
  ref_theta = ref_e->tangent;

  //determine the size of the uncertainty at the reference edgel
  double ref_dx = dpos, ref_dt=dtheta;
  if (adaptive){
    ref_dx += ref_e->uncertainty;
    ref_dt += ref_e->uncertainty*vnl_math::pi_over_4/2;
  }

  //compute the curvature bounds due to the token length and dpos
  //This is to be used for determining the min and max of the curvature
  //values for the default bundle
  // double dk = 2*ref_dx/(token_len*token_len/4.0 + ref_dx*ref_dx);

  //1) construct the bundle at the ref edgel and the other edgel

  // create the default bundle (omega: the restricted curve bundle space)
  // (currently assuming independence of the parameters)
  //
  // Also determine the size of the grid necessary to represent the curve bundle
  // given its uncertainty parameters

  Kmin.resize(int(2*std::floor(ref_dx/_dx_+0.5)+1), int(2*std::floor(ref_dt/_dt_+0.5)+1));
  Kmin.fill(-max_k);

  Kmax.resize(int(2*std::floor(ref_dx/_dx_+0.5)+1), int(2*std::floor(ref_dt/_dt_+0.5)+1));
  Kmax.fill(max_k);
}


//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_CC_curve_model_3d* sdet_CC_curve_model_3d::intersect(sdet_CC_curve_model_3d* cm)
{
  assert(cm->type==CC3d);

  return new sdet_CC_curve_model_3d(this, cm);
}

//: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
sdet_curve_model* sdet_CC_curve_model_3d::transport(vgl_point_2d<double> pt, double theta)
{
  return new sdet_CC_curve_model_3d(this, pt, theta);
}

//: get the size of the bundle for computing the saliency heuristic
double sdet_CC_curve_model_3d::get_CC_3d_bundle_area()
{
  double vol = 0.0;

  for (unsigned i=0; i<Kmax.rows(); i++)
    for (unsigned j=0; j<Kmax.cols(); j++)
      if (Kmax(i,j)>Kmin(i,j))
        vol += 1.0;

  return vol;
}

//: determine if edgel pair is legal
bool sdet_CC_curve_model_3d::edgel_pair_legal(sdet_int_params &params, double tan1, double tan2)
{
  //simple orientation based pruning
  bool ret = (sdet_dot(params.ref_dir, tan1)>0 && sdet_dot(params.ref_dir, tan2)>0);
  return ret;
}


#ifdef DBDET_USE_APPROX_SINE

//always wrap input angle to -PI..PI
inline double mysin_lowprecision(double x) {
  if (x < -3.14159265)
      x += 6.28318531;
  else
  if (x >  3.14159265)
      x -= 6.28318531;

  //compute sine
  if (x < 0)
      return 1.27323954 * x + .405284735 * x * x;
  else
      return 1.27323954 * x - 0.405284735 * x * x;
}

inline double mycos_lowprecision(double x) {
  if (x < -3.14159265)
      x += 6.28318531;
  else
  if (x >  3.14159265)
      x -= 6.28318531;
  //compute cosine: sin(x + PI/2) = cos(x)
  x += 1.57079632;
  if (x >  3.14159265)
      x -= 6.28318531;

  if (x < 0)
      return 1.27323954 * x + 0.405284735 * x * x;
  else
      return 1.27323954 * x - 0.405284735 * x * x;
}


inline double mysin_precise(double x) {
  //always wrap input angle to -PI..PI
  if (x < -3.14159265)
      x += 6.28318531;
  else
  if (x >  3.14159265)
      x -= 6.28318531;

  //compute sine
  if (x < 0)
  {
      double const sin = 1.27323954 * x + .405284735 * x * x;

      if (sin < 0)
          return .225 * (sin *-sin - sin) + sin;
      else
          return .225 * (sin * sin - sin) + sin;
  }
  else
  {
      double const sin = 1.27323954 * x - 0.405284735 * x * x;

      if (sin < 0)
          return .225 * (sin *-sin - sin) + sin;
      else
          return .225 * (sin * sin - sin) + sin;
  }
}

inline double mycos_precise(double x) {
  if (x < -3.14159265)
      x += 6.28318531;
  else
  if (x >  3.14159265)
      x -= 6.28318531;
  //compute cosine: sin(x + PI/2) = cos(x)
  x += 1.57079632;
  if (x >  3.14159265)
      x -= 6.28318531;

  if (x < 0)
  {
      double const cos = 1.27323954 * x + 0.405284735 * x * x;

      if (cos < 0)
          return .225 * (cos *-cos - cos) + cos;
      else
          return .225 * (cos * cos - cos) + cos;
  }
  else
  {
      double const cos = 1.27323954 * x - 0.405284735 * x * x;

      if (cos < 0)
          return .225 * (cos *-cos - cos) + cos;
      else
          return .225 * (cos * cos - cos) + cos;
  }
}

// You can also try mysin_lowprecision for really fast speedups, but this will
// generate largely different results.
#define mysin(x) (mysin_precise((x)))
#define mycos(x) (mycos_precise((x)))

#else

#define mysin(x) (std::sin((x)))
#define mycos(x) (std::cos((x)))

#endif

//: compute the CC curve bundle for an edgel pair at the ref edgel
//
// \Remarks This is an optimzied version of the implementation.  See the commented block
// after this function for the original implementation, if you think this
// optimized one has become unreadable.
void sdet_CC_curve_model_3d::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double /*token_len*/, double max_k,
                     bool adaptive)
{
  //determine the size of the uncertainty at the reference edgel
  double ref_dx = dpos, ref_dt=dtheta;
  if (adaptive){
    ref_dx += ref_e->uncertainty;
    ref_dt += ref_e->uncertainty*vnl_math::pi_over_4/2;
  }

  //compute the curvature bounds due to the token length and dpos
  //This is to be used for determining the min and max of the curvature
  //values for the default bundle
  //double dk = 2*ref_dx/(token_len*token_len/4.0 + ref_dx*ref_dx);

  //1) construct the bundle at the ref edgel and the other edgel

  // create the default bundle (omega: the restricted curve bundle space)
  // (currently assuming independence of the parameters)
  //
  // Also determine the size of the grid necessary to represent the curve bundle
  // given its uncertainty parameters

  Kmin.resize(2*std::floor(ref_dx/_dx_+0.5)+1, 2*std::floor(ref_dt/_dt_+0.5)+1);
  Kmin.fill((float)-max_k);

  Kmax.resize(2*std::floor(ref_dx/_dx_+0.5)+1, 2*std::floor(ref_dt/_dt_+0.5)+1);
  Kmax.fill((float)max_k);

  sdet_edgel* ne=nullptr; //relic of older design req.
  ne = (ref_e == e1) ? e2 : e1;

  //now compute the four constraint surfaces due to the neighboring edgel

  const double DX = ref_e->pt.x() - ne->pt.x();
  const double DY = ref_e->pt.y() - ne->pt.y();
  const double DX2 = DX*DX;
  const double DY2 = DY*DY;

  //if neighboring point is close to the ref edgel, no static constraint can be computed of leave it as the default bundle
  if ( DX2 + DY2 < 4*dpos*dpos )
    return;

  const double T0 = ne->tangent;
  const double T2 = ref_e->tangent;

  //The uncertainty of the neighboring edgel (default uncertainty = adaptive)
  const double dx = (adaptive ? dpos+ne->uncertainty : dpos);
  const double dx_sq = dx*dx;
  const double dt = (adaptive ? dtheta+ne->uncertainty*(vnl_math::pi_over_4/2.0) : dtheta);
  const double T0_p_dt = T0 + dt;
  const double T0_m_dt = T0 - dt;
  //double dt = dtheta;

  if ( (DX*mycos(T2)+DY*mysin(T2)) < 0 ) {
    for (unsigned i=0; i<Kmax.rows(); ++i){
      for (unsigned j=0; j<Kmax.cols(); ++j){
        const double dx2 = Dx(i);
        const double dt2 = Dt(j);

        const double T2t = T2+dt2;
        const double tt = mysin(T2t)*DX - mycos(T2t)*DY;

        const double denom = (-2.0*dx2*tt + DX2 + DY2 - dx_sq + dx2*dx2)*0.5;

        const double ttmdx2 = tt - dx2;
        const float k_dx_min = (ttmdx2-dx)/denom;
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmin(i,j) < k_dx_min)
          Kmin(i,j) = k_dx_min;

        const float k_dx_max = (ttmdx2+dx)/denom;
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmax(i,j) > k_dx_max)
          Kmax(i,j) = k_dx_max;

        const double sin_t2_p_dt2_m_t0_m_dt = mysin(T2t-T0_m_dt);
        const double sin_t2_p_dt2_m_t0_p_dt = mysin(T2t-T0_p_dt);
        const double sin_T0pdt_times_DY = DY*mysin(T0_p_dt);
        const double cos_T0pdt_times_DX = DX*mycos(T0_p_dt);
        const double sin_T0mdt_times_DY = DY*mysin(T0_m_dt);
        const double cos_T0mdt_times_DX = DX*mycos(T0_m_dt);

        const auto k_dt_min = static_cast<float>(sin_t2_p_dt2_m_t0_m_dt/(-sin_t2_p_dt2_m_t0_m_dt*dx2 + cos_T0mdt_times_DX + sin_T0mdt_times_DY));
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmin(i,j) < k_dt_min)
          Kmin(i,j) = k_dt_min;

        const auto k_dt_max = static_cast<float>(sin_t2_p_dt2_m_t0_p_dt/(-sin_t2_p_dt2_m_t0_p_dt*dx2 + cos_T0pdt_times_DX + sin_T0pdt_times_DY));
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmax(i,j) > k_dt_max)
          Kmax(i,j) = k_dt_max;
      }
    }
  } else {
    for (unsigned i=0; i<Kmax.rows(); ++i){
      for (unsigned j=0; j<Kmax.cols(); ++j){
        const double dx2 = Dx(i);
        const double dt2 = Dt(j);

        const double T2t = T2+dt2;
        const double tt = mysin(T2t)*DX - mycos(T2t)*DY;

        const double denom = -(2.0*dx2*tt - DX2 - DY2 + dx_sq - dx2*dx2)/2.0;

        const double ttmdx2 = tt - dx2;
        const float k_dx_min = (ttmdx2-dx)/denom;
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmin(i,j) < k_dx_min)
          Kmin(i,j) = k_dx_min;

        const float k_dx_max = (ttmdx2+dx)/denom;
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmax(i,j) > k_dx_max)
          Kmax(i,j) = k_dx_max;

        const double sin_t2_p_dt2_m_t0_m_dt = mysin(T2t-T0_m_dt);
        const double sin_t2_p_dt2_m_t0_p_dt = mysin(T2t-T0_p_dt);
        const double sin_T0pdt_times_DY = DY*mysin(T0_p_dt);
        const double cos_T0pdt_times_DX = DX*mycos(T0_p_dt);
        const double sin_T0mdt_times_DY = DY*mysin(T0_m_dt);
        const double cos_T0mdt_times_DX = DX*mycos(T0_m_dt);

        const auto k_dt_max = static_cast<float>(sin_t2_p_dt2_m_t0_m_dt/(-sin_t2_p_dt2_m_t0_m_dt*dx2 + cos_T0mdt_times_DX + sin_T0mdt_times_DY));
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmax(i,j) > k_dt_max)
          Kmax(i,j) = k_dt_max;

        const auto k_dt_min = static_cast<float>(sin_t2_p_dt2_m_t0_p_dt/(-sin_t2_p_dt2_m_t0_p_dt*dx2 + cos_T0pdt_times_DX + sin_T0pdt_times_DY));
        //compute the intersection of all the surfaces to compute the final bundle
        if (Kmin(i,j) < k_dt_min)
          Kmin(i,j) = k_dt_min;
      }
    }
  }
}

#undef mysin
#undef mycos

/*
 *
 * ORIGINAL IMPLEMENTATION OF THE ABOVE OPTIMIZED FUNCTION. THIS IS INCLUDED
 * HERE FOR REFERENCE AND CLARITY.
 *
//: compute the CC curve bundle for an edgel pair at the ref edgel
void sdet_CC_curve_model_3d::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta, double token_len, double max_k,
                     bool adaptive)
{
  //determine the size of the uncertainty at the reference edgel
  double ref_dx = dpos, ref_dt=dtheta;
  if (adaptive){
    ref_dx += ref_e->uncertainty;
    ref_dt += ref_e->uncertainty*vnl_math::pi_over_4/2;
  }

  //compute the curvature bounds due to the token length and dpos
  //This is to be used for determining the min and max of the curvature
  //values for the default bundle
  double dk = 2*ref_dx/(token_len*token_len/4.0 + ref_dx*ref_dx);

  //1) construct the bundle at the ref edgel and the other edgel

  // create the default bundle (omega: the restricted curve bundle space)
  // (currently assuming independence of the parameters)
  //
  // Also determine the size of the grid necessary to represent the curve bundle
  // given its uncertainty parameters

  Kmin.resize(int(2*std::floor(ref_dx/_dx_+0.5)+1), int(2*std::floor(ref_dt/_dt_+0.5)+1));
  Kmin.fill(float(-max_k));

  Kmax.resize(int(2*std::floor(ref_dx/_dx_+0.5)+1), int(2*std::floor(ref_dt/_dt_+0.5)+1));
  Kmax.fill(float(max_k));

  sdet_edgel* ne=0; //relic of older design req.
  if (ref_e == e1)
    ne = e2;
  else
    ne = e1;

  //now compute the four constraint surfaces due to the neighboring edgel
  vbl_array_2d<float> k_dx_min(Kmin.rows(), Kmin.cols());
  vbl_array_2d<float> k_dx_max(Kmin.rows(), Kmin.cols());
  vbl_array_2d<float> k_dt_min(Kmin.rows(), Kmin.cols());
  vbl_array_2d<float> k_dt_max(Kmin.rows(), Kmin.cols());

  double DX = ref_e->pt.x() - ne->pt.x();
  double DY = ref_e->pt.y() - ne->pt.y();

  //if neighboring point is close to the ref edgel, no constraint can be computed of leave it as the default bundle
  if (std::sqrt(DX*DX+DY*DY)<2*dpos)
    return;

  double T0 = ne->tangent;
  double T2 = ref_e->tangent;

  //The uncertainty of the neighboring edgel (default uncertainty = adaptive)
  double dx = (adaptive ? dpos+ne->uncertainty : dpos);
  double dt = (adaptive ? dtheta+ne->uncertainty*vnl_math::pi_over_4/2 : dtheta);
  //double dt = dtheta;

  for (int i=0; i<Kmax.rows(); ++i){
    for (int j=0; j<Kmax.cols(); ++j){

      double dx2 = Dx(i);
      double dt2 = Dt(j);

      double T2t = T2+dt2;
      double tt = sin(T2t)*DX-cos(T2t)*DY;

      k_dx_min(i,j) = float(-2*(tt-dx-dx2)/(2*dx2*tt - DX*DX - DY*DY + dx*dx - dx2*dx2));
      k_dx_max(i,j) = float(-2*(tt+dx-dx2)/(2*dx2*tt - DX*DX - DY*DY + dx*dx - dx2*dx2));

      if ((DX*std::cos(T2)+DY*std::sin(T2))<0){
          k_dt_min(i,j) = float(std::sin((T2+dt2)-(T0-dt))/(std::sin((T0-dt)-(T2+dt2))*dx2 + DX*std::cos(T0-dt) + DY*std::sin(T0-dt)));
          k_dt_max(i,j) = float(std::sin((T2+dt2)-(T0+dt))/(std::sin((T0+dt)-(T2+dt2))*dx2 + DX*std::cos(T0+dt) + DY*std::sin(T0+dt)));
      }
      else {
          k_dt_max(i,j) = float(std::sin((T2+dt2)-(T0-dt))/(std::sin((T0-dt)-(T2+dt2))*dx2 + DX*std::cos(T0-dt) + DY*std::sin(T0-dt)));
          k_dt_min(i,j) = float(std::sin((T2+dt2)-(T0+dt))/(std::sin((T0+dt)-(T2+dt2))*dx2 + DX*std::cos(T0+dt) + DY*std::sin(T0+dt)));
      }
    }
  }

  //Now compute the intersection of all the surfaces to compute the final bundle
  vbl_array_2d_max_replace(Kmin, k_dx_min);
  vbl_array_2d_max_replace(Kmin, k_dt_min);

  vbl_array_2d_min_replace(Kmax, k_dx_max);
  vbl_array_2d_min_replace(Kmax, k_dt_max);

}
*/



//: Transport given curve from the given extrinsic point-tangent to the current point-tangent
//: Transport a given curve from one anchor to another
vgl_point_2d<double> sdet_CC_curve_model_3d::transport_CC(vgl_point_2d<double>dx_dt, double k,
                                                            vgl_point_2d<double> spt, double stheta,
                                                            vgl_point_2d<double> dpt, double dtheta,
                                                            bool & sgn_change)
{
  double DX = dpt.x()-spt.x();
  double DY = dpt.y()-spt.y();
  double dx1 = dx_dt.x();
  double dt1 = dx_dt.y();

  double tT1 = stheta + dt1;
  double T2 = dtheta;

  sgn_change = false;

  //compute transported model (dx2, dt2)
  double dx2, dt2;

  if (std::fabs(k)<1e-5) //constant curvature case:
  {
    dx2 = dx1 + (DX*-std::sin(tT1)+DY*std::cos(tT1));
    dt2 = tT1-T2;
  }
  else { //regular arc

    double a = DX - (dx1-1/k)*std::sin(tT1);
    double b = DY + (dx1-1/k)*std::cos(tT1);

    //double A = std::sqrt(a*a + b*b);
    //double tT2 = -std::atan(a/b);
    double tT2 = std::atan2(a, -b);

    //// alternate solutions based on simple algebra
    //double dx2a = 1/k - A;
    //double dx2b = i/k + A;

    // alternate solutions (based on tT2)
    double dx2a, dx2b;
    if (std::fabs(std::sin(tT2))>1e-5){
      dx2a = +1/k - a/std::sin(tT2);
      dx2b = -1/k - a/std::sin(tT2); //assume tT2 is backwards
    }
    else {
      dx2a = +1/k - b/std::cos(tT2);
      dx2b = -1/k - b/std::cos(tT2); //assume tT2 is backwards
    }

    if (std::fabs(dx2a) < std::fabs(dx2b)){
      dx2 = dx2a;
    }
    else {
      dx2 = dx2b;
      sgn_change = true;
    }
    dt2 = tT2 - T2;
  }

  //correct dt2 to keep it within the -pi/2, pi/2 limits
  if (dt2 < -vnl_math::pi) dt2 += 2*vnl_math::pi;
  if (dt2 >  vnl_math::pi) dt2 -= 2*vnl_math::pi;

  // keep dt2 between -pi/2 and pi/2 (reverse curvature signs if reversed)
  if (dt2 < -vnl_math::pi_over_2){
    dt2 += vnl_math::pi;
    dx2 = -dx2;
    sgn_change = !sgn_change;
  }

  if (dt2 > vnl_math::pi_over_2){
    dt2 -= vnl_math::pi;
    dx2 = -dx2;
    sgn_change = !sgn_change;
  }

  return {dx2, dt2};
}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double> sdet_CC_curve_model_3d::compute_best_fit()
{
  if (this->bundle_is_valid())
  {
    //find the point of minimum deviation of the edgel
    unsigned mini = Kmax.rows(); unsigned minj = Kmax.cols();
    double mind = 100;
    for (unsigned i=0; i<Kmax.rows(); i++){
      for (unsigned j=0; j<Kmax.cols(); j++){

        if (Kmax(i,j)>Kmin(i,j)){
          double d = Dx(i)*Dx(i)+Dt(j)*Dt(j);

          if (d<mind){
            mind = d;
            mini=i;
            minj=j;
          }
        }
      }
    }

    assert(mini<Kmax.rows() && minj<Kmax.cols());

    //use this point to set the best fit of the curve bundle
    vgl_point_2d<double> best_fit(Dx(mini), Dt(minj));

    //this->set_best_fit(best_fit, (Kmax(mini, minj)+Kmin(mini, minj))/2);
    this->set_best_fit(best_fit, Kmin(mini, minj));

    return best_fit;
  }

  return {0,0}; //no optimal, just the ref
}


//: Compute the best fit curve from the curve bundle
vgl_point_2d<double> sdet_CC_curve_model_3d::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  return compute_best_fit(); //no use for the edgel chain in this function
}

//: Set the best fit curve
void sdet_CC_curve_model_3d::set_best_fit(vgl_point_2d<double> dx_dt, double kk)
{
  double dx = dx_dt.x();
  double dt = dx_dt.y();

  //compute the extrinsic point and tangent of the centroid
  theta = sdet_angle0To2Pi(ref_theta + dt);
  pt = ref_pt + vgl_vector_2d<double>(dx*-std::sin(theta), dx* std::cos(theta));
  k= kk;
}

//: function to check if the curve fit is reasonable
bool sdet_CC_curve_model_3d::
curve_fit_is_reasonable(std::deque<sdet_edgel*> & edgel_chain, sdet_edgel* /*ref_e*/, double /*dpos*/)
{
  //compute LG-ratio and store as the quality

  compute_best_fit(edgel_chain);
  return true;
}

// utility functions

//: are these two curve bundles C^2?
bool sdet_CC_curve_model_3d::is_C2_with(sdet_curve_model* cm)
{
  auto* cm2 = (sdet_CC_curve_model_3d*)cm;

  bool valid = false;
  for (unsigned i=0; i<Kmax.rows(); i++)
    for (unsigned j=0; j<Kmax.cols(); j++)
        valid = valid || (std::min(Kmax(i,j), cm2->Kmax(i,j)) > std::max(Kmin(i,j), cm2->Kmin(i,j)));

  return valid;
}

//: are these two curve bundles C^1?
bool sdet_CC_curve_model_3d::is_C1_with(sdet_curve_model* cm)
{
  auto* cm2 = (sdet_CC_curve_model_3d*)cm;

  bool valid = false;
  for (unsigned i=0; i<Kmax.rows(); i++)
    for (unsigned j=0; j<Kmax.cols(); j++)
        valid = valid || ((Kmax(i,j)>Kmin(i,j)) && (cm2->Kmax(i,j)>cm2->Kmin(i,j)));

  return valid;
}

//: report accuracy of measurement
void sdet_CC_curve_model_3d::
report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
  double theta_min=1000.0, theta_max=-1000.0, k_min=1000.0, k_max=-1000.0;

  //FIX ME

  //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = k;      //curvature
  estimates[2] = 0.0;    //curvature derivative

  min_estimates[0] = sdet_angle0To2Pi(theta_min+ref_theta);
  min_estimates[1] = k_min;
  min_estimates[2] = 0.0;

  max_estimates[0] = sdet_angle0To2Pi(theta_max+ref_theta);
  max_estimates[1] = k_max;
  max_estimates[2] = 0.0;

}

//: print info
void sdet_CC_curve_model_3d::print_info()
{
  std::cout << " : k= " << k ;
}

//: print central info to file
void sdet_CC_curve_model_3d::print(std::ostream&  os)
{
  //FIX ME
  os << "[";
  os << ref_pt.x()<<" ";
 // os << "] [";
  os << ref_pt.y()<<" ";
  //os << "] [";
  os << ref_theta<<" ";
  //os << "] [";
  os << pt.x()<<" ";
  //os << "] [";
  os << pt.y()<<" ";
  //os << "] [";
  os << theta<<" ";
  //os << "] [";
  os << k;

  os << "]";
}

//: read central info from file
void sdet_CC_curve_model_3d::read(std::istream& is)
{
  //double x, y;
  unsigned char dummy;

  //FIX ME
  is >> dummy; // "[";
  is >> ref_pt.x();
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> ref_pt.y();
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> ref_theta;
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> pt.x();
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> pt.y();
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> theta;
  //is >> dummy; // "]"
  //is >> dummy; // "[";
  is >> k;
  is >> dummy; // "]"
}


//*****************************************************************************//
// Euler Spiral curve model
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_ES_curve_model::sdet_ES_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                           double dpos, double dtheta,
                                           double token_len, double max_k, double max_gamma,
                                           bool /*adaptive*/):
  pt(ref_e->pt), theta(ref_e->tangent), k(0.0), gamma(0.0)
{
  type = ES;

  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k, max_gamma);
}

//: constructor 3: From the intersection of two curve bundles
sdet_ES_curve_model::sdet_ES_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2) :
  pt(((sdet_ES_curve_model*)cm1)->pt), theta(((sdet_ES_curve_model*)cm1)->theta), k(0.0), gamma(0.0)
{
  type = ES;

  //compute the new curve bundle by intersection
  cv_bundle = vgl_clip(((sdet_ES_curve_model*)cm1)->cv_bundle, ((sdet_ES_curve_model*)cm2)->cv_bundle, vgl_clip_type_intersect);
}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_ES_curve_model* sdet_ES_curve_model::intersect(sdet_ES_curve_model* cm)
{
  return new sdet_ES_curve_model(this, cm);
}

//: determine if edgel pair is legal
bool sdet_ES_curve_model::edgel_pair_legal(sdet_int_params &params, double /*tan1*/, double /*tan2*/)
{
  //allow everything for now (assuming basic check has been done prior to forming the curve model)
  return true;

  //look up the Euler spiral parameters for this pair
  double k, gamma, len;
  double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)
  // read the ES solutions from the table and scale appropriately
  bvgl_eulerspiral_lookup_table::instance()->look_up( params.t1, params.t2,
                                                      &k, &gamma, &len,
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  k = k/params.d; gamma= gamma/(params.d*params.d);

  //some energy function
  double E = gamma*gamma*len;

  //threshold using a simple energy function
  bool ret = (E<1); //arbitrary threshold

  return ret;
}

//: compute the ES curve bundle for an edgel pair at the ref edgel
void sdet_ES_curve_model::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta,
                     double token_len, double max_k, double max_gamma)
{
  //determine the intrinsic parameters for this edgel pair
  sdet_int_params params = sdet_get_intrinsic_params(e1->pt, e2->pt, e1->tangent, e2->tangent);

  //do the energy test
  bool test_passed = edgel_pair_legal(params, e1->tangent, e2->tangent);
  //bool test_passed = edgel_pair_legal2(params, true);

  if (test_passed)
    compute_curve_bundle(cv_bundle, params, ref_e==e1, dpos, dtheta, token_len, max_k, max_gamma);

}

//: compute the ES curve bundle for an edgel pair given intrinsic params
void sdet_ES_curve_model::
compute_curve_bundle(vgl_polygon<double>& bundle,
                     sdet_int_params &params, bool first_is_ref,
                     double dpos, double dtheta,
                     double /*token_len*/, double /*max_k*/, double /*max_gamma*/)
{
  // if the geometry is not valid (too close) for this computation, just assign omega
  // as the curve bundle
  if (params.d < dpos){
    double omega_coords[] = { -1,0,  0,0.2, 1,0,  0,-0.2 };
    vgl_polygon<double> omega(omega_coords, 4);

    bundle = omega; //set omega as the curve bundle
    return;
  }

  // predict the variation in the intrinsic parameters
  double alpha = std::asin(dpos/params.d);

  double t1_1, t1_2, t1_3, t1_4, t2_1, t2_2, t2_3, t2_4;
  if (first_is_ref)
  {
    double dt1p = alpha; //predicted perturbation in theta1
    double dt2p = alpha + dtheta; //predicted perturbation in theta2

    //perturbation of the intrinsic parameters due to measurement uncertainty
    t1_1 = params.t1 + dt1p;  t2_1 = params.t2;
    t1_2 = params.t1 + dt1p;  t2_2 = params.t2 + dt2p;
    t1_3 = params.t1 - dt1p;  t2_3 = params.t2;
    t1_4 = params.t1 - dt1p;  t2_4 = params.t2 - dt2p;
  }
  else
  {
    double dt1p = alpha + dtheta; //predicted perturbation in theta1
    double dt2p = alpha; //predicted perturbation in theta2

    //perturbation of the intrinsic parameters due to measurement uncertainty
    t1_1 = params.t1;         t2_1 = params.t2 - dt2p;
    t1_2 = params.t1 + dt1p;  t2_2 = params.t2 + dt2p;
    t1_3 = params.t1;         t2_3 = params.t2 + dt2p;
    t1_4 = params.t1 - dt1p;  t2_4 = params.t2 - dt2p;
  }

  // Construct the curve bundle in k-gamma space from this region.
  double k1, gamma1, len1, k2, gamma2, len2, k3, gamma3, len3, k4, gamma4, len4;
  double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)

  // read the ES solutions from the table and scale appropriately
  bvgl_eulerspiral_lookup_table::instance()->look_up( t1_1, t2_1, &k1, &gamma1, &len1,
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  bvgl_eulerspiral_lookup_table::instance()->look_up( t1_2, t2_2, &k2, &gamma2, &len2,
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  bvgl_eulerspiral_lookup_table::instance()->look_up( t1_3, t2_3, &k3, &gamma3, &len3,
                                                      &k0_max_error, &gamma_max_error, &len_max_error );
  bvgl_eulerspiral_lookup_table::instance()->look_up( t1_4, t2_4, &k4, &gamma4, &len4,
                                                      &k0_max_error, &gamma_max_error, &len_max_error );

  //if reference is the second edgel we need to compute the curvature at that point
  if (!first_is_ref){
    k1 = k1+gamma1*len1;
    k2 = k2+gamma2*len2;
    k3 = k3+gamma3*len3;
    k4 = k4+gamma4*len4;
  }

  double d2 = params.d*params.d;

  //scaling
  k1 = k1/params.d; gamma1 = gamma1/(d2);
  k2 = k2/params.d; gamma2 = gamma2/(d2);
  k3 = k3/params.d; gamma3 = gamma3/(d2);
  k4 = k4/params.d; gamma4 = gamma4/(d2);

  //form the curve bundle polygon from these coordinates
  bundle.new_sheet();
  bundle.push_back(k1, gamma1);
  bundle.push_back(k2, gamma2);
  bundle.push_back(k3, gamma3);
  bundle.push_back(k4, gamma4);

}

//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_ES_curve_model::compute_best_fit(std::deque<sdet_edgel*> &/*edgel_chain*/)
{
  //assign the rough centroid of this bundle as the best estimate
  k = 0.0;
  gamma = 0.0;
  for (auto & i : cv_bundle[0]){
    k += i.x();
    gamma += i.y();
  }
  k /= cv_bundle.num_vertices();
  gamma /= cv_bundle.num_vertices();

  return {k, gamma};
}


//: function to check if the curve fit is reasonable
bool sdet_ES_curve_model::curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain,
                                                   sdet_edgel* ref_e, double dpos)
{
  //the fit is considered to be reasonable if the length of the interpolating ES
  //is close to the length of the polyline connecting the edgels

  //compute the centroid of this curve bundle
  compute_best_fit(edgel_chain);

  //initialize an ES corresponding to the centroid
  bvgl_eulerspiral es_fit(pt, theta, k, gamma, 1);

  //Compute the polygon lengths
  bool before_ref = true;
  double Lm=0, Lp=0;
  for (unsigned i=0; i<edgel_chain.size()-1; i++){
    if (before_ref) Lm += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt);
    else            Lp += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt);

    if (edgel_chain[i+1]==ref_e)
      before_ref = false;
  }

  //if endpoints of the ES are not close to the edgels
  if (vgl_distance(es_fit.point_at_length(-Lm), edgel_chain[0]->pt) > dpos ||
      vgl_distance(es_fit.point_at_length(Lp),  edgel_chain[edgel_chain.size()-1]->pt) > dpos)
    return false;
  else
    return true;
}



//: compute the distance between edgels and an ES curve
double sdet_ES_curve_model::compute_distance(std::deque<sdet_edgel*> &edgel_chain,
                                              vgl_point_2d<double>pt, double theta,
                                              double k, double gamma)
{
  //form an ES from the parameters
  bvgl_eulerspiral es_fit(pt, theta, k, gamma, 1);

  //compute distance from each of the edgels to the curve
  double dist = 0;
  for (auto & i : edgel_chain){
    double l = vgl_distance(i->pt, pt);

    double ref_dir = sdet_vPointPoint(pt, i->pt); //reference dir
    if (sdet_dot(ref_dir, theta)<0 || sdet_dot(ref_dir, i->tangent)<0)
      l*= -1; //negative direction

    //find the point on the ES around this arclength (assuming s~l)
    double d = vgl_distance(es_fit.point_at_length(l), i->pt);

    dist+=d;
  }

  //normalize by number of edgels
  dist/= edgel_chain.size();

  return dist;
}

//: report accuracy of measurement
void sdet_ES_curve_model::report_accuracy(double *estimates, double *min_estimates, double *max_estimates)
{
  double k_min=1000.0, k_max=-1000.0, gamma_min=1000.0, gamma_max=-1000.0;

  for (auto & i : cv_bundle[0])
  {
    if (i.x()<k_min) k_min = i.x();
    if (i.x()>k_max) k_max = i.x();
    if (i.y()<gamma_min) gamma_min = i.y();
    if (i.y()>gamma_max) gamma_max = i.y();
  }

  //report these numbers
  estimates[0] = theta;  //theta
  estimates[1] = k;      //curvature
  estimates[2] = gamma;  //curvature derivative

  min_estimates[0] = theta;
  min_estimates[1] = k_min;
  min_estimates[2] = gamma_min;

  max_estimates[0] = theta;
  max_estimates[1] = k_max;
  max_estimates[2] = gamma_max;

}

//: print info
void sdet_ES_curve_model::print_info()
{
  std::cout << " : (th=" << theta << ", k=" << k << ", gamma=" << gamma << " ";
}

//*****************************************************************************//
// Euler Spiral curve model with edgel perturbations
//*****************************************************************************//

//: Constructor 1: From a pair of edgels
sdet_ES_curve_model_perturbed::sdet_ES_curve_model_perturbed(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                                                               double dpos, double dtheta,
                                                               double token_len, double max_k, double max_gamma,
                                                               bool /*adaptive*/):
  sdet_ES_curve_model()
{
  //construct the curve bundle from the pair of edgels and the given uncertainty
  compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k, max_gamma);

}

//: constructor 3: From the intersection of two curve bundles
sdet_ES_curve_model_perturbed::sdet_ES_curve_model_perturbed(sdet_curve_model* cm1, sdet_curve_model* cm2):
  sdet_ES_curve_model()
{
  type = ES;

  //compute the curve bundle by intersection for each of the perturbations

  for (int i=0; i<Nperturb; i++)
  {
    pts[i] = ((sdet_ES_curve_model_perturbed*)cm1)->pts[i];
    tangents[i] = ((sdet_ES_curve_model_perturbed*)cm1)->tangents[i];
    cv_bundles[i] = vgl_clip(((sdet_ES_curve_model_perturbed*)cm1)->cv_bundles[i], ((sdet_ES_curve_model_perturbed*)cm2)->cv_bundles[i], vgl_clip_type_intersect);
    if (cv_bundles[i].num_sheets()>1)
      cv_bundles[i] = vgl_polygon<double>(); //remove these erroneous curve bundles
  }
}

//: construct and return a curve model of the same type by intersecting with another curve bundle
sdet_ES_curve_model_perturbed* sdet_ES_curve_model_perturbed::intersect(sdet_ES_curve_model_perturbed* cm)
{
  return new sdet_ES_curve_model_perturbed(this, cm);
}

//: is this bundle valid?
bool sdet_ES_curve_model_perturbed::bundle_is_valid()
{
  bool valid = false;

  for (auto & cv_bundle : cv_bundles)
    valid = valid || (cv_bundle.num_sheets()==1);

  return valid;
}


//: find the perturbed position and tangent given the index
void sdet_ES_curve_model_perturbed::compute_perturbed_position_of_an_edgel(sdet_edgel* e,
                                                                            int per_ind,
                                                                            double dpos, double dtheta,
                                                                            vgl_point_2d<double>& pt, double &tangent)
{
  int pos_i = per_ind/NperturbT;
  int pos_j = per_ind%NperturbT;

  double dp=0, dt=0;
  double NpP = NperturbP;
  double NpT = NperturbT;

  if(NpP>1)
    dp = dpos*(2*pos_i-NpP+1)/(NpP-1);

  if(NpT>1)
    dt = dtheta*(2*pos_j-NpT+1)/(NpT-1);

  //compute perturbed point and tangent corresponding to this position
  pt = e->pt + vgl_vector_2d<double>(dp*std::cos(e->tangent+vnl_math::pi/2),
                                     dp*std::sin(e->tangent+vnl_math::pi/2));
  tangent = e->tangent + dt;
}

//: compute the ES curve bundle for an edgel pair at the ref edgel
void sdet_ES_curve_model_perturbed::
compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                     double dpos, double dtheta,
                     double token_len, double max_k, double max_gamma)
{
  //for each discrete perturbation compute a curve bundle
  for (int i=0; i<Nperturb; i++)
  {
    //compute perturbe position
    compute_perturbed_position_of_an_edgel(ref_e, i, dpos, dtheta, pts[i], tangents[i]);

    //determine the intrinsic parameters for this edgel pair
    sdet_int_params params;
    if (ref_e == e1){
      params = sdet_get_intrinsic_params(pts[i], e2->pt, tangents[i], e2->tangent);

      //do the energy test
      bool test_passed = edgel_pair_legal(params, tangents[i], e2->tangent);
      //bool test_passed = edgel_pair_legal2(params, true);

      if (test_passed)
        sdet_ES_curve_model::compute_curve_bundle(cv_bundles[i], params, true, dpos, dtheta, token_len, max_k, max_gamma);
    }
    else {
      params = sdet_get_intrinsic_params(e1->pt, pts[i], e1->tangent, tangents[i]);

      //do the energy test
      bool test_passed = edgel_pair_legal(params, e1->tangent, tangents[i]);
      //bool test_passed = edgel_pair_legal2(params, false);

      if (test_passed)
        sdet_ES_curve_model::compute_curve_bundle(cv_bundles[i], params, false, dpos, dtheta, token_len, max_k, max_gamma);
    }
  }
}


//: Compute the best fit curve from the curve bundle
vgl_point_2d<double>  sdet_ES_curve_model_perturbed::compute_best_fit(std::deque<sdet_edgel*> &edgel_chain)
{
  //determine the best perturbation of the edgel by looking at the
  //curve bundles produced by each perturbation

  for (int i=0; i<Nperturb; i++){
    //assign the rough centroid of this bundle as the best estimate
    if (cv_bundles[i].num_sheets() != 0){

      //compute centroid
      ks[i] = 0.0;
      gammas[i] = 0.0;
      for (unsigned j=0; j<cv_bundles[i][0].size(); j++){
        ks[i] += cv_bundles[i][0][j].x();
        gammas[i] += cv_bundles[i][0][j].y();
      }
      ks[i] /= cv_bundles[i].num_vertices();
      gammas[i] /= cv_bundles[i].num_vertices();

      //compute minimum gamma -curvature point

      //is the centroid within the curve bundle?
      //assert(cv_bundles[i].contains(vgl_point_2d<double>(ks[i], gammas[i])));
    }
    else { //invalid curve bundles
      ks[i] = 1000.0;
      gammas[i] = 1000.0;
    }
  }

  //determine the best perturbation by looking at the cv bundles
  int ind = 0;
  //**************************************************
  // CRITERIA 1: The largest surviving curvelet wins
  //
  // There is no direct information about the size of the curvelets in this class
  // But since this object is formed for each curvelet, the largest curvelet probably only has
  // one of the perturbations producing a bundle so assign the surviving one as the best
  //**************************************************
  //for (int i=0; i<Nperturb; i++){
  //  if (cv_bundles[i].num_sheets() != 0)
  //    ind = i; //best bundle
  //}

  //**************************************************
  // CRITERIA 2: The tightest bundle is the best bundle
  //**************************************************
  //double smallest_area = 1000;
  //for (int i=0; i<Nperturb; i++){
  //  double bun_area = vgl_area(cv_bundles[i]);
  //  if (cv_bundles[i].num_sheets() != 0 &&
  //      bun_area<smallest_area)
  //  {
  //    smallest_area = bun_area;
  //    ind = i;
  //  }
  //}

  //**************************************************
  // CRITERIA 3: Determine the best fit wrt the centroid
  //**************************************************
  double dist = 1000;
  for (int i=0; i<Nperturb; i++){
    if (cv_bundles[i].num_sheets() != 0){
      double bun_dist = compute_distance(edgel_chain, pts[i], tangents[i], ks[i], gammas[i]);
      if (bun_dist<dist){
        dist = bun_dist;
        ind = i;
      }
    }
  }

  //Now assign the best curve bundle as the main curve bundle
  cv_bundle = cv_bundles[ind];

  pt = pts[ind];
  theta = tangents[ind];
  k = ks[ind];
  gamma = gammas[ind];

  return {k, gamma};
}

//: print info
void sdet_ES_curve_model_perturbed::print_info()
{
  std::cout << " : (th=" << theta << ", k=" << k << ", gamma=" << gamma << " " << std::endl;


  for (int i=0; i<Nperturb; i++){
    std::cout << "                             ";
    std::cout << "pos: " << i << " : (th=" << tangents[i] << ", k=" << ks[i] << ", gamma=" << gammas[i] << " " << std::endl;
  }
}
