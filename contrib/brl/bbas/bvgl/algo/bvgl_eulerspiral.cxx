// This is basic/bvgl/algo/bvgl_eulerspiral.cxx
//:
// \file

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include "bvgl_eulerspiral.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <bnl/algo/bnl_fresnel.h>
#include <bvgl/algo/bvgl_biarc.h>


//some defines for Euler-spiral optimization
constexpr int bvgl_eulerspiral_max_gradient_descent_iter = 50000;  // maxinum number of iterations for gradient descent
const double bvgl_eulerspiral_e_error = 1e-5;   // epsilon for error
const double bvgl_eulerspiral_e_gamma = 1e-8;   // epsilon for gamma
const double bvgl_eulerspiral_e_k = 1e-4;   //Epsilon for curvature


//: ============ bvgl_eulerspiral_optimization_function class ========
class bvgl_eulerspiral_optimization_function : public vnl_least_squares_function
{
public:
  friend class bvgl_eulerspiral;
  ~bvgl_eulerspiral_optimization_function() override = default;
  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  //: return pointer to the Euler spiral of this function
  bvgl_eulerspiral* es() const { return this->es_; }
  //: set Euler spiral pointer
  void set_es(bvgl_eulerspiral * es){ this->es_ = es; }

private:
  //: disabled contructor, can only construct this function inside an eulerspiral class
  bvgl_eulerspiral_optimization_function(unsigned int number_of_unknowns,
    unsigned int number_of_residuals,UseGradient g = use_gradient);
  bvgl_eulerspiral* es_;
};


// =============== bvgl_eulerspiral class =========================================

// -------------- INITIALIZERS/CONSTRUCTORS/DESTRUCTORS ----------------------------

//: Constructor - from intrinsic parameters
bvgl_eulerspiral::
bvgl_eulerspiral(vgl_point_2d<double > start, double start_angle, double k0, double gamma, double len)
: start_(start), k0_(k0), gamma_(gamma), len_(len)
{
  this->set_start_angle(start_angle);
}

//: Constructor - from 2 point-tangents
bvgl_eulerspiral::
bvgl_eulerspiral(vgl_point_2d< double > start, double start_angle,
                 vgl_point_2d< double > end, double end_angle )
{
  this->set_start_params(start, start_angle);
  this->set_end_params(end, end_angle);
  this->compute_es_params();
}

//: Copy Constructor
bvgl_eulerspiral::
bvgl_eulerspiral(const bvgl_eulerspiral & es )
  : start_(es.start_), k0_(es.k0_),
  gamma_(es.gamma_), len_(es.len_)
{
  this->set_start_angle(es.start_angle());
}


// ------------ ACCESS MEMBER VARIABLES --------------------------------------

//: Set the start angle, converted to the range [0, 2*pi)
void bvgl_eulerspiral::
set_start_angle( double start_angle ){
  double theta = std::fmod(start_angle, vnl_math::pi * 2);
  if (theta < 0.0)
    theta = theta + vnl_math::pi * 2;
  this->start_angle_ = theta;
}

//: Set end angle of the biarc, converted to the range [0, 2*pi)
void bvgl_eulerspiral::
set_end_angle( double end_angle ){
  double theta = std::fmod(end_angle, vnl_math::pi * 2);
  if (theta < 0.0)
    theta = theta + vnl_math::pi * 2;
  this->end_angle_ = theta;
}


// ---------------- GEOMETRY FUNCTIONS -----------------------

//: Return a point on the eulerspiral with s arclength away from starting point
vgl_point_2d< double > bvgl_eulerspiral::
point_at_length( double s) const {
  return this->compute_end_pt(this->k0(), this->gamma(), s);
}

vgl_point_2d< double > bvgl_eulerspiral::
point_at( double s) const {
  return point_at_length(s*length());
}

