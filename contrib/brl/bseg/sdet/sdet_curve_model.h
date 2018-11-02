// This is sdet_curve_model.h
#ifndef sdet_curve_model_h
#define sdet_curve_model_h
//:
//\file
//\brief Various curve mdoels used for linking using gemoetric consistency
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//
//  Ozge Can Ozcanli Jan 15, 2007   Added copy constructor
//  Ricardo Fabbri Sept 8, 2009     Optimized the CC_3d model
//
//\endverbatim

#include <vector>
#include <iostream>
#include <deque>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vbl/vbl_array_2d.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <sdet/sdet_edgel.h>
#include <sdet/sdet_sel_utils.h>

//: The base class for curve models used by the linking algorithm.
//  It contains the following:
//     a) a curve bundle as a region in the parameter space of the curve model.
//     b) methods to compute the bundle from edge measurements given expected measurement uncertainties.
//     c) extrinsic parameters of a representative curve (i.e., centroid of the bundle)
//
class sdet_curve_model
{
public:
  enum curve_type { LINEAR, CC, CC2, CC3d, ES };
  curve_type type;

  //: default constructor
  sdet_curve_model()= default;
  sdet_curve_model(curve_type t) : type(t) {}

  //: copy constructor
  sdet_curve_model(const sdet_curve_model& other) { type = other.type; }

  //: destructor
  virtual ~sdet_curve_model()= default;

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  //
  // Not virtual anymore for efficiency reasons
  //
  // virtual sdet_curve_model* intersect(sdet_curve_model* cm)=0;

  //: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
  virtual sdet_curve_model* transport(vgl_point_2d<double> /*pt*/, double /*theta*/){ return nullptr;}//temp :make it virtual

  //: construct and return a curve model of the same type by looking for legal transitions with another curve bundle
  virtual sdet_curve_model* transition(sdet_curve_model* /*cm*/, int& /*trans_k*/, int& /*trans_type*/, double & /*bundle_size*/){ return nullptr;}//temp :make it virtual

  //: is the bundle valid?
  virtual bool bundle_is_valid()=0;

  //: Compute the best fit curve from the curve bundle
  virtual vgl_point_2d<double> compute_best_fit(){ return {0,0}; }

  //: Compute the best fit curve from the curve bundle given the edgel chain
  virtual vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain)=0;

  //: function to check if the curve fit is reasonable
  virtual bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos)=0;

  //: report accuracy of measurement
  virtual void report_accuracy(double *estimates, double *min_estimates, double *max_estimates)=0;

  // utility functions

  //: are these two curve bundles C^2?
  virtual bool is_C2_with(sdet_curve_model* /*cm*/){ return false; }

  //: are these two curve bundles C^1?
  virtual bool is_C1_with(sdet_curve_model* /*cm*/){ return false; }

  //: print info
  virtual void print_info()=0;

  //: print central info to file
  virtual void print(std::ostream&) {
    std::cerr << "ERROR: Curvelet I/O for type "
      << type << " not implemented." << std::endl;
    abort();
  }

  //: read central info from file
  virtual void read(std::istream&) {
    std::cerr << "ERROR: Curvelet I/O for type "
      << type << " not implemented." << std::endl;
    abort();
  }
};

//: A base class for linear curve models
class sdet_linear_curve_model_base : public sdet_curve_model
{
public:
  vgl_point_2d<double> pt;       ///< best position estimate
  double theta;                  ///< best orientation estimate

  //: default constructor
  sdet_linear_curve_model_base(): pt(0.0,0.0), theta(0) { type = LINEAR; }

  //: constructor 1
  sdet_linear_curve_model_base(vgl_point_2d<double> new_pt, double new_theta): pt(new_pt), theta(new_theta) { type = LINEAR; }

  //: copy constructor
  sdet_linear_curve_model_base(const sdet_linear_curve_model_base& other) : sdet_curve_model(other)
  { pt = other.pt; theta = other.theta; }

  //: destructor
  ~sdet_linear_curve_model_base() override= default;

