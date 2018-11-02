#include <iostream>
#include "bwm_3d_corr.h"
//:
// \file
#include <vgl/vgl_distance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bwm_3d_corr::match(std::string const& site, vgl_point_3d<double> &pt) const
{
  std::map<std::string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();

  iter = matches_.find(site);
  if (iter != matches_.end()) {
    pt = iter->second;
    return true;
  }

  std::cerr << "Correspondence is not found for this site\n";
  return false;
}

//need to use a tolerance to check equality of points
static bool
point_equal(vgl_point_3d<double> const & a, vgl_point_3d<double> const & b)
{
  double tol = 0.001;
  double d = vgl_distance<double>(a, b);
  return d<tol;
}

bool bwm_3d_corr::update_match(std::string const& site, vgl_point_3d<double> old_pt, vgl_point_3d<double> new_pt)
{
  std::map<std::string , vgl_point_3d<double> >::iterator
    iter = matches_.find(site);

  if (iter != matches_.end()) {
    vgl_point_3d<double> pt(iter->second);
    if (point_equal(pt,old_pt)) {
      iter->second = vgl_point_3d<double> (new_pt);
      return true;
    }
    ++iter;
  }
  return false;
}

void bwm_3d_corr::set_match(std::string const& site, const double x, const double y, const double z)
{
  vgl_point_3d<double> pt(x, y, z);
  matches_[site] = pt;
}

//: Deletes the correspondence with respect to the given site
void bwm_3d_corr::erase(std::string const& site)
{
  matches_.erase(site);
}

//: return the list of sites being corresponded
std::vector<std::string> bwm_3d_corr::sites() const
{
  std::vector<std::string>  sites(0);
  std::map<std::string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
  while (iter != matches_.end()) {
    sites.push_back(iter->first);
    ++iter;
  }
  return sites;
}

//: test if the site exists in the set of correspondences and if so return the associated 3-d point
bool bwm_3d_corr::site_in(std::string const& site, vgl_point_3d<double> &corr) const
{
  std::map<std::string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
  while (iter != matches_.end()) {
    if (site == iter->first) {
      corr = iter->second;
      return true;
    }
    ++iter;
  }
  return false;
}


// return the set of correspondences as pairs
std::vector<std::pair<std::string, vgl_point_3d<double> > > bwm_3d_corr::match_list() const
{
  std::vector<std::pair<std::string, vgl_point_3d<double> > > mlist;
  std::map<std::string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
  while (iter != matches_.end()) {
   std::pair<std::string, vgl_point_3d<double> > pair;
   pair.first = iter->first;
   pair.second = vgl_point_3d<double> (iter->second.x(), iter->second.y(), iter->second.z());
   mlist.push_back(pair);
   ++iter;
 }
  return mlist;
}
// return the set of corresponded points over all sites
std::vector<vgl_point_3d<double> >  bwm_3d_corr::matching_pts() const
{
  std::vector<vgl_point_3d<double> > matches;
  std::map<std::string, vgl_point_3d<double> >::const_iterator iter =
    matches_.begin();
  while (iter != matches_.end()){
    matches.push_back(iter->second);
    ++iter;
  }
  return matches;
}

// output stream
std::ostream& operator<<(std::ostream& s, bwm_3d_corr const& c)
{
  std::vector<std::pair<std::string, vgl_point_3d<double> > > mlist = c.match_list();

  s << "Number of Sites: " << mlist.size() << '\n';
  std::vector<std::pair<std::string, vgl_point_3d<double> > >::const_iterator iter = mlist.begin();
  while (iter != mlist.end()) {
    s <<  "Site[ " << iter->first << " ]:( X: " << iter->second.x()
      << " Y: " << iter->second.y() << " Z: " << iter->second.z() << " )\n";
    ++iter;
  }

  return s;
}
