#include <brct/brct_plane_corr.h>
#include <vcl_iostream.h>

brct_plane_corr::brct_plane_corr()
{
  plane_ = 0;
  index_ = 0;
}

brct_plane_corr::brct_plane_corr(const int n_cams, const int plane, const int index) : brct_corr(n_cams)
{
  plane_ = plane;
  index_ = index;
}

brct_plane_corr::~brct_plane_corr()
{
}


//external functions
vcl_ostream& operator<<(vcl_ostream& s, brct_plane_corr const& pc)
{
  s << " Plane No. " << pc.plane_ << " Index in Plane " << pc.index_ << "\n";
  brct_corr const& c = (brct_corr)pc;
  s << c;
  return s;
}
