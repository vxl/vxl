// This is mul/vpdfl/vpdfl_kernel_pdf_builder.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes
// \brief Initialises kernel pdfs

#include "vpdfl_kernel_pdf_builder.h"

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vpdfl/vpdfl_kernel_pdf.h>
#include <vnl/vnl_vector.h>
#include <vpdfl/vpdfl_calc_mean_var.h>
#if 0
#include <mbl/mbl_priority_bounded_queue.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>


//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_kernel_pdf_builder::vpdfl_kernel_pdf_builder()
    : min_var_(1.0e-6), build_type_(select_equal), fixed_width_(1.0)
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_kernel_pdf_builder::~vpdfl_kernel_pdf_builder()
{
}

//=======================================================================

vpdfl_kernel_pdf& vpdfl_kernel_pdf_builder::kernel_pdf(vpdfl_pdf_base& model) const
{
  // require a vpdfl_kernel_pdf
  assert(model.is_class("vpdfl_kernel_pdf"));
  return static_cast<vpdfl_kernel_pdf&>(model);
}

//: Use fixed width kernels of given width when building.
void vpdfl_kernel_pdf_builder::set_use_fixed_width(double width)
{
  build_type_ = fixed_width;
  fixed_width_ = width;
}

//: Use equal width kernels of width depending on number of samples.
void vpdfl_kernel_pdf_builder::set_use_equal_width()
{
  build_type_ = select_equal;
}

//: Kernel width proportional to distance to nearby samples.
void vpdfl_kernel_pdf_builder::set_use_width_from_separation()
{
  build_type_ = width_from_sep;
}

//: Build adaptive kernel estimate.
void vpdfl_kernel_pdf_builder::set_use_adaptive()
{
  build_type_ = adaptive;
}

//=======================================================================
//: Define lower threshold on variance for built models
//=======================================================================
void vpdfl_kernel_pdf_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
//=======================================================================
double vpdfl_kernel_pdf_builder::min_var() const
{
  return min_var_;
}

void vpdfl_kernel_pdf_builder::build(vpdfl_pdf_base& model,
                                     const vnl_vector<double>& mean) const
{
  vpdfl_kernel_pdf& kpdf = kernel_pdf(model);

  vcl_vector<vnl_vector<double> >m(1);
  m[0] = mean;
  kpdf.set_centres(&m[0],1,vcl_sqrt(min_var_));
}

//: Build kernel_pdf from n elements in data[i]
void vpdfl_kernel_pdf_builder::build_from_array(vpdfl_pdf_base& model,
                                                const vnl_vector<double>* data, int n) const
{
  vpdfl_kernel_pdf& kpdf = kernel_pdf(model);

  if (n<1)
  {
    vcl_cerr<<"vpdfl_kernel_pdf_builder::build() No examples available.\n";
    vcl_abort();
  }

  switch (build_type_)
  {
    case fixed_width:
    build_fixed_width(kpdf,data,n,fixed_width_);
    break;
    case select_equal:
    build_select_equal_width(kpdf,data,n);
    break;
    case width_from_sep:
    build_width_from_separation(kpdf,data,n);
    break;
    case adaptive:
    build_adaptive(kpdf,data,n);
    break;
    default:
    vcl_cerr<<"vpdfl_kernel_pdf_builder::build_from_array() Unknown build type.\n";
    vcl_abort();
  }
}

void vpdfl_kernel_pdf_builder::build(vpdfl_pdf_base& model, mbl_data_wrapper<vnl_vector<double> >& data) const
{
  /* vpdfl_kernel_pdf& kpdf = */ kernel_pdf(model);

  unsigned long n = data.size();

  if (n<1L)
  {
    vcl_cerr<<"vpdfl_kernel_pdf_builder::build() No examples available.\n";
    vcl_abort();
  }

  if (data.is_class("mbl_data_array_wrapper<T>"))
  {
    mbl_data_array_wrapper<vnl_vector<double> >& data_array =
                   static_cast<mbl_data_array_wrapper<vnl_vector<double> >&>( data);
    build_from_array(model,data_array.data(),n);
    return;
  }

  // Fill array with data
  vcl_vector<vnl_vector<double> >x(n);
  data.reset();
  for (unsigned long i=0;i<n;++i)
  {
    x[i]=data.current();
    data.next();
  }

  build_from_array(model,&x[0],n);
}

void vpdfl_kernel_pdf_builder::weighted_build(vpdfl_pdf_base& model,
                                            mbl_data_wrapper<vnl_vector<double> >& data,
                                            const vcl_vector<double>& /*wts*/) const
{
  vcl_cerr<<"vpdfl_kernel_pdf_builder::weighted_build() Ignoring weights.\n";
  build(model,data);
}

//: Build from n elements in data[i]
void vpdfl_kernel_pdf_builder::build_fixed_width(vpdfl_kernel_pdf& kpdf,
                             const vnl_vector<double>* data, int n, double width) const
{
  kpdf.set_centres(data,n,width);
}

