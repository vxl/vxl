#include "mcal_trivial_ca.h"
//:
// \file
// \author Tim Cootes
// \brief Generates set of axis aligned modes for non-zero elements

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_stats_nd.h>

//=======================================================================
// Constructors
//=======================================================================

mcal_trivial_ca::mcal_trivial_ca()
{
}


//=======================================================================
// Destructor
//=======================================================================

mcal_trivial_ca::~mcal_trivial_ca()
{
}


//: Compute modes of the supplied data relative to the supplied mean
//  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
//  mode_var[i] gives the variance of the data projected onto that mode.
void mcal_trivial_ca::build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                       const vnl_vector<double>& mean,
                                       vnl_matrix<double>& modes,
                                       vnl_vector<double>& mode_var)
{
  if (data.size()==0)
  {
    vcl_cerr<<"mcal_trivial_ca::build_about_mean() No samples supplied.\n";
    vcl_abort();
  }

  data.reset();

  if (data.current().size()==0)
  {
    vcl_cerr<<"mcal_trivial_ca::build_about_mean()\n"
            <<"Warning: Samples claim to have zero dimensions.\n"
            <<"Constructing empty model.\n";

    modes.set_size(0,0);
    mode_var.set_size(0);
    return;
  }

  mbl_stats_nd stats;
  data.reset();
  do
  {
    stats.obs(data.current()-mean);
  } while (data.next());

  // Count non-zero variance elements
  unsigned nc = 0;
  unsigned n = stats.sumSq().size();
  for (unsigned i=0;i<n;++i)
    if (stats.sumSq()[i]>0) nc++;

  modes.set_size(n,nc);
  modes.fill(0.0);
  mode_var.set_size(nc);
  unsigned k=0;
  for (unsigned i=0;i<n;++i)
  {
    if (stats.sumSq()[i]>0)
    {
      modes(i,k)=1.0;  // Create unit vector
      mode_var[k]=stats.sumSq()[i]/stats.n_obs();
      ++k;
    }
  }
}


//=======================================================================
// Method: is_a
//=======================================================================

vcl_string  mcal_trivial_ca::is_a() const
{
  return vcl_string("mcal_trivial_ca");
}

//=======================================================================
// Method: version_no
//=======================================================================

short mcal_trivial_ca::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

mcal_component_analyzer* mcal_trivial_ca::clone() const
{
  return new mcal_trivial_ca(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mcal_trivial_ca::print_summary(vcl_ostream& os) const
{
}

//=======================================================================
// Method: save
//=======================================================================

void mcal_trivial_ca::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
}

//=======================================================================
// Method: load
//=======================================================================

void mcal_trivial_ca::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      break;
    default:
      vcl_cerr << "mcal_trivial_ca::b_read()\n"
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
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
void mcal_trivial_ca::config_from_stream(vcl_istream & is)
{
  vcl_string s = mbl_parse_block(is);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  try
  {
    mbl_read_props_look_for_unused_props(
          "mcal_trivial_ca::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }
}
