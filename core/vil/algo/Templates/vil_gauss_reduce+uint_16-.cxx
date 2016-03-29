#include <vxl_config.h>
#include <vil/algo/vil_gauss_reduce.hxx>

//Explicit instantiation of the called single plane template functions also seems to be needed
//by some compilers/where not covered by one of the specialisations

template void vil_gauss_reduce_1plane(const vxl_uint_16* src_im,
                                      unsigned src_nx, unsigned src_ny,
                                      std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                      vxl_uint_16* dest_im,
                                      std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step);
template void vil_gauss_reduce_121_1plane(const vxl_uint_16* src_im,
                                          unsigned src_nx, unsigned src_ny,
                                          std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                          vxl_uint_16* dest_im,
                                          std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step);
template void vil_gauss_reduce_2_3_1plane(const vxl_uint_16* src_im,
                                          unsigned src_ni, unsigned src_nj,
                                          std::ptrdiff_t s_x_step, std::ptrdiff_t s_y_step,
                                          vxl_uint_16* dest_im, std::ptrdiff_t d_x_step, std::ptrdiff_t d_y_step);

VIL_GAUSS_REDUCE_INSTANTIATE(vxl_uint_16);
