#include <vil/vil_byte.h>
#include <mil/mil_image_data.txx>
// MIL_IMAGE_DATA_INSTANTIATE(vil_byte); - This fails wierdly

template class mil_image_data<vil_byte >;
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<vil_byte >& v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<vil_byte >& v);
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<vil_byte >* v);
template void vsl_b_read(vsl_b_istream& s, mil_image_data<vil_byte >*& v);
