#ifndef vil2_grid_merge_h_
#define vil2_grid_merge_h_
//:
//  \file
//  \brief Merge two images in a chequer-board pattern.
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Merge two images in a chequer-board pattern.
//  image1 and image2 are merged by copying boxes of size (box_ni x box_nj)
//  alternatively from each one.
//  Useful for comparing two images.
template <class T>
inline void vil2_grid_merge(const vil2_image_view<T>& image1,
                         const vil2_image_view<T>& image2,
                         vil2_image_view<T>& dest_image, 
                         unsigned box_ni, unsigned box_nj)
{
  unsigned ni = image1.ni();
  unsigned nj = image1.nj();
  unsigned np = image1.nplanes();
  assert(image2.ni()==ni && image2.nj()==nj && image2.nplanes()==np);

  dest_image.set_size(ni,nj,np);

  for (unsigned p = 0; p<np;++p)
  {
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
      {
        if ( ((i/box_ni)+(j/box_nj))%2 == 0)
          dest_image(i,j,p)=image1(i,j,p);
        else
          dest_image(i,j,p)=image2(i,j,p);
      }
  }

}

#endif // vil2_grid_merge_h_
