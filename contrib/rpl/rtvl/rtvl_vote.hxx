/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_vote_hxx
#define rtvl_vote_hxx

#include "rtvl_vote.h"

#include "rtvl_terms.h"
#include "rtvl_votee.h"
#include "rtvl_voter.h"
#include "rtvl_weight.h"

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

//----------------------------------------------------------------------------
template <unsigned int N>
class rtvl_vote_internal: private rtvl_terms<N>
{
public:
  rtvl_vote_internal(vnl_vector_fixed<double, N> const& voter_location,
                     vnl_vector_fixed<double, N> const& votee_location,
                     vnl_vector_fixed<double, N> const(&voter_basis)[N],
                     rtvl_weight<N>& w);
  void move_basis(unsigned int d);
  void compute(vnl_matrix_fixed<double, N, N>& vote);
  void compute_d(vnl_matrix_fixed<double, N, N> dvote[N]);

private:
  typedef rtvl_terms<N> terms;

  // Bring dependent names in from our superclass.
  using terms::v;
  using terms::vhat;
  using terms::vlen;
  using terms::vlen_squared;
  using terms::cos_theta;
  using terms::sin_theta;
  using terms::dtheta;
  using terms::wflat;
  using terms::wcurve;
  using terms::dwflat;
  using terms::dwcurve;

  vnl_vector_fixed<double, N> const(&basis_)[N];
  rtvl_weight<N>& weighter;

  vnl_vector_fixed<double, N> v_projected;

  vnl_vector_fixed<double, N> vnu;
  vnl_vector_fixed<double, N> vtu;
  vnl_matrix_fixed<double, N, N> normal_space;

  vnl_vector_fixed<double, N> vn;
  vnl_vector_fixed<double, N> vt;
  vnl_vector_fixed<double, N> vc;
  vnl_matrix_fixed<double, N, N> vn_outer_vn;
  vnl_matrix_fixed<double, N, N> vc_outer_vc;

  vnl_matrix_fixed<double, N, N> dvhat;
  vnl_matrix_fixed<double, N, N> dvn;
  vnl_matrix_fixed<double, N, N> dvt;
  vnl_matrix_fixed<double, N, N> dvc;

  double sin_2theta;
  double cos_2theta;
  bool curved;

