#include <bvpl/bvpl_subgrid_iterator.txx>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>

BVPL_SUBGRID_ITERATOR_INSTANTIATE(bsta_gauss_f1);

typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
BVPL_SUBGRID_ITERATOR_INSTANTIATE(gauss_type);
