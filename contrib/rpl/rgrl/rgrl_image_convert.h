#ifndef rgrl_image_convert_h_
#define rgrl_image_convert_h_
//:
// \file
// \author Charlene Tsai
// \date   Oct 2003
// \brief Pre-processor that converts 3D vil_image_view from the pixel space to the real physical space.
//
//  Interpolation is trilinear.
//

#include <vil3d/vil3d_image_view.h>

template< class PixelType >
void
rgrl_image_convert_3d( const vil3d_image_view< PixelType >& src,
                       double x_space, double y_space, double z_space,
                       vil3d_image_view< PixelType >& dest);

#endif // rgrl_image_convert_h_
