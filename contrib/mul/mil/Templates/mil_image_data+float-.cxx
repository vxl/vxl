#include <mil/mil_image_data.txx>
// MIL_IMAGE_DATA_INSTANTIATE(float); - This fails wierdly

template class mil_image_data<float >;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<float >& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<float >& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<float >* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<float >*& v);
