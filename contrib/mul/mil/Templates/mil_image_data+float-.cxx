#include <mil/mil_image_data.txx>
MIL_IMAGE_DATA_INSTANTIATE(float); //- This fails wierdly

#if 0
template class mil_image_data<float >;
typedef mil_image_data<float> * mil_image_data_float_ptr;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<float >& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<float >* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data_float_ptr& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<float >& v);
#endif
