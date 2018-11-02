#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
#include "mcal_general_ca.h"
//:
// \file
// \author Tim Cootes
// \brief Class to perform general Component Analysis


#include <vsl/vsl_indent.h>
#include <mbl/mbl_matxvec.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>
#include <vnl/algo/vnl_brent_minimizer.h>

//=======================================================================
// Constructors
//=======================================================================

mcal_general_ca::mcal_general_ca()
{
  set_defaults();
}

//: Initialise, taking clones of supplied objects
void mcal_general_ca::set(const mcal_component_analyzer& initial_ca,
                          const mcal_single_basis_cost& basis_cost)
{
  initial_ca_ = initial_ca;
  basis_cost_ = basis_cost;
}

void mcal_general_ca::set_defaults()
{
  max_passes_ = 50;
  move_thresh_=1e-4;
}

//=======================================================================
// Destructor
//=======================================================================

mcal_general_ca::~mcal_general_ca() = default;

class mcal_pair_cost1 : public vnl_cost_function
{
 private:
  const vnl_vector<double>& proj1_;
  const vnl_vector<double>& proj2_;
  const vnl_vector<double>& mode1_;
  const vnl_vector<double>& mode2_;
  mcal_single_basis_cost& cost_;
  vnl_vector<double> p1,p2,m1,m2;
 public:
  mcal_pair_cost1(const vnl_vector<double>& proj1,
                  const vnl_vector<double>& proj2,
                  const vnl_vector<double>& mode1,
                  const vnl_vector<double>& mode2,
                  mcal_single_basis_cost& cost)
  : vnl_cost_function(1), proj1_(proj1),proj2_(proj2),mode1_(mode1),mode2_(mode2),cost_(cost) {}

  double f(const vnl_vector<double>& x) override;
};

double mcal_pair_cost1::f(const vnl_vector<double>& x)
{
  double sinA = std::sin(x[0]);
  double cosA = std::cos(x[0]);

  // Rotate axes by A=x[0]
  m1 = cosA*mode1_ + sinA*mode2_;
  m2 = cosA*mode2_ - sinA*mode1_;

  // Rotate projections equivalently
  p1 = cosA*proj1_ + sinA*proj2_;
  p2 = cosA*proj2_ - sinA*proj1_;

  double sum = cost_.cost(m1,p1) + cost_.cost(m2,p2);
  return sum;
}

//: Cost, assuming it can be evaluated from variance of projection
class mcal_pair_cost2 : public vnl_cost_function
{
 private:
  vnl_matrix<double> S_;
  const vnl_vector<double>& mode1_;
  const vnl_vector<double>& mode2_;
  mcal_single_basis_cost& cost_;
  vnl_vector<double> m1,m2;
 public:
  mcal_pair_cost2(const vnl_matrix<double>& S,
                  const vnl_vector<double>& mode1,
                  const vnl_vector<double>& mode2,
                  mcal_single_basis_cost& cost)
  : vnl_cost_function(1), S_(S),
    mode1_(mode1),mode2_(mode2),cost_(cost) {}

  mcal_pair_cost2(const vnl_vector<double>& proj1,
                  const vnl_vector<double>& proj2,
                  const vnl_vector<double>& mode1,
                  const vnl_vector<double>& mode2,
                  mcal_single_basis_cost& cost);

  double f(const vnl_vector<double>& x) override;

  void covar(const vnl_vector<double>& p1,
             const vnl_vector<double>& p2,
             vnl_matrix<double>& S);
};

mcal_pair_cost2::mcal_pair_cost2(const vnl_vector<double>& proj1,
                                 const vnl_vector<double>& proj2,
                                 const vnl_vector<double>& mode1,
                                 const vnl_vector<double>& mode2,
                                 mcal_single_basis_cost& cost)
  : vnl_cost_function(1),mode1_(mode1),mode2_(mode2),cost_(cost)
{
  covar(proj1,proj2,S_);
}


double mcal_pair_cost2::f(const vnl_vector<double>& x)
{
  double sinA = std::sin(x[0]);
  double cosA = std::cos(x[0]);

  // Rotate axes by A=x[0]
  m1 = cosA*mode1_ + sinA*mode2_;
  m2 = cosA*mode2_ - sinA*mode1_;

  // Rotate covariance equivalently
  vnl_matrix<double> R(2,2);
  R(0,0)=cosA;  R(0,1) = sinA;
  R(1,0)=-sinA; R(1,1) = cosA;

  vnl_matrix<double> SA = R*S_*R.transpose();

  double c1 = cost_.cost_from_variance(m1,SA(0,0));
  double c2 = cost_.cost_from_variance(m2,SA(1,1));
  return c1+c2;
}

