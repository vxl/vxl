//:
// \file
#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vsl/vsl_indent.h>
#include <mcal/mcal_general_ca.h>
#include <mbl/mbl_matxvec.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>

//=======================================================================
// Constructors
//=======================================================================

mcal_general_ca::mcal_general_ca()
{
}

//=======================================================================
// Destructor
//=======================================================================

mcal_general_ca::~mcal_general_ca()
{
}

//: Optimise the mode vectors so as to minimise the cost function
double mcal_general_ca::optimise_mode_pair(vnl_vector<double>& proj1,
                                vnl_vector<double>& proj2,
                                vnl_vector<double>& mode1,
                                vnl_vector<double>& mode2)
{
  // Eventually do something more sophisticated than this exhaustive search
vcl_cerr<<"Search not implemented."<<vcl_endl;
  return 1.0;
}

//: Optimise the mode vectors so as to minimise the cost function
double mcal_general_ca::optimise_one_pass(vcl_vector<vnl_vector<double> >& proj,
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
                           vcl_vector<vnl_vector<double> >& proj)
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
  vcl_vector<vnl_vector<double> > proj(n_modes);
  compute_projections(data,mean,modes,proj);

  // Perform multiple passes
  unsigned max_passes = 5;
  double move_thresh=1e-6;  // To be supplied
  for (unsigned i=0;i<max_passes;++i)
  {
    if (optimise_one_pass(proj,modes)<move_thresh) break;
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
    vcl_cerr<<"mcal_general_ca::build_about_mean() No samples supplied.\n";
    vcl_abort();
  }

  data.reset();

  if (data.current().size()==0)
  {
    vcl_cerr<<"mcal_general_ca::build_about_mean()\n"
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

vcl_string  mcal_general_ca::is_a() const
{
  return vcl_string("mcal_general_ca");
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

void mcal_general_ca::print_summary(vcl_ostream& os) const
{
  vsl_indent_inc(os);
  os<<"{"<<vcl_endl
    <<vsl_indent()<<"initial_ca: "<<initial_ca_<<vcl_endl
    <<vsl_indent()<<"basis_cost: "<<basis_cost_<<vcl_endl
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
      break;
    default:
      vcl_cerr << "mcal_general_ca::b_read()\n"
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
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
void mcal_general_ca::config_from_stream(vcl_istream & is)
{
  vcl_string s = mbl_parse_block(is);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);


  if (!props["initial_ca"].empty())
  {
    vcl_istringstream ss(props["initial_ca"]);
    vcl_auto_ptr<mcal_component_analyzer> ca;
    ca=mcal_component_analyzer::create_from_stream(ss);
    initial_ca_ = *ca;

    props.erase("initial_ca");
  }

  if (!props["basis_cost"].empty())
  {
    vcl_istringstream ss(props["basis_cost"]);
    vcl_auto_ptr<mcal_single_basis_cost> bc;
    bc=mcal_single_basis_cost::create_from_stream(ss);
    basis_cost_ = *bc;

    props.erase("basis_cost");
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
