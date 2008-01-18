#include <bsta/algo/bsta_adaptive_updater.txx>
#include <bsta/bsta_gauss_if3.h>

typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_if3>,3> mix_fix_gauss_if3_3;

DBSTA_ADAPTIVE_UPDATER_INSTANTIATE(mix_fix_gauss_if3_3);