  //: print info
  void print_info() override
  {
    std::cout << " : (th=" << theta << ", k=0.0, gamma=0.0 ";
  }

};

//: A simple linear curve model class (Only perturbation in theta and not in position
class sdet_simple_linear_curve_model : public sdet_linear_curve_model_base
{
public:
  double min_theta, max_theta;   ///< range of theta (simple curve bundle)

  //: default constructor
  sdet_simple_linear_curve_model(): min_theta(0.0), max_theta(0.0) {}

  //: destructor
  ~sdet_simple_linear_curve_model() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_simple_linear_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_simple_linear_curve_model(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_simple_linear_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_simple_linear_curve_model(const sdet_simple_linear_curve_model& other) : sdet_linear_curve_model_base(other)
  { min_theta = other.min_theta; max_theta = other.max_theta; }

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_simple_linear_curve_model* intersect(sdet_simple_linear_curve_model * cm);

  //: is this bundle valid?
  //virtual bool bundle_is_valid() { return cv_bundle.num_sheets() != 0; }
  bool bundle_is_valid() override { return max_theta>min_theta; }

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  //: compute the linear curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

};

//: The linear curve model class
class sdet_linear_curve_model : public sdet_linear_curve_model_base
{
public:
  vgl_point_2d<double> ref_pt;   ///< original position estimate
  double ref_theta;              ///< original orientation estimate
  vgl_polygon<double> cv_bundle; ///< the region in dx-dtheta space defining the curve bundle

  //: default constructor
  sdet_linear_curve_model(): sdet_linear_curve_model_base(), ref_pt(0.0,0.0), ref_theta(0) { type = LINEAR; }

  //: destructor
  ~sdet_linear_curve_model() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_linear_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_linear_curve_model(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_linear_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_linear_curve_model(const sdet_linear_curve_model& other) : sdet_linear_curve_model_base(other)
  { ref_pt = other.ref_pt; ref_theta = other.ref_theta; cv_bundle = other.cv_bundle; }

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_linear_curve_model * intersect(sdet_linear_curve_model * cm);

  //: is this bundle valid?
  bool bundle_is_valid() override { return cv_bundle.num_sheets()==1; }

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  //: compute the linear curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

};

//: The circular arc curve model class
class sdet_CC_curve_model : public sdet_curve_model
{
public:
  vgl_polygon<double> cv_bundle; ///< the region in theta-k space defining the curve bundle
  vgl_point_2d<double> pt;       ///< best position estimate
  double tangent;               //stored extrinsic tangent

  double theta;                  ///< best orientation estimate
  double k;                      ///< best estimate of curvature

  //: default constructor
  sdet_CC_curve_model(): pt(0.0,0.0), tangent(0.0), theta(0), k(0.0) { type = CC; }

  //: constructor
  sdet_CC_curve_model(vgl_point_2d<double> new_pt, double new_tangent): pt(new_pt), tangent(new_tangent), theta(0), k(0.0) { type = CC; }

  //: destructor
  ~sdet_CC_curve_model() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_CC_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_CC_curve_model(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_CC_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_CC_curve_model(const sdet_CC_curve_model& other) : sdet_curve_model(other)
  { cv_bundle = other.cv_bundle; pt = other.pt; tangent = other.tangent; theta = other.theta; k = other.k; }

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_CC_curve_model *intersect(sdet_CC_curve_model * cm);

  //: is this bundle valid?
  bool bundle_is_valid() override { return cv_bundle.num_sheets()==1; }

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  //: compute the CC curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k);

  //: compute the CC curve bundle for an edgel pair given intrinsic params
  void compute_curve_bundle(vgl_polygon<double>& bundle, sdet_int_params &params, bool first_is_ref, double dpos, double dtheta, double token_len, double max_k);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

  //: print info
  void print_info() override;
};

#undef NkClasses
#define NkClasses 9

//const double sdet_k_classes[7] = {-0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3};
//const double sdet_k_classes[7] = {-0.01, -0.005, -0.002, 0.0, 0.002, 0.005, 0.01};
const double sdet_k_classes[9] = { -0.2, -0.1, -0.05, -0.01, 0.0, 0.01, 0.05, 0.1, 0.2};

//: The circular arc curve model class (new model)
class sdet_CC_curve_model_new : public sdet_curve_model
{
public:
  vgl_polygon<double> cv_bundles[NkClasses]; ///< the region in dx-dtheta space defining the curve bundle for each curvature

