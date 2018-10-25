// This is brl/bseg/bvpl/util/io/bvpl_bundler_features_2d_io.cxx
#include "bvpl_bundler_features_2d_io.h"

void vsl_b_read(vsl_b_istream &is, bvpl_bundler_features_2d& bundler_features)
{
  bundler_features.b_read(is);
  return;
}

void vsl_b_write(vsl_b_ostream &os, bvpl_bundler_features_2d const& bundler_features)
{
  bundler_features.b_write(os);
  return;
}

void vsl_b_read(vsl_b_istream &is, bvpl_bundler_features_2d* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    vsl_b_read(is,*p);
  }//end if not null
  else
    p = nullptr;

  return;
}

void vsl_b_write(vsl_b_ostream &os, const bvpl_bundler_features_2d* p)
{
  if (p == nullptr)
    vsl_b_write(os,false);//indicate null ptr
  else
  {
    vsl_b_write(os, true);
    vsl_b_write(os, *p);
  }
  return;
}

void vsl_print_summary(std::ostream& os, const bvpl_bundler_features_2d * /*p*/)
{
  os << "vsl_print_summary not yet implemented. " << std::flush;
  return;
}
