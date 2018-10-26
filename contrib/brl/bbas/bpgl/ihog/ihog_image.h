#ifndef ihog_image_H_
#define ihog_image_H_
//:
// \file
// \brief An image with an associated transformation
// \author Matt Leotta
// \date 4/14/04
//
// \verbatim
//  Modifications
//  8/2010
//  G.Tunali  removed dependency on vimt
// \endverbatim
// var
#include "ihog_transform_2d.h"
#include <vil/vil_image_view.h>

template <class T>
class ihog_image
{
 private:
  vil_image_view<T> image_;
  ihog_transform_2d xform_;
 public:
  //: Default constructor
  //  Creates an empty one-plane image.
  ihog_image() = default;

  //: Construct an image of size (ni, nj, np) with optional world_to_image transform.
  ihog_image(unsigned ni, unsigned nj, unsigned np=1,
             const ihog_transform_2d& w2i=ihog_transform_2d())
    : xform_(w2i), image_(ni, nj, np) {}

  //: Construct from a view and optional world-to-image transform (takes copies of both).
  explicit ihog_image(const vil_image_view<T>& view,
                      const ihog_transform_2d& w2i=ihog_transform_2d())
    : image_(view), xform_(w2i) {}

  //: Destructor
  virtual ~ihog_image() = default;

  //: Baseclass view of image
  virtual const vil_image_view_base& image_base() const { return image_; }

  //: Image view
  vil_image_view<T>& image() { return image_; }

  //: Image view
  const vil_image_view<T>& image() const { return image_; }

  ihog_transform_2d world2im() const { return xform_; }

  void set_world2im(const ihog_transform_2d& w2i) { xform_ = w2i ;}
};

#endif
