#ifndef bvxm_mog_norm_txx_
#define bvxm_mog_norm_txx_
//:
// \file

#include "bvxm_mog_norm.h"

#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <bvxm/grid/bvxm_voxel_grid.h>

//: Computes the l2 distances between the mixtures of gaussians in a bvxm_grid and a reference.
//  If no mixture is given as a reference, the first mixture in the grid is used
template <class T>
bool bvxm_mog_norm<T>::mog_l2_grid(bvxm_voxel_grid_base_sptr apm_base,
                                   bvxm_voxel_grid_base_sptr mask_base,
                                   bvxm_voxel_grid_base_sptr dist_base,
                                   bool reference_given,
                                   mix_gauss_type reference)
{
  // cast grids
  bvxm_voxel_grid< mix_gauss_type>* apm_grid = static_cast<bvxm_voxel_grid< mix_gauss_type>* >(apm_base.ptr());
  bvxm_voxel_grid<bool>* mask_grid = static_cast<bvxm_voxel_grid< bool>* >(mask_base.ptr());
  bvxm_voxel_grid<T>* distance_grid = static_cast<bvxm_voxel_grid< T >* >(dist_base.ptr());
  distance_grid->initialize_data(float(0));

  if (!apm_grid ||!mask_grid || !distance_grid)
    return false;

  vgl_vector_3d<unsigned> grid_size = apm_grid->grid_size();
  vcl_cout << "Grid Size: " << grid_size << vcl_endl;

  // iterate through the apm grid and measure distances
  typename bvxm_voxel_grid<mix_gauss_type>::iterator apm_slab_it = apm_grid->begin();
  typename bvxm_voxel_grid<T>::iterator dist_slab_it = distance_grid->begin();
  bvxm_voxel_grid<bool>::iterator mask_slab_it = mask_grid->begin();

  if (reference_given){
    for (unsigned i=0; i<reference.num_components(); i++)
    {
      vcl_cout << "Mean " <<reference.distribution(i).mean() << vcl_endl
               << "Var " <<reference.distribution(i).var() << vcl_endl;
    }
  }

  vcl_cout << "Measuring distances:" << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++apm_slab_it, ++mask_slab_it, ++dist_slab_it)
  {
    vcl_cout << '.';

    //3. Iterate through slab and threshold. At this point the grids get updated on disk
    typename bvxm_voxel_slab<mix_gauss_type>::iterator apm_it = (*apm_slab_it).begin();
    bvxm_voxel_slab<bool>::iterator mask_it = (*mask_slab_it).begin();
    typename bvxm_voxel_slab<T>::iterator dist_it = (*dist_slab_it).begin();

    // If the reference is not given then it is the one in the first voxel
    if (!reference_given) {
      reference = *((*apm_slab_it).first_voxel());
      vcl_cout << "Reference Mixture:\n";
      reference_given = true;
      for (unsigned i=0; i<reference.num_components(); i++)
      {
        vcl_cout << "Mean " <<reference.distribution(i).mean() << vcl_endl
                 << "Var " <<reference.distribution(i).var() << vcl_endl;
      }
    }

    for (; apm_it!= (*apm_slab_it).end(); ++apm_it, ++mask_it, ++dist_it)
    {
      T distance = bvxm_mog_norm<T>::mog_l2(*apm_it, reference);
      if (*mask_it)
        ( *dist_it) = distance;
    }
  }
  return true;
}

//:Computes the l2 norm between two mixtures of gaussians
template <class T>
T bvxm_mog_norm<T>::mog_l2 (mix_gauss_type const& g, mix_gauss_type const& f)
{
  //define some constants so that they are only calculated once
  //srqt(4*pi)
  T k1 = T(vnl_math::two_over_sqrtpi * 0.25);

  // This distance is composed of 5 terms
  T t1 = 0;
  T t2 = 0;
  T t3 = 0;
  T t4 = 0;
  T t5 = 0;

  //terms 1,2,3
  for (unsigned i = 0; i < g.num_components(); i++)
  {
    t1 = t1 + vcl_pow(g.weight(i),2 )* k1 * (T(1)/vcl_sqrt(g.distribution(i).var()));

    for (unsigned j = i+1; j < g.num_components(); j++)
    {
      T sum_var = g.distribution(i).var() +  g.distribution(j).var();
      t2 = t2 + g.weight(i)*g.weight(j)
        * T(vnl_math::one_over_sqrt2pi)
        * (T(1)/vcl_sqrt(sum_var))
        * vcl_exp(-(T(1)/T(2)) * vcl_pow((g.distribution(i).mean()- g.distribution(j).mean()),2)
        * (T(1)/sum_var));
    }

    for (unsigned j = 0; j < f.num_components(); j++)
    {
      T sum_var = g.distribution(i).var() + f.distribution(j).var();
      t3 += g.weight(i) * f.weight(j)
          * T(vnl_math::one_over_sqrt2pi)
          / T(vcl_sqrt(sum_var))
          * T(vcl_exp(-0.5 * vcl_pow(g.distribution(i).mean()-f.distribution(j).mean(), 2) / sum_var));
    }
  }

  //terms 4,5
  for (unsigned i = 0; i < f.num_components(); i++)
  {
    t4 = t4 + vcl_pow(f.weight(i),2)*k1*(T(1)/vcl_sqrt(f.distribution(i).var()));

    for (unsigned j = i+1; j < f.num_components(); j++)
    {
      T sum_var = f.distribution(i).var() + f.distribution(j).var();
      t5 += f.weight(i) * f.weight(j)
          * T(vnl_math::one_over_sqrt2pi)
          / T(vcl_sqrt(sum_var))
          * T(vcl_exp(-0.5 * vcl_pow(f.distribution(i).mean()-f.distribution(j).mean(), 2) / sum_var));
    }
  }

  return vcl_sqrt(t1 +2*t2 - 2*t3 + t4 + 2*t5);
}