//: Return tangent of a point on the eulerspiral with s arclength away from starting point
vgl_vector_2d< double > bvgl_eulerspiral::
tangent_at_length( double s) const {
  double angle;
  angle = this->start_angle() + s*(this->k0() + 0.5*this->gamma()*s);
  return {std::cos(angle), std::sin(angle)};
}

vgl_vector_2d< double > bvgl_eulerspiral::
tangent_at( double s) const {
  return tangent_at_length(s*length());
}

//: Returns the angle (in radian) in [0, 2PI] of the tangent at arclength s
// of the parameter along the curve.
double bvgl_eulerspiral::tangent_angle_at_length(double s) const {
  double angle = this->start_angle() + s*(this->k0() + 0.5*this->gamma()*s);
  // make sure angle is in [0, 2pi)
  angle = std::fmod(angle, vnl_math::pi*2);
  if (angle <0)
    angle = angle + vnl_math::pi*2;
  return angle;
}

double bvgl_eulerspiral::tangent_angle_at(double s) const {
  return tangent_angle_at_length(s*length());
}

//: Return curvature of a point on the eulerspiral with s arclength away from starting point
double bvgl_eulerspiral::
curvature_at_length( double s ) const {
  return this->k0() + s * this->gamma();
}

double bvgl_eulerspiral::
curvature_at( double s ) const {
  return curvature_at_length(s*length());
}


//: compute extrinsic points of eulerspiral
void bvgl_eulerspiral::
compute_spiral(std::vector<vgl_point_2d<double> >& spiral, double ds, int npts){
  // check if default values are applied
  if (ds==0 && npts==0){
    //use default values
    npts = 100;
    ds = this->len_/npts;
  }

  // check validity of the input parameters
  spiral.clear();
  if (ds <= 0)
    return;
  if ((npts == 0) || (npts*ds > this->len_))
    npts = (int) (this->len_ / ds);

  // increment s and compute corresponding eulerspiral point
  double s = ds;
  spiral.push_back(this->start());
  for (int i = 1; i<npts; i++){
    s = i * ds;
    vgl_point_2d< double> cur_pt = this->point_at_length(s);
    spiral.push_back(cur_pt);
  }
  if (npts * ds < this->len_)
    spiral.push_back(this->point_at_length(this->len_));

}

// ---------------- SUPPORT FUNCTIONS ---------------------------

//: Set the core parameters of the eulerspiral
void bvgl_eulerspiral::
init(const vgl_point_2d<double >& start, double start_angle, double k0, double gamma, double len){
  this->set_start(start);
  this->set_start_angle(start_angle);
  this->set_k0(k0);
  this->set_gamma(gamma);
  this->set_len(len);
}

//: Set intrinsic core parameters of the eulerspiral
void bvgl_eulerspiral::
set_intrinsic_params(double k0, double gamma, double len){
  this->set_k0(k0);
  this->set_gamma(gamma);
  this->set_len(len);
}

//: Set extrinsic core parameters of the eulerspiral
void bvgl_eulerspiral::
set_extrinsic_params(const vgl_point_2d< double >& start, double start_angle){
  this->set_start(start);
  this->set_start_angle(start_angle);
}

//: set parameters at starting point
void bvgl_eulerspiral::
set_start_params( vgl_point_2d< double > start, double start_angle ){
  this->set_start(start);
  this->set_start_angle(start_angle);
}

//: set parameters at ending point
void bvgl_eulerspiral::
set_end_params( vgl_point_2d< double > end, double end_angle ){
  this->set_end(end);
  this->set_end_angle(end_angle);
}

//: set parameters at both starting point and end point
void bvgl_eulerspiral::
set_params( vgl_point_2d< double > start, double start_angle,
           vgl_point_2d< double > end, double end_angle )
{
  this->set_start_params(start, start_angle);
  this->set_end_params(end, end_angle);
}

