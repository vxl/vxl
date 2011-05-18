// This is brl/bseg/bapl/io/bapl_io_dsift.cxx
#include "bapl_io_dsift.h"
#include <vcl_iostream.h>

void vsl_b_read(vsl_b_istream &is, bapl_dsift &dsift)
{
  dsift.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, bapl_dsift const& dsift)
{
  dsift.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, bapl_dsift* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bapl_dsift();
    vsl_b_read(is,*p);
  }
  else
    p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const bapl_dsift* p)
{
  if ( p == 0 )
    vsl_b_write(os,false); //indicate null pointer stored
  else
  {
    vsl_b_write(os, true);//indicate non-null pointer stored
    vsl_b_write(os, *p);
  }
}

void vsl_print_summary(vcl_ostream& os, const bapl_dsift *p)
{
  os << "vsl_print_summary not yet implemented. " << vcl_flush;
}
