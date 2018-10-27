#include "vsph_grid_index_2d.h"
#include "vsph_utils.h"
#include <vnl/vnl_math.h>
#include <vgl/vgl_vector_3d.h>

double vsph_grid_index_2d::pye() const
{
  if (in_radians_)
    return vnl_math::pi;
  return 180.0;
}
vsph_grid_index_2d::vsph_grid_index_2d(): n_bins_theta_(180), n_bins_phi_(360), in_radians_(true){
  std::vector<std::pair<vsph_sph_point_2d, int> > temp0;
  std::vector<std::vector<std::pair<vsph_sph_point_2d, int> > > temp(n_bins_phi_+1, temp0);
  // two extra bins are reserved for theta = 0 and theta = 180
  // since for these bins, the value of phi is irrelevant
  index_.resize(n_bins_theta_+2, temp);

  theta_inc_ = pye()/n_bins_theta_;
  phi_inc_ = 2.0*pye()/n_bins_phi_;
}

vsph_grid_index_2d::vsph_grid_index_2d(unsigned n_bins_theta, unsigned n_bins_phi,
                                       bool in_radians):
  n_bins_theta_(n_bins_theta), n_bins_phi_(n_bins_phi),
  in_radians_(in_radians)
{
  std::vector<std::pair<vsph_sph_point_2d, int> > temp0;
  std::vector<std::vector<std::pair<vsph_sph_point_2d, int> > > temp(n_bins_phi_+1, temp0);
  // two extra bins are reserved for theta = 0 and theta = 180
  // since for these bins, the value of phi is irrelevant
  index_.resize(n_bins_theta_+2, temp);

  theta_inc_ = pye()/n_bins_theta_;
  phi_inc_ = 2.0*pye()/n_bins_phi_;
}

bool vsph_grid_index_2d::index(vsph_sph_point_2d const& sp,
                               unsigned& th_idx, unsigned& ph_idx) const
{
  double ph = sp.phi_, th = sp.theta_;
  bool in_radians = sp.in_radians_;
  // convert input angles to box angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  if (ph == -pye()) ph = pye();

  if (th == 0.0) {
   th_idx = n_bins_theta_;
   ph_idx = n_bins_phi_;
   return true;
  }
  else if (th == pye()) {
    th_idx = n_bins_theta_ + 1;
    ph_idx = n_bins_phi_;
    return true;
  }
  else {
    th_idx = static_cast<unsigned>(std::floor(th/theta_inc_));
    if (th_idx >= n_bins_theta_)
      return false;
  }
  ph_idx = 0;
  if (ph == pye())
    ph_idx = n_bins_phi_-1;
  else{
    double diff = std::floor(ph+pye());
    ph_idx = static_cast<unsigned>(diff/phi_inc_);
    if (ph_idx >= n_bins_phi_)
      return false;
  }
  return true;
}

bool vsph_grid_index_2d::insert(vsph_sph_point_2d const& sp, int id )
{
  unsigned th_idx, phi_idx;
  int find_id = -1;
  if (this->find(sp, th_idx, phi_idx, find_id)) return false;
  if (!index(sp, th_idx, phi_idx)) return false;

  double ph = sp.phi_, th = sp.theta_;
  bool in_radians = sp.in_radians_;
  // convert input angles to grid angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  vsph_sph_point_2d spin(th, ph, in_radians_);
  std::pair<vsph_sph_point_2d, int> pr(spin, id);
  index_[th_idx][phi_idx].push_back(pr);
  return true;
}

static void add(std::vector<std::pair<vsph_sph_point_2d, int> > const& vec_to_add, std::vector<std::pair<vsph_sph_point_2d, int> >& result)
{
  unsigned n = vec_to_add.size();
  for (unsigned i = 0; i<n; ++i)
    result.push_back(vec_to_add[i]);
}