//: Compute intrinsic parameters of eulerspiral using starting and ending point-tangents
bool bvgl_eulerspiral::
compute_es_params( bool use_lookup_table, bool use_levenberg_marquardt ){
  if (use_levenberg_marquardt)
    return this->compute_es_params_use_levenberg_marquardt(use_lookup_table);
  else
    return this->compute_es_params_use_simple_gradient_descent(use_lookup_table);
}

//: Compute intrinsic parameters of eulerspiral given two point-tangents
bool bvgl_eulerspiral::
compute_es_params( vgl_point_2d< double > start, double start_angle,
                  vgl_point_2d< double > end, double end_angle )
{
  this->set_params(start, start_angle, end, end_angle);
  return this->compute_es_params();
}


//: comparison operator.
//  Comparison is on the curve, two parametric curves are identical if their
//  equations are equivalent
bool bvgl_eulerspiral::
operator==(const bvgl_eulerspiral& that) const{
  return (this->start() == that.start() &&
    this->start_angle() == that.start_angle() &&
    this->k0() == that.k0()&&
    this->gamma() == that.gamma() &&
    this->length() == that.length()
    );
};

//: assignment operator
bvgl_eulerspiral& bvgl_eulerspiral::
operator=( bvgl_eulerspiral const& that){
  this->init(that.start(), that.start_angle(), that.k0(), that.gamma(), that.length());
  return (*this);
}

// ---------------- MISCELLANEOUS ----------------------
//: Print parameters of the eulerspiral
void bvgl_eulerspiral::
print(std::ostream &os ){
  os << std::endl << "Start parameters" << std::endl;
  os << "Start point = ( "<< this->start().x() << " , " << this->start().y() << " )" << std::endl;
  os << "Start angle = " << this->start_angle() << std::endl;
  os << "End parameters" << std::endl;
  os << "End point = ( "<< this->end().x() << " , " << this->end().y() << " )" << std::endl;
  os << "End angle = " << this->end_angle() << std::endl;

  os << "k0 = " << this->k0() << std::endl;
  os << "gamma = " << this->gamma() << std::endl;
  os << "len = " << this->len_ << std::endl;
  os << "turning angle = " << this->turning_angle_ << std::endl;
  os << "error = " << this->error_ << std::endl;
  os << "psi = " << this->psi_ << std::endl;
  os << "Number of iterations for last optimization = " << this->num_iterations() << std::endl;
  os << "Number of end-point function evaluation for last optimization = "
    << this->num_evaluations() << std::endl;
}


// -------------------- INTERNAL SUPPORT FUNCTIONS -----------------------------

//: Return error: euclidean distance between end-point of the Euler spiral with the given
// intrinsic params and the desired end-point
inline double bvgl_eulerspiral::
compute_error(double k0, double len){
  // assumes normalized parameters
  // compute the endpoint of the Euler spiral with the given intrinsic parameters
  double gamma = 2*(this->turning_angle_ - k0 * len) / (len * len);
  vgl_point_2d< double > cur_end_pt = this->compute_end_pt(k0, gamma, len, true);

  //the error is the distance between the current end point and the desired end point
  vgl_vector_2d< double > t = cur_end_pt - vgl_point_2d< double >(1, 0);
  return t.length();
}


