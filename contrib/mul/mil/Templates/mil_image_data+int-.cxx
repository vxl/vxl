#include <mil/mil_image_data.txx>
MIL_IMAGE_DATA_INSTANTIATE(int); //- This fails wierdly

#if 0
template class mil_image_data<int>;
typedef mil_image_data<int> * mil_image_data_int_ptr;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<int>& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<int>* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data_int_ptr& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<int>& v);
#endif
