#include <sdet/sdet_graph_img_seg.h>
#include <vbl/vbl_smart_ptr.hxx>

VBL_SMART_PTR_INSTANTIATE(sdet_graph_img_seg);

template void sdet_segment_img(vil_image_view<vxl_byte> const& img, unsigned margin, int neigh, vxl_byte weight_thres, float sigma, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img);
template void sdet_segment_img(vil_image_view<float> const& img, unsigned margin, int neigh, float weight_thres, float sigma, int min_size, vil_image_view<vil_rgb<vxl_byte> >& out_img);
