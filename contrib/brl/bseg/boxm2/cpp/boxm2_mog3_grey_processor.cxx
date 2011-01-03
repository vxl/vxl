#include "boxm2_mog3_grey_processor.h"
//:
// \file



float  boxm2_mog3_grey_processor::expected_color( vnl_vector_fixed<unsigned char, 8> mog3)
{
    float exp_intensity=(float)mog3[0]*(float)mog3[2]+
                        (float)mog3[3]*(float)mog3[5]+
                        (float)mog3[6]*(float)(255-mog3[2]-mog3[5]);

    exp_intensity/=(255.0f*255.0f);

    return exp_intensity;
}