  vgl_point_2d<double> ref_pt;   ///< Extrinsic anchoring position
  double ref_theta;              ///< Extrinsic anchoring orientation

  vgl_point_2d<double> pt;       ///< best position estimate
  double theta;                  ///< best orientation estimate
  double k;                      ///< best estimate of curvature

  //: default constructor
  sdet_CC_curve_model_new(): ref_pt(0.0, 0.0), ref_theta(0.0), pt(0.0,0.0), theta(0), k(0.0) { type = CC2; }

  //: constructor
  sdet_CC_curve_model_new(vgl_point_2d<double> new_pt, double new_tangent): ref_pt(new_pt), ref_theta(new_tangent), pt(new_pt), theta(new_tangent), k(0.0) { type = CC2; }

  //: destructor
  ~sdet_CC_curve_model_new() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_CC_curve_model_new(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_CC_curve_model_new(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm1, sdet_CC_curve_model_new* cm2);

  //: constructor 4: By transporting a curve model to another extrinsic anchor point
  sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm, vgl_point_2d<double> pt, double theta);

  //: copy constructor
  sdet_CC_curve_model_new(const sdet_CC_curve_model_new& other);

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_CC_curve_model_new *intersect(sdet_CC_curve_model_new * cm);

  //: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
  sdet_curve_model* transport(vgl_point_2d<double> pt, double theta) override;

  //: construct and return a curve model of the same type by looking for legal transitions with another curve bundle
  sdet_curve_model* transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double & bundle_size) override;

  //: construct and return a curve bundle of the same type by looking for legal transitions with another curve bundle
  sdet_curve_model* consistent_transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double &gc_cost);

  //: construct and return a curve bundle of the same type by looking for legal C1 transitions with another curve bundle
  sdet_curve_model* C1_transition(sdet_curve_model* cm);

  //: is this bundle valid?
  bool bundle_is_valid() override;

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  //: compute the CC curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k);

  //: Transport a curve bundle from one anchor to the current anchor
  vgl_polygon<double> transport_bundle(const vgl_polygon<double> & cv_bundle, double k,
                                       vgl_point_2d<double> spt, double stheta, vgl_point_2d<double> dpt, double dtheta, bool & sgn_change);

  //: Transport a curve bundle from the given anchor to the current anchor
  vgl_polygon<double> transport_bundle_from(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta, bool & sgn_change);

  //: Transport a curve bundle from the current anchor to a new anchor
  vgl_polygon<double> transport_bundle_to(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta, bool & sgn_change);

  //: Transport a single CC curve
  vgl_point_2d<double> transport_CC(vgl_point_2d<double>dx_dt, double k,
                                    vgl_point_2d<double> spt, double stheta,
                                    vgl_point_2d<double> dpt, double dtheta,
                                    bool & sgn_change);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit() override;

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: Set the best fit curve
  virtual void set_best_fit(vgl_point_2d<double> dx_dt, double k);

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

  //: print info
  void print_info() override;

  //: print central info to file
  void print(std::ostream&) override;