//: Return end point of the Euler spiral with the given intrinsic paramters
vgl_point_2d< double > bvgl_eulerspiral::
compute_end_pt( double k0, double gamma, double len, bool normalized ) const {
  // increment num_evaluations
  this->num_evaluations_ += 1;

  vgl_point_2d< double > start_pt;
  vgl_point_2d< double > end_pt;
  double theta;

  if (normalized){
    start_pt.set(0, 0);
    // convert this->psi - this->start_angle to the range [0, 2pi) and assign to theta
    theta = std::fmod(this->start_angle()-this->psi_, 2*vnl_math::pi);
    theta = (theta < 0)? theta+2*vnl_math::pi : theta;
  }
  else{
    start_pt = this->start();
    theta = this->start_angle();
  }

  if (len == 0)
    return start_pt;

  if ( std::fabs(gamma) < bvgl_eulerspiral_e_gamma ){
    if ( std::fabs(k0) < bvgl_eulerspiral_e_k ){
      //straight line
      end_pt = start_pt + vgl_vector_2d< double >(std::cos(theta), std::sin(theta)) * len;
    }
    else
    {
      //circle
      double const_term = 1.0/k0;
      end_pt.set(start_pt.x()+ const_term*(std::sin(k0*len+theta)-std::sin(theta)),
        start_pt.y()+ const_term*(-std::cos(k0*len+theta)+std::cos(theta)));
    }
    return end_pt;
  }

  double const1 = std::sqrt( vnl_math::pi*std::fabs(gamma) );
  double const2 = std::sqrt( vnl_math::pi/std::fabs(gamma) );

  double fresnel1_cos, fresnel1_sin;
  double fresnel2_cos, fresnel2_sin;
  bnl_fresnel_integral((k0+gamma*len)/const1, &fresnel1_cos, &fresnel1_sin );
  bnl_fresnel_integral( k0/const1, &fresnel2_cos, &fresnel2_sin );

  double c = fresnel1_cos - fresnel2_cos;
  c = (gamma < 0) ? -c : c;

  double s = fresnel1_sin - fresnel2_sin;

  double cos_term = std::cos(theta-((k0*k0)/(2.0*gamma)));
  double sin_term = std::sin(theta-((k0*k0)/(2.0*gamma)));

  end_pt.set(start_pt.x()+const2*(c*cos_term - s*sin_term),
    start_pt.y()+const2*(c*sin_term + s*cos_term));

  return end_pt;
};


