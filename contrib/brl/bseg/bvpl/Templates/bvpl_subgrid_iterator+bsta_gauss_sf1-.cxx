#include <bvpl/bvpl_subgrid_iterator.hxx>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>

BVPL_SUBGRID_ITERATOR_INSTANTIATE(bsta_gauss_sf1);

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
BVPL_SUBGRID_ITERATOR_INSTANTIATE(gauss_type);
