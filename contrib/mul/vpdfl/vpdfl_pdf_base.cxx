#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Base class for Multi-Variate Probability Density Function classes.

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vpdfl/vpdfl_pdf_base.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================

vpdfl_pdf_base::vpdfl_pdf_base()
{
}

//=======================================================================
// Destructor
//=======================================================================

vpdfl_pdf_base::~vpdfl_pdf_base()
{
}

//=======================================================================
//: Probability density at x
//=======================================================================
double vpdfl_pdf_base::operator()(const vnl_vector<double>& x) const
{
  return exp(log_p(x));
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_pdf_base::version_no() const
{
  return 1;
}

//=======================================================================
// Method: vxl_add_to_binary_loader
//=======================================================================

void vsl_add_to_binary_loader(const vpdfl_pdf_base& b)
{
  vsl_binary_loader<vpdfl_pdf_base>::instance().add(b);
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string vpdfl_pdf_base::is_a() const
{
  return vcl_string("vpdfl_pdf_base");

}

//=======================================================================
// Method: print
//=======================================================================

  // required if data is present in this base class
void vpdfl_pdf_base::print_summary(vcl_ostream& os) const
{
  os << vsl_indent() << "N. Dims : "<< mean_.size();
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this base class
void vpdfl_pdf_base::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, mean_);
  vsl_b_write(bfs, var_);
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this base class
void vpdfl_pdf_base::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,var_);
      break;
    default:
      vcl_cerr << "vpdfl_pdf_base::b_read() ";
      vcl_cerr << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
  }
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_pdf_base* b)
{
  if (b)
  {
    vsl_b_write(bfs, b->is_a());
    b->b_write(bfs);
  }
  else
    vsl_b_write(bfs, vcl_string("VSL_NULL_PTR"));
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vpdfl_pdf_base& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vpdfl_pdf_base& b)
{
    b.b_read(bfs);
}


void vsl_print_summary(vcl_ostream& os,const vpdfl_pdf_base& b)
{
  os << b.is_a() << ": ";
  vsl_inc_indent(os);
  b.print_summary(os);
  vsl_dec_indent(os);
}


void vsl_print_summary(vcl_ostream& os,const vpdfl_pdf_base* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No vpdfl_pdf_base defined.";
}

