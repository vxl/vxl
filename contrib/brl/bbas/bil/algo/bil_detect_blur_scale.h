#ifndef bil_detect_blur_scale_h
#define bil_detect_blur_scale_h

#include <vil/vil_image_resource_sptr.h>

bool inbounds(int x,int y,vil_image_resource_sptr & img);

void bil_detect_blur_scale(vil_image_resource_sptr  & img, int len_of_curves,float & est_sigma);

#endif
