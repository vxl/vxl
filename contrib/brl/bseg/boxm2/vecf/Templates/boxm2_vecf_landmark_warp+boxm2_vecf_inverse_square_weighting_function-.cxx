#include "../boxm2_vecf_vector_field.txx"
#include "../boxm2_vecf_landmark_warp.txx"
#include "../boxm2_vecf_inverse_square_weighting_function.h"

BOXM2_VECF_LANDMARK_WARP_INSTANTIATE(boxm2_vecf_inverse_square_weighting_function);
BOXM2_VECF_VECTOR_FIELD_INSTANTIATE(boxm2_vecf_landmark_mapper<boxm2_vecf_inverse_square_weighting_function> );
