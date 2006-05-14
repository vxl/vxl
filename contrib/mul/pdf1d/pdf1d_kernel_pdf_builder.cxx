// This is mul/pdf1d/pdf1d_kernel_pdf_builder.cxx

//:
// \file

#include "pdf1d_kernel_pdf_builder.h"
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h> // vcl_abort()
#include <vcl_cmath.h>

#include <vnl/vnl_vector_ref.h>
#include <mbl/mbl_data_wrapper.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <mbl/mbl_index_sort.h>
#include <pdf1d/pdf1d_kernel_pdf.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

//=======================================================================

pdf1d_kernel_pdf_builder::pdf1d_kernel_pdf_builder()
    : min_var_(1.0e-6), build_type_(select_equal), fixed_width_(1.0)
{
}

//=======================================================================

pdf1d_kernel_pdf_builder::~pdf1d_kernel_pdf_builder()
{
}

//=======================================================================

pdf1d_kernel_pdf& pdf1d_kernel_pdf_builder::kernel_pdf(pdf1d_pdf& model) const
{
  // require a pdf1d_kernel_pdf
  assert(model.is_class("pdf1d_kernel_pdf"));
  return static_cast<pdf1d_kernel_pdf&>(model);
}

//: Use fixed width kernels of given width when building.
void pdf1d_kernel_pdf_builder::set_use_fixed_width(double width)
{
  build_type_ = fixed_width;
  fixed_width_ = width;
}

//: Use equal width kernels of width depending on number of samples.
void pdf1d_kernel_pdf_builder::set_use_equal_width()
{
  build_type_ = select_equal;
}

//: Kernel width proportional to distance to nearby samples.
void pdf1d_kernel_pdf_builder::set_use_width_from_separation()
{
  build_type_ = width_from_sep;
}

//: Build adaptive kernel estimate.
void pdf1d_kernel_pdf_builder::set_use_adaptive()
{
  build_type_ = adaptive;
}

//=======================================================================
//: Define lower threshold on variance for built models
void pdf1d_kernel_pdf_builder::set_min_var(double min_var)
{
  min_var_ = min_var;
}

//=======================================================================
//: Get lower threshold on variance for built models
double pdf1d_kernel_pdf_builder::min_var() const
{
  return min_var_;
}

void pdf1d_kernel_pdf_builder::build(pdf1d_pdf& model, double mean) const
{
  pdf1d_kernel_pdf& kpdf = kernel_pdf(model);

  vnl_vector<double> m(1);
  m[0] = mean;
  kpdf.set_centres(m,vcl_sqrt(min_var_));
}

//: Build kernel_pdf from n elements in data[i]
void pdf1d_kernel_pdf_builder::build_from_array(pdf1d_pdf& model,
                                                const double* data, int n) const
{
  pdf1d_kernel_pdf& kpdf = kernel_pdf(model);

  if (n<1)
  {
    vcl_cerr<<"pdf1d_kernel_pdf_builder::build() No examples available.\n";
    vcl_abort();
  }

  // Initially just use Silverman's estimate
  // Later allow switching of alternative algorithms
  // e.g. width proportional to dist. to nearest point,
  //    adaptive kernel estimates etc.

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
    vcl_cerr<<"pdf1d_kernel_pdf_builder::build() Unknown build type.\n";
    vcl_abort();
  }
}

void pdf1d_kernel_pdf_builder::build(pdf1d_pdf& model, mbl_data_wrapper<double>& data) const
{
  // pdf1d_kernel_pdf& kpdf = kernel_pdf(model); // unused

  int n = data.size();

  if (n<1)
  {
    vcl_cerr<<"pdf1d_kernel_pdf_builder::build() No examples available.\n";
    vcl_abort();
  }

  if (data.is_class("mbl_data_array_wrapper<T>"))
  {
    mbl_data_array_wrapper<double>& data_array =
                   static_cast<mbl_data_array_wrapper<double>&>(data);
    build_from_array(model,data_array.data(),n);
    return;
  }

  // Fill array with data
  vnl_vector<double> x(n);
  data.reset();
  for (int i=0;i<n;++i)
  {
    x[i]=data.current();
    data.next();
  }

  build_from_array(model,x.data_block(),n);
}

