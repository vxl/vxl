#include <brct/brct_corr.h>
#include <vcl_iostream.h>

brct_corr::brct_corr()
{
  n_cams_ = 0;
}

brct_corr::brct_corr(const int n_cams)
{
  n_cams_ = n_cams;
  matches_.resize(n_cams_);
  valid_.resize(n_cams_);
  for(int i = 0; i<n_cams_; i++)
    {
      matches_[i]=vgl_homg_point_2d<double>(-1, -1);
      valid_[i]=false;
    }
}
brct_corr::~brct_corr()
{
}

void brct_corr::set_match(const int cam, const double x, const double y)
{
  matches_[cam] = vgl_homg_point_2d<double>(x, y);
  valid_[cam]=true;
}


//external functions
vcl_ostream& operator<<(vcl_ostream& s, brct_corr const& c)
{
  s << "NCAMS: " << c.n_cams_ << "\n";
  for(int i = 0; i<c.n_cams_ ; i++)
    if(c.valid_[i])
      s <<  "CAM[" << i << "] X: " << c.matches_[i].x() << " Y: " << c.matches_[i].y() << "\n";
    else
      s << "CAM[" << i << "] INVALID\n";
  return s;
}