//: Build from n elements in data[i].  Chooses width.
//  Same width selected for all points, using
//  $w=(4/(2n+d.n)^{1/(d+4)}\sigma$, as suggested by Silverman
//  Note: This value only suitable for gaussian kernels!
void vpdfl_kernel_pdf_builder::build_select_equal_width(vpdfl_kernel_pdf& kpdf,
                              const vnl_vector<double>* data, int n) const
{
  vnl_vector<double> m,var;
  vpdfl_calc_mean_var(m,var,data,n);

  double mean_var = var.mean();
  if (mean_var<min_var_) var=min_var_;

  double d = data[0].size();

  // See Silverman, p88-89 : This is suitable for Gaussian kernels
  double k_var = mean_var*vcl_pow(4.0/(n*(d+2)),2.0/(d+4));
  double w = vcl_sqrt(k_var);

  build_fixed_width(kpdf,data,n,w);
}

//: Kernel width proportional to distance to nearby samples.
void vpdfl_kernel_pdf_builder::build_width_from_separation(vpdfl_kernel_pdf& kpdf,
                               const vnl_vector<double>* data, int n) const
{
  vnl_vector<double> width(n);
  double* w=width.data_block();
  const double min_diff = 1e-6;

  //const unsigned k = 2;  // Second nearest neighbour
  for (int i=0;i<n;++i)
  {
#if 0
    mbl_priority_bounded_queue<double,vcl_vector<double>,vcl_less<double> > d_sq(k);
#endif

    // Number of repeats of the point
    // If resampling used, some points will be present several times
    int n_repeats=0;
    double min_d2= -1.0;
    for (int j=0;j<n;++j)
    {
      if (j!=i)
      {
        double d2 = vnl_vector_ssd(data[i],data[j]);

        if (d2<min_diff)
          n_repeats++;
        else
          if (d2<min_d2 || min_d2<0) min_d2=d2;
#if 0
        d_sq.push(d2);
#endif
      }
    }

    // Width set to distance to k-th nearest neighbour
#if 0
    w[i] = vcl_sqrt(d_sq.top());
#endif

    //: Width to nearest neighbour, allowing for repeats
    if (min_d2<min_var_) min_d2=min_var_;
    w[i] = vcl_sqrt(min_d2)/(n_repeats+1);
  }

  kpdf.set_centres(data,n,width);
}

//: Build adaptive kernel estimate.
//  Use equal widths to create a pilot estimate, then use the prob at each
//  data point to modify the widths
void vpdfl_kernel_pdf_builder::build_adaptive(vpdfl_kernel_pdf& kpdf,
                               const vnl_vector<double>* data, int n) const
{
  // First build the pilot estimate
  build_select_equal_width(kpdf,data,n);

  // Evaluate the pdf at each point
  vnl_vector<double> log_p(n);
  for (int i=0;i<n;++i)
  {
    log_p[i]=kpdf.log_p(data[i]);
  }

  double log_mean = log_p.mean();

  vnl_vector<double> new_width = kpdf.width();

  for (int i=0;i<n;++i)
  {
    // Scale each inversely by sqrt(prob)
    // Check: Should there be a power of d in there?
    new_width[i] *= vcl_exp(-0.5*(log_p[i]-log_mean));
  }

  kpdf.set_centres(data,n,new_width);
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_kernel_pdf_builder::is_a() const
{
  return vcl_string("vpdfl_kernel_pdf_builder");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool vpdfl_kernel_pdf_builder::is_class(vcl_string const& s) const
{
  return vpdfl_builder_base::is_class(s) || s==vpdfl_kernel_pdf_builder::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_kernel_pdf_builder::version_no() const
{
  return 1;
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_kernel_pdf_builder::print_summary(vcl_ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_kernel_pdf_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_kernel_pdf_builder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,min_var_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vpdfl_kernel_pdf_builder &)\n"
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
//   // kernel_widths can be fixed_width,select_equal,width_from_sep,adaptive
//   kernel_widths: fixed_width
//   // Width to be used when it is fixed_width
//   fixed_width: 1.0
// }
// \endverbatim
// \throw mbl_exception_parse_error if the parse fails.
void vpdfl_kernel_pdf_builder::config_from_stream(vcl_istream & is)
{
  vcl_string s = mbl_parse_block(is);

  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  double mv=1.0e-6;
  if (props.find("min_var")!=props.end())
  {
    mv=vul_string_atof(props["min_var"]);
    props.erase("min_var");
  }
  set_min_var(mv);

  build_type bt=select_equal;
  if (props.find("kernel_widths")!=props.end())
  {
    if (props["kernel_widths"]=="fixed_width") bt=fixed_width;
    else
    if (props["kernel_widths"]=="select_equal") bt=select_equal;
    else
    if (props["kernel_widths"]=="width_from_sep") bt=width_from_sep;
    else
    if (props["kernel_widths"]=="adaptive") bt=adaptive;
    else
    {
      vcl_string msg="Unknown kernel_width type : "+props["kernel_widths"];
      throw mbl_exception_parse_error(msg);
    }
    props.erase("kernel_widths");
  }
  build_type_ = bt;

  fixed_width_=1.0;
  if (props.find("fixed_width")!=props.end())
  {
    fixed_width_=vul_string_atof(props["fixed_width"]);
    props.erase("fixed_width");
  }

  try
  {
    mbl_read_props_look_for_unused_props(
        "vpdfl_kernel_pdf_builder::config_from_stream", props);
  }

  catch(mbl_exception_unused_props &e)
  {
    throw mbl_exception_parse_error(e.what());
  }

}

