#ifndef brad_nitf_abs_radiometric_calibration_h_
#define brad_nitf_abs_radiometric_calibration_h_

#include <brad/brad_image_metadata.h>
#include <vil/vil_convert.h>

vil_image_view_base_sptr brad_nitf_abs_radiometric_calibrate(vil_image_view_base_sptr img_sptr, brad_image_metadata_sptr md);
#endif // brad_nitf_abs_radiometric_calibration_h_
