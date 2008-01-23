#include <bsta/algo/bsta_adaptive_updater.txx>
#include <bsta/bsta_gauss_f1.h>

typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3> mix_fix_gauss_f1_3;

DBSTA_ADAPTIVE_UPDATER_INSTANTIATE(mix_fix_gauss_f1_3);
