#include <brct/brct_corr.h>
#include <vcl_iostream.h>

brct_corr::brct_corr()
{
  n_cams_ = 0;
  plane_ = 0;
  index_ = 0;
}

brct_corr::brct_corr(const int n_cams, const int plane, const int index)
{
  n_cams_ = n_cams;
  plane_ = plane;
  index_ = index;
  matches_.resize(n_cams_);
  for(int i = 0; i<n_cams_; i++)
    matches_[i]=vgl_homg_point_2d<double>(-1, -1);
}

brct_corr::~brct_corr()
{
}

void brct_corr::set_match(const int cam, const double x, const double y)
{
  matches_[cam] = vgl_homg_point_2d<double>(x, y);
}


//external functions
vcl_ostream& operator<<(vcl_ostream& s, brct_corr const& c)
{
  s << "NCAMS: " << c.n_cams_ << " P: "  << c.plane_ << " I: " << c.index_
    << "\n";
  for(int i = 0; i<c.n_cams_ ; i++)
    s <<  "X: " << c.matches_[i].x() << " Y: " << c.matches_[i].y() << "\n";
  return s;
}