bool vsph_grid_index_2d::find(vsph_sph_point_2d const& sp, unsigned& th_idx,
                              unsigned& ph_idx, int& id,  double tol) const
{
  id = -1;
  double marg = MARGIN_DEG;
  if (this->in_radians_)
    marg = MARGIN_RAD;
  if (!index(sp, th_idx, ph_idx)) return false;
  double ph = sp.phi_, th = sp.theta_;
  bool in_radians = sp.in_radians_;
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  if (ph == -pye()) ph = pye();


  //determine if the point is near the grid bin boundaries
  double th_min = th_idx*theta_inc_, th_max = th_min+theta_inc_;
  double ph_min = (ph_idx*phi_inc_)-pye(), ph_max = ph_min + phi_inc_;
  double th_min_dif = std::fabs(th-th_min), th_max_dif = std::fabs(th_max-th);
  double ph_min_dif = std::fabs(vsph_utils::azimuth_diff(ph_min, ph, in_radians_));
  double ph_max_dif = std::fabs(vsph_utils::azimuth_diff(ph, ph_max, in_radians_));
  bool th_min_zero = th_min == 0.0;
  bool near_th_min = th>0.0 && (th_min_dif<=marg);

  bool th_max_pi = (th_max == pye());

  bool near_th_max = (th < pye()) && (th_max_dif<=marg);

  bool ph_min_mpi = (ph_min == -pye());
  bool near_ph_min = (ph > -pye())&& ( ph_min_dif<=marg);
  bool ph_max_pi = (ph_max == pye());
  bool near_ph_max = ph < pye() && (ph_max_dif<=marg);

  //add the bin specified by input thetha and phi
  std::vector<std::pair<vsph_sph_point_2d, int> > spts = index_[th_idx][ph_idx];
  // add points from surrounding bins if the point is close to the boundaries
  if (near_th_min&&!th_min_zero) add(index_[th_idx-1][ph_idx], spts);
  if (near_th_max&&!th_max_pi) add(index_[th_idx+1][ph_idx], spts);
  if (near_ph_min&&!ph_min_mpi) add(index_[th_idx][ph_idx-1], spts);
  if (near_ph_max&&!ph_max_pi) add(index_[th_idx][ph_idx+1], spts);
  if (near_th_min&&!th_min_zero&&near_ph_min&&!ph_min_mpi)
    add(index_[th_idx-1][ph_idx-1], spts);
  if (near_th_min&&!th_min_zero&&near_ph_max&&!ph_max_pi)
    add(index_[th_idx-1][ph_idx+1], spts);
  if (near_th_max&&!th_max_pi&&near_ph_min&&!ph_min_mpi)
    add(index_[th_idx+1][ph_idx-1], spts);
  if (near_th_max&&!th_max_pi&&near_ph_max&&!ph_max_pi)
    add(index_[th_idx+1][ph_idx+1], spts);
  // handle the north and south pole case
  if (th_min_zero) {
    add(index_[n_bins_theta_][n_bins_phi_], spts);
    for (unsigned i = 0; i<n_bins_phi_; ++i)
      add(index_[0][i], spts);
  }
  if (th_max_pi) {
    add(index_[n_bins_theta_+1][n_bins_phi_], spts);
    for (unsigned i = 0; i<n_bins_phi_; ++i)
      add(index_[n_bins_theta_-1][i], spts);
  }
  // handle the +- 180 cut case
  if (near_th_min&&!th_min_zero&&ph_min_mpi)
    add(index_[th_idx-1][n_bins_phi_-1], spts);
  if (near_th_min&&!th_min_zero&&ph_max_pi)
    add(index_[th_idx-1][0], spts);
  if (near_th_max&&!th_max_pi&&ph_min_mpi)
    add(index_[th_idx+1][n_bins_phi_-1], spts);
  if (near_th_max&&!th_max_pi&&ph_max_pi)
    add(index_[th_idx+1][0], spts);


  if (!this->in_radians_) {
    ph /= vnl_math::deg_per_rad;
    th /= vnl_math::deg_per_rad;
  }
  double x = std::sin(th)*std::cos(ph);
  double y = std::sin(th)*std::sin(ph);
  double z = std::cos(th);
  vgl_vector_3d<double> cp(x, y, z);

  bool found = false;
  if (spts.size())
    for (std::vector<std::pair<vsph_sph_point_2d, int> >::const_iterator sit = spts.begin(); (sit != spts.end())&&!found; ++sit) {
      double ph_ix = ((*sit).first).phi_,  th_ix = ((*sit).first).theta_;
      if (!this->in_radians_) {
        ph_ix /= vnl_math::deg_per_rad;
        th_ix /= vnl_math::deg_per_rad;
      }
      double xix = std::sin(th_ix)*std::cos(ph_ix);
      double yix = std::sin(th_ix)*std::sin(ph_ix);
      double zix = std::cos(th_ix);
      vgl_vector_3d<double> cpix(xix, yix, zix);
      vgl_vector_3d<double> dif = cp-cpix;
      double dist = length(dif);
      if (dist<=tol) {
        found = true;
        id = (*sit).second;
      }
    }
 return found;
}

void vsph_grid_index_2d::clear()
{
  auto iit = index_.begin();
  for (; iit!=index_.end(); iit++) {
    auto jit =
      (*iit).begin();
    for (; jit != (*iit).end(); ++jit)
      (*jit).clear();
  }
}