//: compute Euler spiral core intrinsic parameters from starting and ending point-tangent using
// simple gradient descent algorithm
bool bvgl_eulerspiral::
compute_es_params_use_simple_gradient_descent( bool use_lookup_table ){
  this->num_iterations_ = 0;
  this->num_evaluations_ = 0;

  //compute scaling distance
  vgl_vector_2d< double > v = this->end() - this->start();
  double d = v.length();
  // psi is the angle of line from start point to end point
  this->psi_ = std::atan2(v.y(), v.x());
  if (this->psi_ < 0)
    this->psi_ += vnl_math::pi * 2;

  //degeneracy check
  if (d < bvgl_eulerspiral_e_error){
    this->set_intrinsic_params(0, 0, 0);
    return false;
  }

  double k0_init_est = 0;
  double len_init_est = 1;
  // double dstep = 0.1;
  double dstep_k0 = 0.1;
  double dstep_len = 0.1;
  bool k0_len_initialized = false;

  // initialize values of k0 and len for optimization
  // check if the lookup table is available, if yes then use it, otherwise initialize using biarc
  if ( use_lookup_table ){
    if (bvgl_eulerspiral_lookup_table::instance()->has_table()){
      double k0_est, gamma_est, len_est, turning_angle_est;
      double k0_max_error, gamma_max_error, len_max_error;
      bvgl_eulerspiral_lookup_table::instance()->look_up(this->start_angle()-this->psi_,
        this->end_angle()-this->psi_, &k0_est, &gamma_est, &len_est,
        &k0_max_error, &gamma_max_error, &len_max_error );

      // check if value from lookup table is valid
      if (! vnl_math::isfinite(len_est) ){
        this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
        return false;
      }

      // computing turning angle based on estimated turning angle
      // the correct turning angle should be close to the estimated turning angle
      turning_angle_est = k0_est*len_est + 0.5*gamma_est*len_est*len_est;
      int num_offset_cycle;
      num_offset_cycle = vnl_math::rnd((this->end_angle()-this->start_angle() - turning_angle_est)/(  vnl_math::pi*2) );
      this->turning_angle_ = (this->end_angle()-this->start_angle()) - num_offset_cycle*vnl_math::pi*2;
      k0_init_est = k0_est;
      len_init_est = len_est;
 //     dstep = bvgl_eulerspiral_lookup_table::instance()->dt() ;
      dstep_k0 = k0_max_error;
      dstep_len = len_max_error;

      k0_len_initialized = true;
    }
  }

  // if not using look-up table or lookup table is not available then initialize using biarc
  if ( !k0_len_initialized ){
    // initialize k0 and len using biarc
    // first compute a biarc estimate
    bvgl_biarc biarc_estimate;

    // check if a biarc can be computed. If yes then initialize using the biarc and move on
    // if no, then there is no euler spiral either
    if ( ! biarc_estimate.compute_biarc_params(this->start(), this->start_angle(),
      this->end(), this->end_angle()) )
    {
        this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
        return false;
    }
    // get the total turning angle::This is an important parameter because
    // it defines the one solution out of many possible solutions
    this->turning_angle_ = biarc_estimate.k1()*biarc_estimate.len1() +
      biarc_estimate.k2()*biarc_estimate.len2();

    //From here on, normlize the parameters and use these to perform the optimization
    k0_init_est = biarc_estimate.k1() * d;
    len_init_est = biarc_estimate.len() / d;
    // dstep = 0.1;
    dstep_k0 = 0.1;
    dstep_len = 0.1;
  }

  // perform a simple gradient descent to find the real solution
  double error = this->compute_error(k0_init_est, len_init_est);
  double prev_error = error;

  double k0 = k0_init_est;
  double len = len_init_est;

  double e1, e2, e3, e4;

  int i = 0;
  for (i=0; i < bvgl_eulerspiral_max_gradient_descent_iter; i++){
    if ( error < bvgl_eulerspiral_e_error)
        break;

    e1 = this->compute_error(k0 + dstep_k0, len);
    e2 = this->compute_error(k0 - dstep_k0, len);
    e3 = this->compute_error(k0, len + dstep_len);
  //  if (len > dstep)
    if (len > dstep_len)
      e4 = this->compute_error(k0, len - dstep_len);

    error = std::min(std::min(e1,e2), std::min(e3,e4));

    if (error > prev_error){
       // dstep = dstep/2;
      dstep_k0 = dstep_k0/2;
      dstep_len = dstep_len/2;
       continue;
    }

    if (error == e1)
      //k0 = k0 + dstep;
      k0 = k0 + dstep_k0;
    else if (error == e2)
      //k0 = k0 - dstep;
      k0 = k0 - dstep_k0;
    else if (error == e3)
      //len = len + dstep;
      len = len + dstep_len;
    else if (error==e4)
      //len = len - dstep;
      len = len - dstep_len;

    prev_error = error;
  }

  this->num_iterations_ = i;
  if ( error > bvgl_eulerspiral_e_error){
    this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
    return false;
  }

  //store the parameters
  this->set_k0(k0/d);
  this->set_len(len*d);
  this->set_gamma(2*(this->turning_angle_ - k0*len)/(len*len)/(d*d));
  this->error_ = error;
  return true;
}