  void compute_curved(vnl_matrix_fixed<double, N, N>& vote);
  void compute_dvhat();
  void compute_dvn();
  void compute_dvt();
  void compute_dtheta();
  void compute_dvc();
};

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_vote_internal<N>
::rtvl_vote_internal(vnl_vector_fixed<double, N> const& voter_location,
                     vnl_vector_fixed<double, N> const& votee_location,
                     vnl_vector_fixed<double, N> const(&voter_basis)[N],
                     rtvl_weight<N>& w):
  basis_(voter_basis), weighter(w)
{
  // Compute the vector pointing from voter to votee.
  v = votee_location - voter_location;
  vlen_squared = v.squared_magnitude();
  vlen = std::sqrt(vlen_squared);
  vhat = v;
  if(vlen > 0) { vhat = vhat / vlen; }

  // Project the votee into the voter basis.
  for(unsigned int i=0; i < N; ++i)
    {
    v_projected(i) = dot_product(voter_basis[i], v);
    }

  // Initially every direction is in normal space.
  normal_space.set_identity();
  vnu = v;
  vtu.fill(0);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>
::move_basis(unsigned int d)
{
  // Move the vector from normal space to tangent space.
  vnl_vector_fixed<double, N> const& b = this->basis_[d-1];
  normal_space -= outer_product(b,b);
  vnu -= b*v_projected[d-1];
  vtu += b*v_projected[d-1];
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute(vnl_matrix_fixed<double, N, N>& vote)
{
  curved = false;

  // Vote with the normal space assuming no curvature.
  wflat = weighter.compute_flat(*this);
  vote = normal_space * wflat;

  // If the votee projects into the normal-space with a
  // non-trivial fraction of its length there may be curvature.
  double vnu_mag2 = vnu.squared_magnitude();
  if(vnu_mag2 > 1e-16*vlen_squared)
    {
    vn = vnu; vn /= std::sqrt(vnu_mag2);
    vt = vtu; vt.normalize();
    sin_theta = dot_product(vhat, vn);
    if(sin_theta < 0) { sin_theta = 0; }

    // Update the vote for curvature.
    this->compute_curved(vote);
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_vote_internal<N>::compute_curved(vnl_matrix_fixed<double, N, N>& vote)
{
  curved = true;

  // Vote with curvature.
  cos_theta = dot_product(vhat, vt);
  if(cos_theta < 0) { cos_theta = 0; }

  // Truncate to avoid numerical roundoff issues.
  if(sin_theta > 1) { sin_theta = 1; }
  if(cos_theta > 1) { cos_theta = 1; }

  // Normal direction moved along the hypothesized curve.
  sin_2theta = 2*sin_theta*cos_theta;
  cos_2theta = 1-2*sin_theta*sin_theta;
  vc = cos_2theta*vn - sin_2theta*vt;

  wcurve = weighter.compute_curved(*this);

  vn_outer_vn = outer_product(vn, vn);
  vc_outer_vc = outer_product(vc, vc);

  vote -= vn_outer_vn * wflat;
  vote += vc_outer_vc * wcurve;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_dvhat()
{
  dvhat.set_identity();
  for(unsigned int i=0; i < N; ++i)
    {
    for(unsigned int k=0; k < N; ++k)
      {
      dvhat(i,k) -= vhat(i)*vhat(k);
      }
    }
  if(vlen > 0)
    {
    dvhat /= vlen;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_dvn()
{
  dvn = normal_space;
  for(unsigned int i=0; i < N; ++i)
    {
    for(unsigned int k=0; k < N; ++k)
      {
      dvn(i,k) -= vn(i)*vn(k);
      }
    }
  double vnu_norm = vnu.two_norm();
  if(vnu_norm > 0)
    {
    dvn /= vnu_norm;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_dvt()
{
  dvt.set_identity();
  dvt -= normal_space;
  for(unsigned int i=0; i < N; ++i)
    {
    for(unsigned int k=0; k < N; ++k)
      {
      dvt(i,k) -= vt(i)*vt(k);
      }
    }
  double vtu_norm = vtu.two_norm();
  if(vtu_norm > 0)
    {
    dvt /= vtu_norm;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_dtheta()
{
  dtheta = dvhat*vn + dvn*vhat;
  if(cos_theta > 0)
    {
    dtheta /= cos_theta;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_dvc()
{
  for(unsigned int i=0; i < N; ++i)
    {
    for(unsigned int k=0; k < N; ++k)
      {
      dvc(i,k) = (dvn(i,k)*cos_2theta - vn(i)*sin_2theta*2*dtheta(k) -
                  dvt(i,k)*sin_2theta - vt(i)*cos_2theta*2*dtheta(k));
      }
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_internal<N>::compute_d(vnl_matrix_fixed<double, N, N> dvote[N])
{
  weighter.compute_flat_d(*this, dwflat);
  for(unsigned int k=0; k < N; ++k)
    {
    dvote[k] = normal_space*dwflat(k);
    }
  if(!curved)
    {
    return;
    }

  this->compute_dvhat();
  this->compute_dvn();
  this->compute_dvt();
  this->compute_dtheta();
  this->compute_dvc();
  weighter.compute_curved_d(*this, dwcurve);

  for(unsigned int k=0; k < N; ++k)
    {
    dvote[k] -= vn_outer_vn * dwflat(k);
    for(unsigned int i=0; i < N; ++i)
      {
      for(unsigned int j=0; j < N; ++j)
        {
        dvote[k](i,j) -= (dvn(i,k)*vn[j] + vn[i]*dvn(j,k))*wflat;
        }
      }
    }

  for(unsigned int k=0; k < N; ++k)
    {
    dvote[k] += vc_outer_vc * dwcurve(k);
    for(unsigned int i=0; i < N; ++i)
      {
      for(unsigned int j=0; j < N; ++j)
        {
        dvote[k](i,j) += (dvc(i,k)*vc[j] + vc[i]*dvc(j,k))*wcurve;
        }
      }
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote(rtvl_voter<N>& voter, rtvl_votee<N>& votee,
               rtvl_weight<N>& w, bool include_ball)
{
  rtvl_vote_internal<N> vi(voter.location(), votee.location(),
                           voter.basis(), w);

  // Evaluate the vote for each tensor component from highest
  // dimension to lowest dimension of the normal space.
  bool do_component = include_ball;
  for(unsigned int d = N; d > 0; --d)
    {
    double saliency = voter.lambda(d-1) - voter.lambda(d);
    if(do_component && saliency > 0)
      {
      votee.go(vi, saliency);
      }
    vi.move_basis(d);
    do_component = true;
    }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_vote_component(rtvl_vote_internal<N>& vi,
                         vnl_matrix_fixed<double, N, N>& vote)
{
  vi.compute(vote);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void
rtvl_vote_component_d(rtvl_vote_internal<N>& vi,
                      vnl_matrix_fixed<double, N, N> (&dvote)[N])
{
  vi.compute_d(dvote);
}

//----------------------------------------------------------------------------

#define RTVL_VOTE_INSTANTIATE(N) \
  template class rtvl_vote_internal<N>; \
  template void rtvl_vote<N>( \
    rtvl_voter<N>&, rtvl_votee<N>&, rtvl_weight<N>&, bool); \
  template void rtvl_vote_component<N>( \
    rtvl_vote_internal<N>&, vnl_matrix_fixed<double, N, N>&); \
  template void rtvl_vote_component_d<N>( \
    rtvl_vote_internal<N>&, vnl_matrix_fixed<double, N, N> (&)[N])

#endif