  //: read central info from file
  void read(std::istream&) override;
};

////: The circular arc curve model class (new model)
//class sdet_CC_curve_model_new : public sdet_curve_model
//{
//public:
//  vgl_polygon<double> cv_bundles[NkClasses]; ///< the region in dx-dtheta space defining the curve bundle for each curvature
//
//  vgl_point_2d<double> ref_pt;   ///< Extrinsic anchoring position
//  double ref_theta;              ///< Extrinsic anchoring orientation
//
//  vgl_point_2d<double> pt;       ///< best position estimate
//  double theta;                  ///< best orientation estimate
//  double k;                      ///< best estimate of curvature
//
//  //: default constructor
//  sdet_CC_curve_model_new(): ref_pt(0.0, 0.0), ref_theta(0.0), pt(0.0,0.0), theta(0), k(0.0) { type = CC2; }
//
//  //: constructor
//  sdet_CC_curve_model_new(vgl_point_2d<double> new_pt, double new_tangent): ref_pt(new_pt), ref_theta(new_tangent), pt(new_pt), theta(new_tangent), k(0.0) { type = CC2; }
//
//  //: destructor
//  virtual ~sdet_CC_curve_model_new(){}
//
//  //: Constructor 1: From a pair of edgels
//  sdet_CC_curve_model_new(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);
//
//  //: constructor 2: From a set of edgels
//  sdet_CC_curve_model_new(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}
//
//  //: constructor 3: From the intersection of two curve bundles
//  sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm1, sdet_CC_curve_model_new* cm2);
//
//  //: constructor 4: By transporting a curve model to another extrinsic anchor point
//  sdet_CC_curve_model_new(sdet_CC_curve_model_new* cm, vgl_point_2d<double> pt, double theta);
//
//  //: copy constructor
//  sdet_CC_curve_model_new(const sdet_CC_curve_model_new& other);
//
//  //: construct and return a curve model of the same type by intersecting with another curve bundle
//  virtual sdet_curve_model* intersect(sdet_curve_model* cm);
//
//  //: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
//  virtual sdet_curve_model* transport(vgl_point_2d<double> pt, double theta);
//
//  //: construct and return a curve model of the same type by looking for legal transitions with another curve bundle
//  virtual sdet_curve_model* transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double & bundle_size);
//
//  //: construct and return a curve bundle of the same type by looking for legal transitions with another curve bundle
//  sdet_curve_model* consistent_transition(sdet_curve_model* cm, int& trans_k, int& trans_type, double &gc_cost);
//
//  //: construct and return a curve bundle of the same type by looking for legal C1 transitions with another curve bundle
//  sdet_curve_model* C1_transition(sdet_curve_model* cm);
//
//  //: is this bundle valid?
//  virtual bool bundle_is_valid();
//
//  //: determine if edgel pair is legal
//  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);
//
//  //: compute the CC curve bundle for an edgel pair given expected errors
//  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k);
//
//  //: Transport a curve bundle from one anchor to the current anchor
//  vgl_polygon<double> transport_bundle(const vgl_polygon<double> & cv_bundle, double k,
//                                       vgl_point_2d<double> spt, double stheta, vgl_point_2d<double> dpt, double dtheta);
//
//  //: Transport a curve bundle from the given anchor to the current anchor
//  vgl_polygon<double> transport_bundle_from(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta);
//
//  //: Transport a curve bundle from the current anchor to a new anchor
//  vgl_polygon<double> transport_bundle_to(const vgl_polygon<double> & cv_bundle, double k, vgl_point_2d<double> pt, double theta);
//
//  //: Transport a single CC curve
//  vgl_point_2d<double> transport_CC(vgl_point_2d<double>dx_dt, double k,
//                                    vgl_point_2d<double> spt, double stheta,
//                                    vgl_point_2d<double> dpt, double dtheta,
//                                    bool & valid);
//
//  //: Compute the best fit curve from the curve bundle
//  virtual vgl_point_2d<double> compute_best_fit();
//
//  //: Compute the best fit curve from the curve bundle
//  virtual vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain);
//
//  //: Set the best fit curve
//  virtual void set_best_fit(vgl_point_2d<double> dx_dt, double k);
//
//  //: function to check if the curve fit is reasonable
//  virtual bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos);
//
//  //: report accuracy of measurement
//  virtual void report_accuracy(double *estimates, double *min_estimates, double *max_estimates);
//
//  //: print info
//  virtual void print_info();
//
//  //: print central info to file
//  virtual void print(std::ostream&);
//
//  //: read central info from file
//  virtual void read(std::istream&);
//};

//discrete perturbations in position
#undef NperturbPCC
#define NperturbPCC 3

//: The circular arc curve model class that also includes the edgel perturbations
class sdet_CC_curve_model_perturbed : public sdet_CC_curve_model
{
public:
  vgl_polygon<double> cv_bundles[NperturbPCC]; //the bundles corresponding to the perturbed positions

