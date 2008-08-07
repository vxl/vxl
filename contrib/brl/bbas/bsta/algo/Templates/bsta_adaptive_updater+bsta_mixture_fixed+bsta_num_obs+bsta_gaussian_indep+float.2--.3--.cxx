#include <bsta/algo/bsta_adaptive_updater.txx>
#include <bsta/bsta_gaussian_indep.h>

typedef bsta_mixture_fixed<bsta_num_obs<bsta_gaussian_indep<float,2> >,3> mix_fix_gauss_if2_3;

BSTA_ADAPTIVE_UPDATER_INSTANTIATE(mix_fix_gauss_if2_3);
