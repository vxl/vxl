#include "bwm_3d_corr.h"
//:
// \file
#include <vgl/vgl_distance.h>
#include <vcl_iostream.h>

bool bwm_3d_corr::match(vcl_string const& site, vgl_point_3d<double> &pt) const
{
  vcl_map<vcl_string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();

  iter = matches_.find(site);
  if (iter != matches_.end()) {
    pt = iter->second;
    return true;
  }

  vcl_cerr << "Correspondence is not found for this site\n";
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

bool bwm_3d_corr::update_match(vcl_string const& site, vgl_point_3d<double> old_pt, vgl_point_3d<double> new_pt)
{
  vcl_map<vcl_string , vgl_point_3d<double> >::iterator
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

void bwm_3d_corr::set_match(vcl_string const& site, const double x, const double y, const double z)
{
  vgl_point_3d<double> pt(x, y, z);
  matches_[site] = pt;
}

//: Deletes the correspondence with respect to the given site
void bwm_3d_corr::erase(vcl_string const& site)
{
  matches_.erase(site);
}

//: return the list of sites being corresponded
vcl_vector<vcl_string> bwm_3d_corr::sites() const
{
  vcl_vector<vcl_string>  sites(0);
  vcl_map<vcl_string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
  while (iter != matches_.end()) {
    sites.push_back(iter->first);
    ++iter;
  }
  return sites;
}

//: test if the site exists in the set of correspondences and if so return the associated 3-d point
bool bwm_3d_corr::site_in(vcl_string const& site, vgl_point_3d<double> &corr) const
{
  vcl_map<vcl_string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
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
vcl_vector<vcl_pair<vcl_string, vgl_point_3d<double> > > bwm_3d_corr::match_list() const
{
  vcl_vector<vcl_pair<vcl_string, vgl_point_3d<double> > > mlist;
  vcl_map<vcl_string, vgl_point_3d<double> >::const_iterator iter = matches_.begin();
  while (iter != matches_.end()) {
   vcl_pair<vcl_string, vgl_point_3d<double> > pair;
   pair.first = iter->first;
   pair.second = vgl_point_3d<double> (iter->second.x(), iter->second.y(), iter->second.z());
   mlist.push_back(pair);
   ++iter;
 }
  return mlist;
}
// return the set of corresponded points over all sites
vcl_vector<vgl_point_3d<double> >  bwm_3d_corr::matching_pts() const
{
  vcl_vector<vgl_point_3d<double> > matches;
  vcl_map<vcl_string, vgl_point_3d<double> >::const_iterator iter =
    matches_.begin();
  while (iter != matches_.end()){
    matches.push_back(iter->second);
    ++iter;
  }
  return matches;
}

// output stream
vcl_ostream& operator<<(vcl_ostream& s, bwm_3d_corr const& c)
{
  vcl_vector<vcl_pair<vcl_string, vgl_point_3d<double> > > mlist = c.match_list();

  s << "Number of Sites: " << mlist.size() << '\n';
  vcl_vector<vcl_pair<vcl_string, vgl_point_3d<double> > >::const_iterator iter = mlist.begin();
  while (iter != mlist.end()) {
    s <<  "Site[ " << iter->first << " ]:( X: " << iter->second.x()
      << " Y: " << iter->second.y() << " Z: " << iter->second.z() << " )\n";
    ++iter;
  }

  return s;
}