//: Calculate l2-norm between two gaussian pdfs
template <class T>
T bvxm_mog_norm<T>::l2_gauss2mix(gauss_type const&g, mix_gauss_undef const&f, bool normalize)
{
  //define some constants so that they are only calculated once
  //sqrt(4*pi)
  T k1 = T(vnl_math::two_over_sqrtpi * 0.25);

  // This distance is composed of 5 terms

  //terms 1,3
  T t1 = T(k1 * (1.0/vcl_sqrt(g.var())));
  T t3 = 0;

  for (unsigned j = 0; j < f.num_components(); j++)
  {
    T sum_var = g.var() + f.distribution(j).var();
    t3 = t3 + f.weight(j)
      * T(vnl_math::one_over_sqrt2pi)
      * (T(1)/vcl_sqrt(sum_var))
      * vcl_exp(-(T(1)/T(2))* vcl_pow((g.mean() - f.distribution(j).mean()), 2)
      * (T(1)/sum_var));
  }


  //terms 4,5
  T t4 = 0;
  T t5 = 0;

  for (unsigned i = 0; i < f.num_components(); i++)
  {
    t4 = t4 + vcl_pow(f.weight(i),2)*k1*(T(1)/vcl_sqrt(f.distribution(i).var()));

    for (unsigned j = i+1; j < f.num_components(); j++)
    {
      T sum_var = f.distribution(i).var() + f.distribution(j).var();

      t5 = t5 + f.weight(i)*f.weight(j)
        * T(vnl_math::one_over_sqrt2pi)
        * (T(1)/vcl_sqrt(sum_var))
        *  vcl_exp(-(T(1)/T(2))*vcl_pow(f.distribution(i).mean() - f.distribution(j).mean(),2)
        * (T(1)/sum_var));
    }
  }
  if (normalize)
    return T(vcl_sqrt(t1 - 2*t3 + t4 + 2*t5))/T(vcl_sqrt(t1 + t4 + 2*t5));
  else
    return T(vcl_sqrt(t1 - 2*t3 + t4 + 2*t5));
}

//: Calculate l2-norm between a mixture and gaussian pdfs
template <class T>
T bvxm_mog_norm<T>::l2_gauss(gauss_type const&g1, gauss_type const& g2, bool  normalize)
{
  //define some constants so that they are only calculated once
  //srqt(4*pi)
  T k = (1.0f/4.0f) * T(vnl_math::two_over_sqrtpi);
  T t1 = k*(1.0f/vcl_sqrt(g1.var()));
  T t2 = k*(1.0f/vcl_sqrt(g2.var()));
  T t3 = T(vnl_math::one_over_sqrt2pi)
    * (1.0f/vcl_sqrt(g1.var() + g2.var()))
    *  vcl_exp(-(1.0f/2.0f)*vcl_pow(g1.mean() - g2.mean(),2)
    * (1.0f/(g1.var() + g2.var())));

  if (normalize)
    return vcl_sqrt(t1 + t2 - 2*t3)/vcl_sqrt(t1 + t2);

  else
    return vcl_sqrt(t1 + t2 - 2*t3);
}


//: Calculates KL divergence for two gaussian distributions KL(g1|g2)
template <class T>
T bvxm_mog_norm<T>::kl_distance(gauss_type const&g1, gauss_type const& g2)
{
  T t1 = T(2.0*vcl_log(vcl_sqrt(g2.var()/g1.var())));
  T t2 = T(g1.var()/g2.var());
  T t3 = T(vcl_pow(g2.mean() - g1.mean(),2));
  if (t1+t2+t3 < T(1.0))
    vcl_cout << "smaller than 0: " << (t1+t2+t3-1)/2 << '\n';
  return T(0.5*(t1+t2+t3 -1.0));
}

//:Calculates symmetric version of KL divergence KL_sym(g1,g2) = 1/2(KL(g1|g2)+ KL(g2|g1))
template <class T>
T bvxm_mog_norm<T>::kl_symmetric_distance(gauss_type const&g1, gauss_type const& g2)
{
  return T(0.5)*(kl_distance(g1,g2)+kl_distance(g2,g1));
}

#define BVXM_MOG_NORM_INSTANTIATE(T) \
template class bvxm_mog_norm<T >

#endif