bool bvgl_eulerspiral::
compute_es_params_use_levenberg_marquardt(bool use_lookup_table ){

  this->num_iterations_ = 0;
  this->num_evaluations_ = 0;

  //compute scaling distance
  vgl_vector_2d< double > v = this->end() - this->start();
  double d = v.length();

  //degeneracy check
  if (d < bvgl_eulerspiral_e_error){
    this->set_intrinsic_params(0, 0, 0);
    return false;
  }

  // psi is the angle of line from start point to end point
  this->psi_ = std::atan2(v.y(), v.x());
  if (this->psi_ < 0)
    this->psi_ += vnl_math::pi * 2;

  vnl_vector< double > k0_len_estimate(2);
  bool k0_len_initialized = false;

  // compute turning_angle and initialize k0 and len for optimization

  // check if the lookup table is available, if yes then use it, otherwise initialize using biarc
  if ( use_lookup_table ){
    if (bvgl_eulerspiral_lookup_table::instance()->has_table()){
      double k0_est, gamma_est, len_est, turning_angle_est;
      double k0_max_error, gamma_max_error, len_max_error;
      bvgl_eulerspiral_lookup_table::instance()->look_up(this->start_angle()-this->psi_,
        this->end_angle()-this->psi_, &k0_est, &gamma_est, &len_est,
        &k0_max_error, &gamma_max_error, &len_max_error );

      // check if value from lookup table is valid
      if (! vnl_math::isfinite(len_est) ){
        this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
        return false;
      }

      // computing turning angle based on estimated turning angle
      // the correct turning angle should be close to the estimated turning angle
      turning_angle_est = k0_est*len_est + 0.5*gamma_est*len_est*len_est;
      int num_offset_cycle;
      num_offset_cycle = vnl_math::rnd((this->end_angle()-this->start_angle() - turning_angle_est)/(  vnl_math::pi*2) );
      this->turning_angle_ = (this->end_angle()-this->start_angle())-num_offset_cycle*vnl_math::pi*2;
      // initialized k0 and len with values from lookup table
      k0_len_estimate.put(0, k0_est);
      k0_len_estimate.put(1, len_est);
      k0_len_initialized = true;
    }
  }

  // if not using look-up table or lookup table is not available then initialize using biarc
  if ( !k0_len_initialized ){
    // initialize k0 and len using biarc
    // first compute a biarc estimate
    bvgl_biarc biarc_estimate;

    // check if a biarc can be computed. If yes then initialize using the biarc and move on
    // if no, then there is no euler spiral either
    if ( ! biarc_estimate.compute_biarc_params(this->start(), this->start_angle(),
      this->end(), this->end_angle()) )
    {
        this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
        return false;
    }
    // get the total turning angle::This is an important parameter because
    // it defines the one solution out of many possible solutions
    this->turning_angle_ = biarc_estimate.k1()*biarc_estimate.len1() +
      biarc_estimate.k2()*biarc_estimate.len2();

    //From here on, normlize the parameters and use these to perform the optimization
    k0_len_estimate.put(0, biarc_estimate.k1()*d);
    k0_len_estimate.put(1, biarc_estimate.len()/d);
  }


  //: construct the optimization function
  bvgl_eulerspiral_optimization_function es_optim_funct(2, 2, vnl_least_squares_function::no_gradient);
  es_optim_funct.set_es(this);

  // initialize root vector
  vnl_vector< double > root_vector = k0_len_estimate;

  // solve for root using nonlinear minimization, vnl_levenberg_marquardt algorithm
  vnl_levenberg_marquardt lm(es_optim_funct);

  if (! lm.minimize(root_vector)){
    this->set_intrinsic_params(0, 0, vnl_huge_val((double)1.0));
    return false;
  }

  double k0 = root_vector.get(0);
  double len = root_vector.get(1);

  //store the parameters
  this->set_k0(k0/d);
  this->set_len(len*d);
  this->set_gamma(2*(this->turning_angle_ - k0*len)/(len*len)/(d*d));

  // other optimization parameters
  this->error_ = std::sqrt(lm.get_end_error());
  this->num_iterations_ = lm.get_num_iterations();
  return true;
}


// /////////////////////////////////////////////////////////////////////////////
//
// ------------ bvgl_eulerspiral_lookup_table functions ----------------------//
//

#ifdef BVGL_WHERE_BRL_LIB_DIR_H_EXISTS
  #include <bvgl_where_brl_lib_dir.h>
  const std::string bvgl_eulerspiral_lookup_table::file_path = std::string(BRL_LIB_DIR);
#else
  const std::string bvgl_eulerspiral_lookup_table::file_path = std::string();
#endif

const std::string bvgl_eulerspiral_lookup_table::
file_name = std::string("bvgl_eulerspiral_lookup_table.bvl");