void pdf1d_kernel_pdf_builder::weighted_build(pdf1d_pdf& model,
                                              mbl_data_wrapper<double>& data,
                                              const vcl_vector<double>& /*wts*/) const
{
  vcl_cerr<<"pdf1d_kernel_pdf_builder::weighted_build() Ignoring weights.\n";
  build(model,data);
}

//: Build from n elements in data[i]
void pdf1d_kernel_pdf_builder::build_fixed_width(pdf1d_kernel_pdf& kpdf,
                                                 const double* data, int n, double width) const
{
  vnl_vector_ref<double> v_data(n,const_cast<double*>(data)); // const violation
  kpdf.set_centres(v_data,width);
}

//: Build from n elements in data[i].  Chooses width.
//  Same width selected for all points, using
//  $w=(3n/4)^{-0.2}\sigma$, as suggested by Silverman
void pdf1d_kernel_pdf_builder::build_select_equal_width(pdf1d_kernel_pdf& kpdf,
                                                        const double* data, int n) const
{
  double m,var;
  pdf1d_calc_mean_var(m,var,data,n);
  if (var<min_var_) var=min_var_;

  double k_var = var*vcl_pow(4.0/(3*n),0.4);
  double w = vcl_sqrt(k_var);

  build_fixed_width(kpdf,data,n,w);
}

//: Return distance to closest neighbour to i0, not identical to i0
//  Assumes index is sorted: data[index[i]] <= *data[index[i+1]]
static double dist_to_neighbour(int i0, const double* data, const int *index, int n)
{
  int k = 3;
  int ilo = i0-k; if (ilo<0) ilo=0;
  int ihi = i0+k; if (ihi>=n) ihi=n-1;

  return vcl_fabs(data[index[ihi]]-data[index[ilo]]);

#if 0
  double di0 = data[index[i0]];

  const double min_diff = 1.0e-6;
  // Look below i0
  int i=i0-1;
  while (i>0 && vcl_fabs(data[i]-di0)<min_diff) --i;
  double d_lo = vcl_fabs(data[i]-di0);

  // Look above i0
  i=i0+1;
  int n1 = n-1;
  while (i<n1 && vcl_fabs(data[i]-di0)<min_diff) ++i;
  double d_hi = vcl_fabs(data[i]-di0);

  return d_hi+d_lo;
#endif
}

//: Kernel width proportional to distance to nearby samples.
void pdf1d_kernel_pdf_builder::build_width_from_separation(pdf1d_kernel_pdf& kpdf,
                                                           const double* data, int n) const
{
  // Sort the data
  vcl_vector<int> index;
  mbl_index_sort(data, n, index);

  vnl_vector<double> width(n);
  double* w=width.data_block();

  double min_w = vcl_sqrt(min_var_)/n;

  for (int i=0;i<n;++i)
  {
    w[index[i]] = dist_to_neighbour(i, data, &index.front(), n);
    if (w[index[i]]<min_w) w[index[i]]=min_w;
  }

  kpdf.set_centres(vnl_vector_ref<double>(n, const_cast<double *>(data)), width);
}

//: Build adaptive kernel estimate.
//  Use equal widths to create a pilot estimate, then use the prob at each
//  data point to modify the widths
void pdf1d_kernel_pdf_builder::build_adaptive(pdf1d_kernel_pdf& kpdf,
                                              const double* data, int n) const
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
    new_width[i] *= vcl_exp(-0.5*(log_p[i]-log_mean));
  }

  kpdf.set_centres(kpdf.centre(),new_width);
}

//=======================================================================

vcl_string pdf1d_kernel_pdf_builder::is_a() const
{
  return vcl_string("pdf1d_kernel_pdf_builder");
}

//=======================================================================

bool pdf1d_kernel_pdf_builder::is_class(vcl_string const& s) const
{
  return pdf1d_builder::is_class(s) || s==pdf1d_kernel_pdf_builder::is_a();
}

//=======================================================================

short pdf1d_kernel_pdf_builder::version_no() const
{
  return 1;
}

//=======================================================================

void pdf1d_kernel_pdf_builder::print_summary(vcl_ostream& os) const
{
  os << "Min. var.: "<< min_var_;
}

//=======================================================================

void pdf1d_kernel_pdf_builder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,min_var_);
}

//=======================================================================

void pdf1d_kernel_pdf_builder::b_read(vsl_b_istream& bfs)
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
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_kernel_pdf_builder &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

