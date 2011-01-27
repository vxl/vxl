#include <bvpl/bvpl_combined_neighb_operator.h>
#include <bvpl/kernels/bvpl_corner2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/bvpl_direction_to_color_map.h>
#include <bvpl/bvpl_discriminative_non_max_suppression.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_edge2d_functor.h>
#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/functors/bvpl_find_surface_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_xx_kernel_factory.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/kernels/bvpl_kernel_iterator.h>
#include <bvpl/functors/bvpl_local_max_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/kernels/bvpl_neighborhood_kernel_factory.h>
#include <bvpl/functors/bvpl_negative_gauss_conv_functor.h>
#include <bvpl/functors/bvpl_opinion_functor.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/bvpl_subgrid_iterator.h>
#include <bvpl/bvpl_susan_opinion_operator.h>
#include <bvpl/bvpl_vector_operator.h>
#include <bvpl/bvpl_voxel_subgrid.h>
#include <bvpl/kernels/bvpl_weighted_cube_kernel_factory.h>

#include <bvpl/pro/bvpl_processes.h>
#include <bvpl/pro/bvpl_register.h>

#include <bvpl/kernels/io/bvpl_io_kernel.h>

int main() { return 0; }
