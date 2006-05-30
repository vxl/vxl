// This is mul/vpdfl/vpdfl_gaussian_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Multi-variate gaussian PDF with arbitrary axes.
// \author Tim Cootes
// \date 16-Oct-98
//
// Modifications
// \verbatim
//    IMS   Converted to VXL 18 April 2000
// \endverbatim

//=======================================================================
// inclusions
//=======================================================================

#include "vpdfl_gaussian_builder.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <mbl/mbl_data_wrapper.h>
#include <vpdfl/vpdfl_gaussian.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>

// Weights smaller than this are assumed to be zero
const double min_wt = 1e-8;

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_gaussian_builder::vpdfl_gaussian_builder()
  : min_var_(1.0e-6)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_gaussian_builder::~vpdfl_gaussian_builder()
{
}

//=======================================================================

vpdfl_gaussian& vpdfl_gaussian_builder::gaussian(vpdfl_pdf_base& model) const
{
    // need a vpdfl_gaussian
  assert(model.is_class("vpdfl_gaussian"));
  return static_cast<vpdfl_gaussian&>(model);
}
//=======================================================================

vpdfl_pdf_base* vpdfl_gaussian_builder::new_model() const
{
  return new vpdfl_gaussian;
}

//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void vpdfl_gaussian_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double vpdfl_gaussian_builder::min_var() const
{
  return min_var_;
}

//=======================================================================

void vpdfl_gaussian_builder::build(vpdfl_pdf_base& model,
                                   const vnl_vector<double>& mean) const
{
  vpdfl_gaussian& g = gaussian(model);
  int n = mean.size();

  vnl_vector<double> var(n);
  for (int i=0;i<n;i++) var(i)=min_var_;

  // Generate an identity matrix for eigenvectors
  vnl_matrix<double> P(n,n);
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j) P(i,j) = 0.0;

  for (int i=0;i<n;++i) P(i,i) = 1.0;

  g.set(mean,var,P,var);
}
//=======================================================================

void vpdfl_gaussian_builder::updateCovar(vnl_matrix<double>& S,
                                         const vnl_vector<double>& vec,
                                         double w) const
{
  unsigned int n = vec.size();
  const double *v = vec.data_block();
  if (S.rows()!=n)
  {
    S.set_size(n,n);
    double **S_data = S.data_array();
    for (unsigned int i=0; i<n; ++i)
      for (unsigned int j=0; j<n; ++j)
        S_data[j][i] = w*v[i]*v[j];
  }
  else
  {
    double **S_data = S.data_array();
    double * S_row;
    for (unsigned int i=0; i<n; ++i)
    {
      S_row = S_data[i];
      double vw = w*v[i];
      for (unsigned int j=0; j<n; ++j)
        S_row[j] += vw*v[j];
    }
  }
}
  //=======================================================================

    //: Build model from mean and covariance
void vpdfl_gaussian_builder::buildFromCovar(vpdfl_gaussian& g,
                                            const vnl_vector<double>& mean,
                                            const vnl_matrix<double>& S) const
{
  unsigned int n = mean.size();
  vnl_matrix<double> evecs(S.rows(), S.rows());
  vnl_vector<double> evals(S.rows());


  vnl_symmetric_eigensystem_compute(S, evecs, evals);
  // eigenvalues are lowest first here
  evals.flip();
  evecs.fliplr();
  // eigenvalues are highest first now

  // Apply threshold to variance
  double *ev = evals.data_block();
  for (unsigned int i=0;i<n;++i)
    if (ev[i]<min_var_) ev[i]=min_var_;

  g.set(mean,evecs,evals);
}

//=======================================================================

void vpdfl_gaussian_builder::build(vpdfl_pdf_base& model,
                                   mbl_data_wrapper<vnl_vector<double> >& data) const
{
  vpdfl_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();

  assert(n_samples >= 2L); // Not enough examples available

  vnl_vector<double> mean;
  vnl_matrix<double> S;

  meanCovar(mean,S,data);
  buildFromCovar(g,mean,S);
}
//=======================================================================