void mcal_pair_cost2::covar(const vnl_vector<double>& p1,
                            const vnl_vector<double>& p2,
                            vnl_matrix<double>& S)
{
  S.set_size(2,2);
  S(0,0) = dot_product(p1,p1)/p1.size();
  S(1,1) = dot_product(p2,p2)/p1.size();
  S(0,1) = dot_product(p1,p2)/p1.size();
  S(1,0) = S(0,1);
}


//: Optimise the mode vectors so as to minimise the cost function
double mcal_general_ca::optimise_mode_pair(vnl_vector<double>& proj1,
                                           vnl_vector<double>& proj2,
                                           vnl_vector<double>& mode1,
                                           vnl_vector<double>& mode2)
{
  vnl_cost_function *cost_fn;
  if (basis_cost().can_use_variance())
  {
    // Use more efficient cost evaluation
    cost_fn = new mcal_pair_cost2(proj1,proj2,mode1,mode2,basis_cost());
  }
  else
  {
    // Use cost which explicitly rotates projection data
    cost_fn = new mcal_pair_cost1(proj1,proj2,mode1,mode2,basis_cost());
  }

  vnl_brent_minimizer brent1(*cost_fn);

  // Note that rotation should be in range [0,pi/2)
  // There is fourfold cyclic symmetry - cost(A)==cost(A+pi/2)
  // We could perform an initial exhaustive search, then use
  // A=minimize_given_bounds(a,b,c)
  double A = brent1.minimize(0.0);

  // Tidy up
  delete cost_fn;

  if (A==0.0) return 0.0;

  // Apply rotation
  double sinA = std::sin(A);
  double cosA = std::cos(A);

  vnl_vector<double> m1=mode1,m2=mode2;
  vnl_vector<double> p1=proj1,p2=proj2;

  // Rotate axes by A=x[0]
  mode1 = cosA*m1 + sinA*m2;
  mode2 = cosA*m2 - sinA*m1;

  // Rotate projections equivalently
  proj1 = cosA*p1 + sinA*p2;
  proj2 = cosA*p2 - sinA*p1;

  return std::fabs(A);
}

//: Optimise the mode vectors so as to minimise the cost function
double mcal_general_ca::optimise_one_pass(std::vector<vnl_vector<double> >& proj,
                                          vnl_matrix<double>& modes)
{
  unsigned n_modes = modes.cols();
  double move_sum=0.0;
  for (unsigned i=1;i<n_modes;++i)
  {
    vnl_vector<double> mode1 = modes.get_column(i);
    for (unsigned j=0;j<i;++j)
    {
      vnl_vector<double> mode2 = modes.get_column(j);
      move_sum += optimise_mode_pair(proj[i],proj[j],mode1,mode2);
      modes.set_column(j,mode2);
    }
    modes.set_column(i,mode1);
  }
  return move_sum;
}

//: Compute projections onto each mode
//  proj[j][i] is the projection of the i-th data sample onto the j-th mode
void mcal_general_ca::compute_projections(mbl_data_wrapper<vnl_vector<double> >& data,
                                          const vnl_vector<double>& mean,
                                          vnl_matrix<double>& modes,
                                          std::vector<vnl_vector<double> >& proj)
{
  // Compute projection of data onto each mode
  unsigned n_modes = modes.cols();
  unsigned n_egs   = data.size();
  proj.resize(n_modes);
  for (unsigned j=0;j<n_modes;++j) { proj[j].set_size(n_egs); }
  vnl_vector<double> b(n_modes);
  vnl_vector<double> dx;
  data.reset();
  for (unsigned i=0;i<n_egs;++i,data.next())
  {
    dx=data.current()-mean;
    mbl_matxvec_prod_vm(dx,modes,b);
    for (unsigned j=0;j<n_modes;++j) proj[j][i]=b[j];
  }
}

