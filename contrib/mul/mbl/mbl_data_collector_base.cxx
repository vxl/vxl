#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \author Tim Cootes
// \brief Non-templated base class for mbl_data_collector_base

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <mbl/mbl_data_collector_base.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================


mbl_data_collector_base::mbl_data_collector_base()
{
}

//=======================================================================
// Destructor
//=======================================================================


mbl_data_collector_base::~mbl_data_collector_base()
{
}

//=======================================================================
// Method: vsl_add_to_binary_loader
//=======================================================================


void vsl_add_to_binary_loader(const mbl_data_collector_base& b)
{
  vsl_binary_loader<mbl_data_collector_base>::instance().add(b);
}

//=======================================================================
// Method: is_a
//=======================================================================


vcl_string mbl_data_collector_base::is_a() const
{
  return vcl_string("mbl_data_collector_base");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool mbl_data_collector_base::is_class(vcl_string const& s) const
{
  static const vcl_string s_ = "mbl_data_collector_base";
  return s==s_;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================


void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector_base* b)
{
  if (b)
  {
    vsl_b_write(bfs,b->is_a());
    b->b_write(bfs);
  }
  else
    vsl_b_write(bfs,vcl_string("VSL_NULL_PTR"));
}

//=======================================================================
// Associated function: operator<<
//=======================================================================


void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector_base& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================


void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector_base& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================


vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector_base* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mbl_data_collector_base defined.";
}
