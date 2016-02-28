#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <boct/boct_tree_cell.hxx>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

BOCT_TREE_CELL_INSTANTIATE(short, gauss_type);
