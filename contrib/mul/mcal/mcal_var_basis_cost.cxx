#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include "mcal_var_basis_cost.h"
//:
// \file
// \author Tim Cootes
// \brief Cost function to promote sparse basis vectors

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Constructors
//=======================================================================

mcal_var_basis_cost::mcal_var_basis_cost() = default;

//=======================================================================
// Destructor
//=======================================================================

mcal_var_basis_cost::~mcal_var_basis_cost() = default;

//: Returns true since cost can be computed from the variance.
bool mcal_var_basis_cost::can_use_variance() const
{
  return true;
}

  //: Compute component of the cost function from given basis vector
  // \param[in] unit_basis   Unit vector defining basis direction
  // \param[in] projections  Projections of the dataset onto this basis vector
double mcal_var_basis_cost::cost(const vnl_vector<double>& unit_basis,
                                 const vnl_vector<double>& projections)
{
  double var = projections.squared_magnitude()/projections.size();
  return cost_from_variance(unit_basis,var);
}

  //: Compute component of the cost function from given basis vector
  // Cost is log(variance)
double mcal_var_basis_cost::cost_from_variance(const vnl_vector<double>&  /*unit_basis*/,
                                               double variance)
{
  return std::log(1e-8+variance);
}


//=======================================================================
// Method: is_a
//=======================================================================

std::string  mcal_var_basis_cost::is_a() const
{
  return std::string("mcal_var_basis_cost");
}

//=======================================================================
// Method: version_no
//=======================================================================

short mcal_var_basis_cost::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

mcal_single_basis_cost* mcal_var_basis_cost::clone() const
{
  return new mcal_var_basis_cost(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mcal_var_basis_cost::print_summary(std::ostream& os) const
{
  os << is_a();
}

//=======================================================================
// Method: save
//=======================================================================

void mcal_var_basis_cost::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
}

//=======================================================================
// Method: load
//=======================================================================

void mcal_var_basis_cost::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      break;
    default:
      std::cerr << "mcal_var_basis_cost::b_read()\n"
               << "Unexpected version number " << version << std::endl;
      std::abort();
  }
}

//=======================================================================
//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void mcal_var_basis_cost::config_from_stream(std::istream & is)
{
  std::string s = mbl_parse_block(is);

  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  try
  {
    mbl_read_props_look_for_unused_props(
          "mcal_var_basis_cost::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}