//: static bvgl_eulerspiral_lookup_table instance
bvgl_eulerspiral_lookup_table* bvgl_eulerspiral_lookup_table::instance_ = nullptr;

//: Return the pointer to the only instance of the class
bvgl_eulerspiral_lookup_table *bvgl_eulerspiral_lookup_table::instance(){
  if (! instance_){
    instance_ = new bvgl_eulerspiral_lookup_table();
  }
  return bvgl_eulerspiral_lookup_table::instance_;
}

//: Constructor
bvgl_eulerspiral_lookup_table::bvgl_eulerspiral_lookup_table(){
  std::string full_path = bvgl_eulerspiral_lookup_table::file_path + std::string("/") +
    bvgl_eulerspiral_lookup_table::file_name;
  vsl_b_ifstream in_stream(full_path);
  // vsl_b_ifstream in_stream(bvgl_eulerspiral_lookup_table::file_path +
  //   bvgl_eulerspiral_lookup_table::file_path);
  // check if data file is available
  if (!in_stream){
    this->has_table_ = false;
    in_stream.close();
    std::cerr << "The data file used to speed-up Euler Spiral computation " <<
      bvgl_eulerspiral_lookup_table::file_name
      << " is corrupted or missing." << std::endl;
    return;
  }

  this->has_table_ = true;
  //read in the tables from data files
  vsl_b_read(in_stream, this->npts_);

  // k0
  for (int i = 0; i < this->npts_; i++ ){
    std::vector< double > k0_row;
    vsl_b_read(in_stream, k0_row);
    if (! in_stream ){
      std::cerr << "Reading data file failed. " << std::endl;
      return;
    }
    this->k0_table_.push_back(k0_row);
  }

  // gamma
  for (int i = 0; i < this->npts_; i++ ){
    std::vector< double > gamma_row;
    vsl_b_read(in_stream, gamma_row);
    if (! in_stream ){
      std::cerr << "Reading data file failed. " << std::endl;
      return;
    }
    this->gamma_table_.push_back(gamma_row);
  }

  // len
  for (int i = 0; i < this->npts_; i++ ){
    std::vector< double > len_row;
    vsl_b_read(in_stream, len_row);
    if (! in_stream ){
      std::cerr << "Reading data file failed. " << std::endl;
      return;
    }
    this->len_table_.push_back(len_row);
  }

  in_stream.close();

  //compute the dtt
  this->dt_ = 2* vnl_math::pi/npts_;
}

//: Destructor
bvgl_eulerspiral_lookup_table::~bvgl_eulerspiral_lookup_table()= default;

//: delta theta values for the table (tells you about the accuracy of the lookup)
double bvgl_eulerspiral_lookup_table::dt(){
  return dt_;
}

