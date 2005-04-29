#ifndef bugl_random_transform_set_2d_txx_
#define bugl_random_transform_set_2d_txx_
#include <vcl_cstdlib.h> // for rand()
#include <vnl/vnl_math.h>
#include <bugl/bugl_random_transform_set_2d.h>
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
  for(unsigned i = 0; i<n_trans; ++i)
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
//----------------------------------------------------------------------------
#undef BUGL_RANDOM_TRANSFORM_SET_2D_INSTANTIATE
#define BUGL_RANDOM_TRANSFORM_SET_2D_INSTANTIATE(T) \
template class bugl_random_transform_set_2d<T >

#endif // bugl_random_transform_set_2d_txx_