//: Optimise the mode vectors so as to minimise the cost function
void mcal_general_ca::optimise_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                          const vnl_vector<double>& mean,
                                          vnl_matrix<double>& modes,
                                          vnl_vector<double>& mode_var)
{
  // Compute projection of data onto each mode
  unsigned n_modes = mode_var.size();
  unsigned n_egs   = data.size();
  std::vector<vnl_vector<double> > proj(n_modes);
  compute_projections(data,mean,modes,proj);

  // Perform multiple passes
  for (unsigned i=0;i<max_passes_;++i)
  {
    if (optimise_one_pass(proj,modes)<move_thresh_) break;
  }

  // Compute the variances on each mode
  compute_projections(data,mean,modes,proj);
  mode_var.set_size(n_modes);
  for (unsigned j=0;j<n_modes;++j)
    mode_var[j]=proj[j].squared_magnitude()/n_egs;
}

//: Compute modes of the supplied data relative to the supplied mean
//  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
//  mode_var[i] gives the variance of the data projected onto that mode.
void mcal_general_ca::build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                       const vnl_vector<double>& mean,
                                       vnl_matrix<double>& modes,
                                       vnl_vector<double>& mode_var)
{
  if (data.size()==0)
  {
    std::cerr<<"mcal_general_ca::build_about_mean() No samples supplied.\n";
    std::abort();
  }

  data.reset();

  if (data.current().size()==0)
  {
    std::cerr<<"mcal_general_ca::build_about_mean()\n"
            <<"Warning: Samples claim to have zero dimensions.\n"
            <<"Constructing empty model.\n";

    modes.set_size(0,0);
    mode_var.set_size(0);
    return;
  }

  // Compute initial approximation
  initial_ca().build_about_mean(data,mean,modes,mode_var);

  // Now perform optimisation
  optimise_about_mean(data,mean,modes,mode_var);
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string  mcal_general_ca::is_a() const
{
  return std::string("mcal_general_ca");
}

//=======================================================================
// Method: version_no
//=======================================================================

short mcal_general_ca::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

mcal_component_analyzer* mcal_general_ca::clone() const
{
  return new mcal_general_ca(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mcal_general_ca::print_summary(std::ostream& os) const
{
  vsl_indent_inc(os);
  os<<"{\n"
    <<vsl_indent()<<"initial_ca: "<<initial_ca_<<'\n'
    <<vsl_indent()<<"basis_cost: "<<basis_cost_<<'\n'
    <<vsl_indent()<<"} ";
  vsl_indent_dec(os);
}

//=======================================================================
// Method: save
//=======================================================================

void mcal_general_ca::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,initial_ca_);
  vsl_b_write(bfs,basis_cost_);
  vsl_b_write(bfs,max_passes_);
  vsl_b_write(bfs,move_thresh_);
}

//=======================================================================
// Method: load
//=======================================================================

void mcal_general_ca::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,initial_ca_);
      vsl_b_read(bfs,basis_cost_);
      vsl_b_read(bfs,max_passes_);
      vsl_b_read(bfs,move_thresh_);
      break;
    default:
      std::cerr << "mcal_general_ca::b_read()\n"
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

//=======================================================================
//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
//   initial_ca: mcal_pca { ... }
//   basis_cost: mcal_sparse_basis_cost { alpha: 0.1 }
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void mcal_general_ca::config_from_stream(std::istream & is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  set_defaults();

  if (props.find("initial_ca")!=props.end())
  {
    std::istringstream ss(props["initial_ca"]);
    std::unique_ptr<mcal_component_analyzer> ca;
    ca=mcal_component_analyzer::create_from_stream(ss);
    initial_ca_ = *ca;

    props.erase("initial_ca");
  }

  if (props.find("basis_cost")!=props.end())
  {
    std::istringstream ss(props["basis_cost"]);
    std::unique_ptr<mcal_single_basis_cost> bc;
    bc=mcal_single_basis_cost::create_from_stream(ss);
    basis_cost_ = *bc;

    props.erase("basis_cost");
  }

  if (props.find("max_passes")!=props.end())
  {
    max_passes_ = vul_string_atoi(props["max_passes"]);
    props.erase("max_passes");
  }
  if (props.find("move_thresh")!=props.end())
  {
    move_thresh_ = vul_string_atoi(props["move_thresh"]);
    props.erase("move_thresh");
  }


  try
  {
    mbl_read_props_look_for_unused_props(
          "mcal_general_ca::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}