//: Computes mean and covariance of given data
void vpdfl_gaussian_builder::meanCovar(vnl_vector<double>& mean, vnl_matrix<double>& S,
                                       mbl_data_wrapper<vnl_vector<double> >& data) const
{
  unsigned long n_samples = data.size();

  assert(n_samples!=0L);

  data.reset();
  int n_dims = data.current().size();
  vnl_vector<double> sum(n_dims);
  sum.fill(0);

  S.set_size(0,0);

  data.reset();
  for (unsigned long i=0;i<n_samples;i++)
  {
    sum += data.current();
    updateCovar(S,data.current(),1.0);

    data.next();
  }

  mean = sum / (double) n_samples;
  updateCovar(S, mean, - (double)n_samples);
  S/=(n_samples-1);
}

//=======================================================================

void vpdfl_gaussian_builder::weighted_build(vpdfl_pdf_base& model,
                                            mbl_data_wrapper<vnl_vector<double> >& data,
                                            const vcl_vector<double>& wts) const
{
  vpdfl_gaussian& g = gaussian(model);

  unsigned long n_samples = data.size();

  assert(n_samples>=2L); // Need enough samples

  data.reset();
  int n_dims = data.current().size();
  vnl_vector<double> sum(n_dims);
  sum.fill(0);
  vnl_matrix<double> S;
  double w_sum = 0.0;
  unsigned actual_samples = 0;

  data.reset();
  for (unsigned long i=0;i<n_samples;i++)
  {
    double w = wts[i];
    if (w >= min_wt) actual_samples ++;
    w_sum += w;
    sum += w*data.current();
    updateCovar(S,data.current(),w);

    data.next();
  }

  if (w_sum/n_samples<min_wt)  // ie near zero
  {
    vcl_cerr<<"vpdfl_gaussian_builder::weighted_build() :\n"
            <<"Weights too close to zero. Sum = "<<w_sum<<vcl_endl;
    vcl_abort();
  }

  if (actual_samples==0)
  {
    vcl_cerr<<"vpdfl_gaussian_builder::weighted_build() :\nAll weights zero.\n";
    vcl_abort();
  }

  if (actual_samples==1)
  {
#if 0
    vcl_cerr<<"vpdfl_gaussian_builder::weighted_build() :\n"
            <<" Warning: Only one sample has non-zero weight.\n";
#endif
    // Build minimal model about the mean (the one non-zero sample)
    sum/=w_sum;
    build(g,sum);
    return;
  }

  S*=actual_samples/((actual_samples - 1) *w_sum);
  sum/=w_sum;
  updateCovar(S, sum, -1.0);  // Remove mean.mean'
  // now sum = weighted mean
  // and S = weighted covariance corrected for unbiased rather than ML result.

  buildFromCovar(g,sum,S);
}
//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_gaussian_builder::is_a() const
{
  static vcl_string class_name_ = "vpdfl_gaussian_builder";
  return class_name_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_gaussian_builder::is_class(vcl_string const& s) const
{
  return vpdfl_builder_base::is_class(s) || s==vpdfl_gaussian_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_gaussian_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_builder_base* vpdfl_gaussian_builder::clone() const
{
  return new vpdfl_gaussian_builder(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_gaussian_builder::print_summary(vcl_ostream& os) const
{
  os << "Min. var. : "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_gaussian_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_gaussian_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  vcl_string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian_builder &)\n"
             << "           Attempted to load object of type "
             << name <<" into object of type " << is_a() << vcl_endl;
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,min_var_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_gaussian_builder &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Read initialisation settings from a stream.
// Parameters:
// \verbatim
// {
//   min_var: 1.0e-6
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void vpdfl_gaussian_builder::config_from_stream(vcl_istream & is)
{
  vcl_string s = mbl_parse_block(is);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  double mv=1.0e-6;

  if (!props["min_var"].empty())
  {
    mv=vul_string_atof(props["min_var"]);
    props.erase("min_var");
  }
  set_min_var(mv);

  try
  {
    mbl_read_props_look_for_unused_props(
        "vpdfl_gaussian_builder::config_from_stream", props);
  }
  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }

}


//==================< end of vpdfl_gaussian_builder.cxx >====================
