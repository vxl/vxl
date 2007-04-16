//:
// \file
// \author Tim Cootes
// \brief Cost function to promote sparse basis vectors

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vsl/vsl_indent.h>
#include <mcal/mcal_sparse_basis_cost.h>
#include <vcl_cmath.h>
// not used? #include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Constructors
//=======================================================================

mcal_sparse_basis_cost::mcal_sparse_basis_cost()
{
  alpha_ = 1.0;
}

//=======================================================================
// Destructor
//=======================================================================

mcal_sparse_basis_cost::~mcal_sparse_basis_cost()
{
}

//: Weighting for sparseness penalty
void mcal_sparse_basis_cost::set_alpha(double a)
{
  alpha_ = a;
}

//: Returns true since cost can be computed from the variance.
bool mcal_sparse_basis_cost::can_use_variance() const
{
  return true;
}

  //: Compute component of the cost function from given basis vector
  // \param: unit_basis - Unit vector defining basis direction
  // \param: projections - Projections of the dataset onto this basis vector
double mcal_sparse_basis_cost::cost(const vnl_vector<double>& unit_basis,
                      const vnl_vector<double>& projections)
{
  double var = projections.squared_magnitude()/projections.size();
  return cost_from_variance(unit_basis,var);
}

  //: Compute component of the cost function from given basis vector
  // Cost is log(variance) + alpha sum |e_i|.
  // \param: unit_basis - Unit vector defining basis direction
  // \param: variance - Variance of projections of the dataset onto this basis vector
double mcal_sparse_basis_cost::cost_from_variance(const vnl_vector<double>& unit_basis,
                                    double variance)
{
  return vcl_log(variance) + alpha_ * unit_basis.one_norm();
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string  mcal_sparse_basis_cost::is_a() const
{
  return vcl_string("mcal_sparse_basis_cost");
}

//=======================================================================
// Method: version_no
//=======================================================================

short mcal_sparse_basis_cost::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

mcal_single_basis_cost* mcal_sparse_basis_cost::clone() const
{
  return new mcal_sparse_basis_cost(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mcal_sparse_basis_cost::print_summary(vcl_ostream& os) const
{
  os<<vsl_indent()<<"alpha: "<<alpha_;
}

//=======================================================================
// Method: save
//=======================================================================

void mcal_sparse_basis_cost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,alpha_);
}

//=======================================================================
// Method: load
//=======================================================================

void mcal_sparse_basis_cost::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,alpha_);
      break;
    default:
      vcl_cerr << "mcal_sparse_basis_cost::b_read()\n"
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
  }
}

//=======================================================================
//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
//   min_modes: 0 max_modes: 99 prop_modes: 0.99
//   // Maximum number of doubles to store in memory at once
//   max_d_in_memory: 1e8
//   // Indicate how to build from large amounts of data
//   use_chunks: false
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void mcal_sparse_basis_cost::config_from_stream(vcl_istream & is)
{
  vcl_string s = mbl_parse_block(is);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  if (!props["alpha"].empty())
  {
    alpha_=vul_string_atof(props["alpha"]);
  }
  props.erase("alpha");

  try
  {
    mbl_read_props_look_for_unused_props(
          "mcal_sparse_basis_cost::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}
