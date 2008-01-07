#ifndef bugl_random_transform_set_2d_txx_
#define bugl_random_transform_set_2d_txx_

#include "bugl_random_transform_set_2d.h"

#include <vcl_cstdlib.h> // for rand()

template<class T>
void bugl_random_transform_set_2d<T>::
equiform_uniform(const unsigned n_trans,
                 vcl_vector<vgl_h_matrix_2d<T> >& transform_set,
                 const T dx, const T dy,
                 const T dtheta, const T ds)
{
  //convert to double
  double dx_d = dx, dy_d = dy, dtheta_d = dtheta, ds_d = ds;
  transform_set.resize(n_trans);
  for (unsigned i = 0; i<n_trans; ++i)
  {
    //generate random samples of uniform distribution
    T rnd_dx = (T)((2.0*dx_d)*(vcl_rand()/(RAND_MAX+1.0)) - dx_d);
    T rnd_dy = (T)((2.0*dy_d)*(vcl_rand()/(RAND_MAX+1.0)) - dy_d);
    T rnd_dtheta =
      (T)((2.0*dtheta_d)*(vcl_rand()/(RAND_MAX+1.0)) - dtheta_d);
    T rnd_ds = (T)((2.0*ds_d)*(vcl_rand()/(RAND_MAX+1.0)) - ds_d);
    rnd_ds += 1.0;
    transform_set[i].set_identity();
    transform_set[i].set_rotation(rnd_dtheta);
    transform_set[i].set_scale(rnd_ds);
    transform_set[i].set_translation(rnd_dx, rnd_dy);
  }
}

template<class T>
void bugl_random_transform_set_2d<T>::
zero_skew_affine_uniform(const unsigned n_trans,
                         vcl_vector<vgl_h_matrix_2d<T> >& transform_set,
                         const T dx, const T dy,
                         const T dtheta, const T ds,
                         const T daspect)
{
  //convert to double
  double dx_d = dx, dy_d = dy, dtheta_d = dtheta, ds_d = ds, da_d = daspect;
  transform_set.resize(n_trans);
  for (unsigned i = 0; i<n_trans; ++i)
  {
    //generate random samples of uniform distribution
    T rnd_dx = (T)((2.0*dx_d)*(vcl_rand()/(RAND_MAX+1.0)) - dx_d);
    T rnd_dy = (T)((2.0*dy_d)*(vcl_rand()/(RAND_MAX+1.0)) - dy_d);
    T rnd_dtheta =
      (T)((2.0*dtheta_d)*(vcl_rand()/(RAND_MAX+1.0)) - dtheta_d);
    T rnd_ds = (T)((2.0*ds_d)*(vcl_rand()/(RAND_MAX+1.0)) - ds_d);
    rnd_ds += 1.0;
    T rnd_da = (T)((2.0*da_d)*(vcl_rand()/(RAND_MAX+1.0)) - da_d);
    rnd_da += 1.0;
    transform_set[i].set_identity();
    transform_set[i].set_rotation(rnd_dtheta);
    transform_set[i].set_scale(rnd_ds);
    transform_set[i].set_aspect_ratio(rnd_da);
    transform_set[i].set_translation(rnd_dx, rnd_dy);
  }
}

template<class T>
void bugl_random_transform_set_2d<T>::
zero_skew_affine_uniform_interval(const unsigned n_trans,
                                  vcl_vector<vgl_h_matrix_2d<T> >& trans_set,
                                  const T xmin, const T xmax,
                                  const T ymin, const T ymax,
                                  const T theta_min, const T theta_max,
                                  const T scale_min, const T scale_max,
                                  const T aspect_min, const T aspect_max)
{
  //convert to double
  double xmin_d = xmin, xmax_d = xmax, ymin_d = ymin, ymax_d = ymax;
  double theta_min_d = theta_min, theta_max_d = theta_max;
  double scale_min_d = scale_min, scale_max_d = scale_max;
  double aspect_min_d = aspect_min, aspect_max_d = aspect_max;

  trans_set.resize(n_trans);
  for (unsigned i = 0; i<n_trans; ++i)
  {
    //generate random samples of uniform distribution
    T rnd_x = (T)((xmax_d-xmin_d)*(vcl_rand()/(RAND_MAX+1.0)) + xmin_d );
    T rnd_y = (T)((ymax_d-ymin_d)*(vcl_rand()/(RAND_MAX+1.0)) + ymin_d );
    T rnd_theta = (T)((theta_max_d-theta_min_d) *
                      (vcl_rand()/(RAND_MAX+1.0)) + theta_min_d );
    T rnd_scale = (T)((scale_max_d-scale_min_d) *
                      (vcl_rand()/(RAND_MAX+1.0)) + scale_min_d );
    T rnd_aspect = (T)((aspect_max_d-aspect_min_d) *
                       (vcl_rand()/(RAND_MAX+1.0)) + aspect_min_d );

    trans_set[i].set_identity();
    trans_set[i].set_rotation(rnd_theta);
    trans_set[i].set_scale(rnd_scale);
    trans_set[i].set_aspect_ratio(rnd_aspect);
    trans_set[i].set_translation(rnd_x, rnd_y);
  }
}

//----------------------------------------------------------------------------
#undef BUGL_RANDOM_TRANSFORM_SET_2D_INSTANTIATE
#define BUGL_RANDOM_TRANSFORM_SET_2D_INSTANTIATE(T) \
template class bugl_random_transform_set_2d<T >

#endif // bugl_random_transform_set_2d_txx_
