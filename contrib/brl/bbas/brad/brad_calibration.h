#ifndef brad_calibration_h_
#define brad_calibration_h_

#include <vector>
#include <tuple>
#include <vil/vil_image_view.h>
#include <brad/brad_image_metadata.h>

//: scale & offset to convert digital number (DN) to top-of-atmosphere (ToA) reflectance
std::tuple<std::vector<double>, std::vector<double> >
brad_radiometric_calibration_params(brad_image_metadata const& md);

//: convert DN to ToA reflectance
vil_image_view<float>
brad_radiometric_calibration(vil_image_view<unsigned short> const& digital_number,
                             std::vector<double> gain = {},
                             std::vector<double> offset = {});

//: convert digital number (DN) to top-of-atmosphere (ToA) reflectance
vil_image_view<float> brad_nitf_abs_radiometric_calibrate(vil_image_view<unsigned short> const& input_img, brad_image_metadata const& md);

#endif  // brad_calibration_h_
