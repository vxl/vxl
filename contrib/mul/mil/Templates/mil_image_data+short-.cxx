#include <mil/mil_image_data.txx>
MIL_IMAGE_DATA_INSTANTIATE(short); //- This fails wierdly

#if 0
template class mil_image_data<short >;
typedef mil_image_data<short> * mil_image_data_short_ptr;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<short >& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<short >* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data_short_ptr& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<short >& v);
#endif