//: look up values of k0, gamma, and len of a normalized euler spiral given
// starting and ending angle. Perform bilear interpolation when in between sampled points
void bvgl_eulerspiral_lookup_table::
look_up(double start_angle, double end_angle, double* k0, double* gamma, double* len,
        double *k0_max_error, double *gamma_max_error, double* len_max_error){
  int start_index_low, start_index_high;
  int end_index_low, end_index_high;
  double a, b;

  // convert to range [0, 2pi)
  start_angle = std::fmod(start_angle, 2*vnl_math::pi);
  start_angle = (start_angle < 0) ? start_angle+2*vnl_math::pi : start_angle;
  end_angle = std::fmod(end_angle, 2*vnl_math::pi);
  end_angle = (end_angle < 0) ? end_angle+2*vnl_math::pi : end_angle;

  // find high and low index for each
  // start_angle
  start_index_low = (int)std::floor(start_angle / this->dt_);
  start_index_high = (int)std::ceil(start_angle / this->dt_);
  start_index_high = (start_index_high >= this->npts_) ? 0 : start_index_high;
  a = (start_angle - start_index_low * this->dt_)/this->dt_;


  // end_angle
  end_index_low = (int)std::floor(end_angle / this->dt());
  end_index_high = (int)std::ceil(end_angle / this->dt());
  end_index_high = (end_index_high >= this->npts_) ? 0 : end_index_high;
  b = (end_angle - end_index_low*this->dt_)/this->dt_;

  // bilear interpolation of values of k0, gamma, and len
  double x11 = 0;
  double x12 = 0;
  double x21 = 0;
  double x22 = 0;
  // x11 ==a==-----------x21
  //  |                   |
  // b|                   |b
  //  |                   |
  //  +       x           +
  // x12 ==a==-----------x22

  x11 = this->k0_table_.at(start_index_low).at(end_index_low);
  x21 = this->k0_table_.at(start_index_high).at(end_index_low);
  x12 = this->k0_table_.at(start_index_low).at(end_index_high);
  x22 = this->k0_table_.at(start_index_high).at(end_index_high);
  *k0 = (1-a)*(1-b)*x11 + a*(1-b)*x21 + (1-a)*b*x12 + a*b*x22;
  *k0_max_error = std::max(
    std::max((double)x11, (double)x21),
    std::max((double)x12, (double)x22)
    ) - std::min(
    std::min((double)x11, (double)x21),
    std::min((double)x12,(double)x22)
    );

  x11 = this->gamma_table_.at(start_index_low).at(end_index_low);
  x21 = this->gamma_table_.at(start_index_high).at(end_index_low);
  x12 = this->gamma_table_.at(start_index_low).at(end_index_high);
  x22 = this->gamma_table_.at(start_index_high).at(end_index_high);
  *gamma = (1-a)*(1-b)*x11 + a*(1-b)*x21 + (1-a)*b*x12 + a*b*x22;
  *gamma_max_error = std::max(
    std::max((double)x11, (double)x21),
    std::max((double)x12, (double)x22)
    ) - std::min(
    std::min((double)x11, (double)x21),
    std::min((double)x12,(double)x22)
    );

  x11 = this->len_table_.at(start_index_low).at(end_index_low);
  x21 = this->len_table_.at(start_index_high).at(end_index_low);
  x12 = this->len_table_.at(start_index_low).at(end_index_high);
  x22 = this->len_table_.at(start_index_high).at(end_index_high);
  *len = (1-a)*(1-b)*x11 + a*(1-b)*x21 + (1-a)*b*x12 + a*b*x22;
  *len_max_error = std::max(
    std::max((double)x11, (double)x21),
    std::max((double)x12, (double)x22)
    ) - std::min(
    std::min((double)x11, (double)x21),
    std::min((double)x12,(double)x22)
    );

  return;
};


// ===================== bvgl_eulerspiral_optimization_function class ===========
//: constructor - the same as base class
bvgl_eulerspiral_optimization_function::
bvgl_eulerspiral_optimization_function(unsigned int number_of_unknowns,
                                       unsigned int number_of_residuals,
                                       UseGradient g):
vnl_least_squares_function(number_of_unknowns, number_of_residuals, g)
{
  this->es_ = nullptr;
}

//: Main function for optimization
// Assume es_.psi and es_.turning_angle has been computed
// Assume a normalized Euler Spiral
void bvgl_eulerspiral_optimization_function::
f(vnl_vector<double> const& x, vnl_vector<double>& fx){
  // extract k0 and len values
  double k0 = x.get(0);
  double len = x.get(1);
  // assumes normalized parameters
  // compute the endpoint of the Euler spiral with the given intrinsic parameters
  double gamma = 2*(this->es()->turning_angle_ - k0 * len) / (len * len);
  vgl_point_2d< double > cur_end_pt = this->es()->compute_end_pt(k0, gamma, len, true);
  // return vector
  fx.put(0, cur_end_pt.x() - 1.0);
  fx.put(1, cur_end_pt.y());
  return;
}
