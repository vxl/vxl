#ifndef icam_image_h_
#define icam_image_h_
//:
// \file
// \brief An image with an associated transformation
//        Partially copied from ihog_image
// \author Peter Vanroose
// \date October 10, 2010 (10/10/10)

#include "icam_transform_2d.h"
#include <vil/vil_image_view.h>

template <class T>
class icam_image : public vil_image_view<T>, public icam_transform_2d
{
 public:
  //: Default constructor
  //  Creates an empty one-plane image.
  icam_image() : vil_image_view<T>(), icam_transform_2d() {}

  //: Construct an image of size (ni, nj, np) with optional world_to_image transform.
  icam_image(unsigned ni, unsigned nj, unsigned np=1,
             icam_transform_2d const& w2i = icam_transform_2d())
    : vil_image_view<T>(ni, nj, np), icam_transform_2d(w2i) {}

  //: Construct from a view and optional world-to-image transform.
  icam_image(vil_image_view<T> const& view,
             icam_transform_2d const& w2i = icam_transform_2d())
    : vil_image_view<T>(view), icam_transform_2d(w2i) {}

  //: Destructor
  ~icam_image() override = default;
};

#endif // icam_image_h_
