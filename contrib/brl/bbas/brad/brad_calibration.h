#ifndef brad_calibration_h_
#define brad_calibration_h_

#include <brad/brad_image_metadata.h>
#include <vil/vil_image_view.h>

vil_image_view<float> brad_nitf_abs_radiometric_calibrate(vil_image_view<unsigned short> const& input_img, brad_image_metadata const& md);
#endif  // brad_calibration_h_
