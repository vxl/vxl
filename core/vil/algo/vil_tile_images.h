// This is core/vil/algo/vil_tile_images.h
#ifndef vil_tile_images_h_
#define vil_tile_images_h_
//:
// \file
// \brief Create a big image by tiling images in patches
// \author Tim Cootes

#include <vector>
#include <cmath>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Create a big image by tiling images in patches (must be of same size).
//  Creates a large image by putting smaller images into an approximately
//  square grid.
//  If there are n small images, the grid will have sqrt(n) columns.
// \sa contrib/mul/tools/tile_images_2d
// \relatesalso vil_image_view
template<class T>
inline void vil_tile_images(vil_image_view<T>& big_image,
                            const std::vector<vil_image_view<T> >& patches)
{
  unsigned n = patches.size();
  unsigned nj = unsigned(std::sqrt(float(n)));
  unsigned ni = 1+(n-1)/nj;

  unsigned pi = patches[0].ni();
  unsigned pj = patches[0].nj();
  unsigned np = patches[0].nplanes();

  big_image.set_size(ni*pi,nj*pj);
  big_image.fill(vxl_byte(0));
  for (unsigned k=0;k<n;++k)
  {
    assert(patches[k].ni()==pi && patches[k].nj()== pj &&
           patches[k].nplanes()== np);

    unsigned i = k%ni;
    unsigned j = k/ni;
    for (unsigned y=0;y<pj;++y)
      for (unsigned x=0;x<pi;++x)
        for (unsigned p=0;p<np;++p)
          big_image(i*pi+x,j*pj+y,p) = patches[k](x,y,p);
  }
}

#endif // vil_tile_images_h_