  vgl_point_2d<double> pts[NperturbPCC];  //stored perturbed positions
  double dts[NperturbPCC];                //centroids of perturbed cv bundles
  double ks[NperturbPCC];

  //: default constructor
  sdet_CC_curve_model_perturbed(): sdet_CC_curve_model(){}

  //: destructor
  ~sdet_CC_curve_model_perturbed() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_CC_curve_model_perturbed(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_CC_curve_model_perturbed(std::vector<sdet_edgel*> edgels, sdet_edgel* ref_edgel): sdet_CC_curve_model(edgels, ref_edgel){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_CC_curve_model_perturbed(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_CC_curve_model_perturbed(const sdet_CC_curve_model_perturbed& other) : sdet_CC_curve_model(other)
  { for (unsigned i = 0; i < NperturbPCC; i++)
      cv_bundles[i] = other.cv_bundles[i];
    for (unsigned i = 0; i < NperturbPCC; i++)
      pts[i] = other.pts[i];
    for (unsigned i = 0; i < NperturbPCC; i++)
      dts[i] = other.dts[i];
    for (unsigned i = 0; i < NperturbPCC; i++)
      ks[i] = other.ks[i];
  }

  //: find the perturbed position of an edgel given the index
  void compute_perturbed_position_of_an_edgel(sdet_edgel* e,
                                              int per_ind,
                                              double dpos, double dtheta,
                                              vgl_point_2d<double>& pt);

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_CC_curve_model_perturbed* intersect(sdet_CC_curve_model_perturbed* cm);

  //: is this bundle valid?
  bool bundle_is_valid() override;

  //: compute the CC curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: print info
  void print_info() override;
};

//define min and max functions for vbl_array_2d

//: compute the min of each element and create a new array
// User is responsible for providing an array of the right size.
template <class T>
inline void vbl_array_2d_min(
    const vbl_array_2d<T> &a1,
    const vbl_array_2d<T> &a2,
    vbl_array_2d<T> &out
    )
{
  assert(a1.size() == a2.size()); //make sure the arrays are the same size
  assert(out.size() == a1.size());

  for (unsigned i=0; i < a1.size(); ++i)
    if (a1.begin()[i] < a2.begin()[i])
      out.begin()[i] = a1.begin()[i];
    else
      out.begin()[i] = a2.begin()[i];
}

//: compute the max of each element and create a new array
// User is responsible for providing an array of the right size.
template <class T>
inline void vbl_array_2d_max(
    const vbl_array_2d<T> &a1,
    const vbl_array_2d<T> &a2,
    vbl_array_2d<T> &out
    )
{
  assert(a1.size() == a2.size()); //make sure the arrays are the same size
  assert(out.size() == a1.size());

  for (unsigned i=0; i < a1.size(); ++i)
    if (a1.begin()[i] > a2.begin()[i])
      out.begin()[i] = a1.begin()[i];
    else
      out.begin()[i] = a2.begin()[i];
}

//: compute the min of each element and store in a1
template <class T>
inline void vbl_array_2d_min_replace(vbl_array_2d<T> &a1, const vbl_array_2d<T> &a2)
{
  assert(a1.size()==a2.size()); //make sure the arrays are the same size

  for (unsigned i=0; i<a1.rows(); ++i){
    for (unsigned j=0; j<a1.cols(); ++j){
      if (a2(i,j)<a1(i,j))
        a1(i,j) = a2(i,j);
    }
  }
}

//: compute the max of each element and store in a1
template <class T>
inline void vbl_array_2d_max_replace(vbl_array_2d<T> &a1, const vbl_array_2d<T> &a2)
{
  assert(a1.size()==a2.size()); //make sure the arrays are the same size

  for (unsigned i=0; i<a1.rows(); ++i){
    for (unsigned j=0; j<a1.cols(); ++j){
      if (a2(i,j)>a1(i,j))
        a1(i,j) = a2(i,j);
    }
  }
}


//Note: The grid size of the perturbations in position and orientation should be a parameter
// in the algorithm but for now I'm gonna keep them as constants
#undef _dx_
#define _dx_ 0.1 //pixels

#undef _dt_
#define _dt_ 0.08 //radians

//: The circular arc curve model class (3d curve bundle)
class sdet_CC_curve_model_3d : public sdet_curve_model
{
public:
  vbl_array_2d<float> Kmin, Kmax; ///< The min and max curvature surfaces for the bundle (as a function of dx, dt)
  //std::vector<double> Dx, Dt;       ///< The values of the Dx and Dt parameters at the grid points

  vgl_point_2d<double> ref_pt;   ///< Extrinsic anchoring position
  double ref_theta;              ///< Extrinsic anchoring orientation

  vgl_point_2d<double> pt;       ///< best position estimate
  double theta;                  ///< best orientation estimate
  double k;                      ///< best estimate of curvature

  //: default constructor
  sdet_CC_curve_model_3d():
    Kmin(1, 1, 0.0f), Kmax(1, 1, 0.0f), ref_pt(0.0, 0.0), ref_theta(0.0), pt(0.0,0.0), theta(0), k(0.0) { type = CC3d; }

  //: constructor
  sdet_CC_curve_model_3d(vgl_point_2d<double> new_pt, double new_tangent):
    Kmin(1, 1, 0.0f), Kmax(1, 1, 0.0f), ref_pt(new_pt), ref_theta(new_tangent), pt(new_pt), theta(new_tangent), k(0.0) { type = CC3d; }

  //: destructor
  ~sdet_CC_curve_model_3d() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_CC_curve_model_3d(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e,
                           double dpos, double dtheta,
                           double token_len, double max_k, double /*max_gamma*/,
                           bool adaptive):
    Kmin(1, 1, (float)-max_k), Kmax(1, 1, (float)max_k), ref_pt(ref_e->pt), ref_theta(ref_e->tangent), pt(0.0,0.0), theta(0), k(0.0)
  {
    type = CC3d;

    //construct the curve bundle from the pair of edgels and the given uncertainty
    compute_curve_bundle(e1, e2, ref_e, dpos, dtheta, token_len, max_k, adaptive);
  }


  //: constructor 2: From a set of edgels
  sdet_CC_curve_model_3d(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_CC_curve_model_3d(sdet_CC_curve_model_3d* cm1, sdet_CC_curve_model_3d* cm2)
    :
    sdet_curve_model(CC3d),
    Kmin(cm1->Kmin.rows(), cm1->Kmin.cols()),
    Kmax(cm1->Kmax.rows(), cm1->Kmax.cols()),
    ref_pt(cm1->ref_pt),
    ref_theta(cm1->ref_theta),
    pt(0.0,0.0),
    theta(0),
    k(0.0)
  {
    //take the intersection of the two bundles by intersecting the four bounding surfaces
    vbl_array_2d_max(cm1->Kmin, cm2->Kmin, Kmin);
    vbl_array_2d_min(cm1->Kmax, cm2->Kmax, Kmax);

    //Dx = cm1->Dx;
    //Dt = cm1->Dt;
  }

  //: constructor 4: By transporting a curve model to another extrinsic anchor point
  sdet_CC_curve_model_3d(sdet_CC_curve_model_3d* cm, vgl_point_2d<double> pt, double theta);

  //: constructor 5: Create default bundle only
  sdet_CC_curve_model_3d(sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: copy constructor
  sdet_CC_curve_model_3d(const sdet_CC_curve_model_3d& other);

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_CC_curve_model_3d* intersect(sdet_CC_curve_model_3d* cm);

  //: construct and return a curve model of the same type by transporting it to a new extrinsic anchor
  sdet_curve_model* transport(vgl_point_2d<double> pt, double theta) override;

  //: get the value of the position perturbation at a grid point
  double Dx(unsigned i)
  { return _dx_*((double)i-(double(Kmax.rows())-1.0)/2.0); }

  //: get the value of the orientation perturbation at a grid point
  double Dt(unsigned j)
  { return _dt_*((double)j-(double(Kmax.cols())-1.0)/2.0); }

  //: is this bundle valid?
  bool bundle_is_valid() override
  {
    //if the Kmax surface is completely less than the Kmin surface, there is no bundle
    for (unsigned i=0; i<Kmax.rows()*Kmax.cols(); i++)
        if (Kmax.begin()[i] > Kmin.begin()[i]) return true;

    return false;
  }

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  //: compute the CC curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, bool adaptive);

  //: Transport a single CC curve
  vgl_point_2d<double> transport_CC(vgl_point_2d<double>dx_dt, double k,
                                    vgl_point_2d<double> spt, double stheta,
                                    vgl_point_2d<double> dpt, double dtheta,
                                    bool & sgn_change);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit() override;

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: Set the best fit curve
  virtual void set_best_fit(vgl_point_2d<double> dx_dt, double k);

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

  // utility functions

  //: are these two curve bundles C^2?
  bool is_C2_with(sdet_curve_model* cm) override;

  //: are these two curve bundles C^1?
  bool is_C1_with(sdet_curve_model* cm) override;

  //: get the size of the bundle for computing the saliency heuristic
  double get_CC_3d_bundle_area();

  //: print info
  void print_info() override;

  //: print central info to file
  void print(std::ostream&) override;

  //: read central info from file
  void read(std::istream&) override;
};


//: The Euler spiral curve model class
class sdet_ES_curve_model : public sdet_curve_model
{
public:
  vgl_polygon<double> cv_bundle; ///< the region in k-gamma space defining the curve bundle
  vgl_point_2d<double> pt;       ///< best position estimate
  double theta;                  ///< best orientation estimate
  double k;                      ///< best estimate of curvature
  double gamma;                  ///< best estimate of the curvature derivative

  //: default constructor
  sdet_ES_curve_model(): pt(0.0,0.0), theta(0), k(0.0), gamma(0.0) { type = ES; }

  //: destructor
  ~sdet_ES_curve_model() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_ES_curve_model(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_ES_curve_model(std::vector<sdet_edgel*> /*edgels*/, sdet_edgel* /*ref_edgel*/): k(0.0), gamma(0.0){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_ES_curve_model(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_ES_curve_model(const sdet_ES_curve_model& other) : sdet_curve_model(other)
  { cv_bundle = other.cv_bundle; pt = other.pt; theta = other.theta; k = other.k; gamma = other.gamma; }

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_ES_curve_model* intersect(sdet_ES_curve_model *cm);

  //: is this bundle valid?
  bool bundle_is_valid() override { return cv_bundle.num_sheets()==1; }

  //: determine if edgel pair is legal
  bool edgel_pair_legal(sdet_int_params &params, double tan1, double tan2);

  ////: determine if an edgel pair can be formed between these edgels
  //bool edgel_pair_legal1(sdet_edgel* e1, sdet_edgel* e2,
  //                       sdet_edgel* ref_e, vgl_polygon<double> & cb);

  ////: determine if an edgel pair can be formed between these edgels
  //bool edgel_pair_legal2(sdet_edgel* e1, sdet_edgel* e2,
  //                       sdet_edgel* ref_e, vgl_polygon<double> & cb);

  ////: determine if an edgel pair can be formed between these edgels
  //bool edgel_pair_legal2(sdet_int_params &params, bool first_is_ref);

  //: compute the ES curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma);

  //: compute the ES curve bundle for an edgel pair given intrinsic params
  void compute_curve_bundle(vgl_polygon<double>& bundle, sdet_int_params &params,
                            bool first_is_ref, double dpos, double dtheta, double token_len, double max_k, double max_gamma);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: function to check if the curve fit is reasonable
  bool curve_fit_is_reasonable(std::deque<sdet_edgel*> &edgel_chain, sdet_edgel* ref_e, double dpos) override;

  //: compute the distance between edgels and a curve model
  double compute_distance(std::deque<sdet_edgel*> &edgel_chain,
                          vgl_point_2d<double>pt, double theta,
                          double k, double gamma);

  //: report accuracy of measurement
  void report_accuracy(double *estimates, double *min_estimates, double *max_estimates) override;

  //: print info
  void print_info() override;
};

//discrete perturbations in position
#undef NperturbP
#define NperturbP 1

//discrete perturbations in orientation
#undef NperturbT
#define NperturbT 9

//total # of perturbations
#undef Nperturb
#define Nperturb (NperturbP*NperturbT)

//: The Euler spiral curve model class that also includes the edgel perturbations
class sdet_ES_curve_model_perturbed : public sdet_ES_curve_model
{
public:
  vgl_polygon<double> cv_bundles[Nperturb]; //the bundles corresponding to the perturbed positions

  vgl_point_2d<double> pts[Nperturb]; //stored perturbed positions
  double tangents[Nperturb];          //stored perturbed tangents
  double ks[Nperturb];                //centroids of perturbed cv bundles
  double gammas[Nperturb];

  //: default constructor
  sdet_ES_curve_model_perturbed() : sdet_ES_curve_model(){}

  //: destructor
  ~sdet_ES_curve_model_perturbed() override= default;

  //: Constructor 1: From a pair of edgels
  sdet_ES_curve_model_perturbed(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma, bool adaptive);

  //: constructor 2: From a set of edgels
  sdet_ES_curve_model_perturbed(std::vector<sdet_edgel*> edgels, sdet_edgel* ref_edgel): sdet_ES_curve_model(edgels, ref_edgel){}

  //: constructor 3: From the intersection of two curve bundles
  sdet_ES_curve_model_perturbed(sdet_curve_model* cm1, sdet_curve_model* cm2);

  //: copy constructor
  sdet_ES_curve_model_perturbed(const sdet_ES_curve_model_perturbed& other) : sdet_ES_curve_model(other)
  {
    for (unsigned i = 0; i < Nperturb; i++)
      cv_bundles[i] = other.cv_bundles[i];
    for (unsigned i = 0; i < Nperturb; i++)
      pts[i] = other.pts[i];
    for (unsigned i = 0; i < Nperturb; i++)
      tangents[i] = other.tangents[i];
    for (unsigned i = 0; i < Nperturb; i++)
      ks[i] = other.ks[i];
    for (unsigned i = 0; i < Nperturb; i++)
      gammas[i] = other.gammas[i];
  }

  //: find the perturbed position and tangent of an edgel given the index
  void compute_perturbed_position_of_an_edgel(sdet_edgel* e,
                                              int per_ind,
                                              double dpos, double dtheta,
                                              vgl_point_2d<double>& pt, double &tangent);

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  sdet_ES_curve_model_perturbed* intersect(sdet_ES_curve_model_perturbed* cm);

  //: is this bundle valid?
  bool bundle_is_valid() override;

  //: compute the ES curve bundle for an edgel pair given expected errors
  void compute_curve_bundle(sdet_edgel* e1, sdet_edgel* e2, sdet_edgel* ref_e, double dpos, double dtheta, double token_len, double max_k, double max_gamma);

  //: Compute the best fit curve from the curve bundle
  vgl_point_2d<double> compute_best_fit(std::deque<sdet_edgel*> &edgel_chain) override;

  //: print info
  void print_info() override;
};

#endif // sdet_curve_model_h
