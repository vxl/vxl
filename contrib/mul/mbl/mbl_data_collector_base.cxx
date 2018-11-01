// This is mul/mbl/mbl_data_collector_base.cxx
//:
// \file
// \author Tim Cootes
// \brief Non-templated base class for mbl_data_collector_base

#include "mbl_data_collector_base.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================


mbl_data_collector_base::mbl_data_collector_base() = default;

//=======================================================================

mbl_data_collector_base::~mbl_data_collector_base() = default;

//=======================================================================

void vsl_add_to_binary_loader(const mbl_data_collector_base& b)
{
  vsl_binary_loader<mbl_data_collector_base>::instance().add(b);
}

//=======================================================================

std::string mbl_data_collector_base::is_a() const
{
  return std::string("mbl_data_collector_base");
}

//=======================================================================

bool mbl_data_collector_base::is_class(std::string const& s) const
{
  return s==mbl_data_collector_base::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector_base& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector_base& b)
{
  b.b_read(bfs);
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_data_collector_base& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_data_collector_base* b)
{
  if (b)
    return os << *b;
  else
    return os << "No mbl_data_collector_base defined.";
}
