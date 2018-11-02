#include <vector>
#include <iostream>
#include <stack>
#include "vil3d_fill_boundary.h"
//:
// \file
// \brief Fill in contour bounded regions in slices of 3D image
// \author Kola Babalola

#include <vil3d/vil3d_image_view.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_convert.h>
#include <vil3d/algo/vil3d_threshold.h>

//: Compute a mask where the regions in each slice of a 3D image bounded by contours are set to "on"
void vil3d_fill_boundary(vil3d_image_view<bool>& bool_image)
{
  unsigned ni = bool_image.ni();
  unsigned nj = bool_image.nj();
  unsigned nk = bool_image.nk();
  unsigned nplanes = bool_image.nplanes();

  // input image is binary, converted to int (need to speak to tim about data types accepted by threshold
  vil3d_image_view<int> image(ni,nj,nk,nplanes);
  vil3d_convert_cast(bool_image,image);

  std::ptrdiff_t istep = image.istep();

  // scan the image and look for a boundary pixel
  int *page0 = image.origin_ptr();

  int *p0 = page0;

  int boundary_label;
  int background_label = 2;
  for (unsigned int k = 0; k < nk; ++k)
  {
    boundary_label = 3;
    for (unsigned int j = 0; j < nj; ++j)
    {
      for (unsigned int i = 0; i < ni; ++i, p0+=istep)
      {
        if (image(i,j,k) == 1)
        {
          label_boundary_and_bkg(image,i,j,k,boundary_label,background_label);
          fill_boundary(image,j,k,boundary_label,background_label);
          boundary_label+=1;
          reset_background(image,background_label);
        }
      }
    }
  }

  // threshold the image to obtain the binary mask
  vil3d_threshold_above(image,bool_image,3);
}

//: Follow the current boundary in the current slice.
//  labeling boundary pixels and background pixels that border the boundary.
void label_boundary_and_bkg(vil3d_image_view<int> &image,int i,int j, int k, int boundary_label, int background_label)
{
  unsigned ni = image.ni();
  unsigned nj = image.nj();

  std::vector<int> x_offset(8); std::vector<int> y_offset(8);

  std::vector<int> next_dir(8);

  // assign the x and y offsets for the 8-neighbourhood
  x_offset[0] = -1; y_offset[0] =  0;
  x_offset[1] = -1; y_offset[1] = -1;
  x_offset[2] = 0;  y_offset[2] = -1;
  x_offset[3] = 1;  y_offset[3] = -1;
  x_offset[4] = 1;  y_offset[4] =  0;
  x_offset[5] = 1;  y_offset[5] =  1;
  x_offset[6] = 0;  y_offset[6] =  1;
  x_offset[7] = -1; y_offset[7] =  1;

  // assign the offsets for the next direction to look in
  next_dir[0] = next_dir[1] = 7;//-istep+jstep;
  next_dir[2] = next_dir[3] = 1;//-istep-jstep;
  next_dir[4] = next_dir[5] = 3;//istep-jstep;
  next_dir[6] = next_dir[7] = 5;//istep+jstep;

  int m,offset=0;
  int i0 = i,j0 = j;
  int i1, j1;

  // check the 8-neighbourhood of the pixels on the boundary
//  while (image(i0,j0,k) != boundary_label || (i!=i0 && j!=j0))
//    while (image(i0,j0,k) != boundary_label || ((i==i0 && j==j0) && (offset != 0 || offset != 1)))
  while (image(i0,j0,k) != boundary_label || (i != i0 || j != j0) ||
         (i == i0 && j == j0 && offset == 7))
  {
    image(i0,j0,k) = boundary_label;

    for (m = 0; m < 8; ++m)
    {
      i1 = i0+x_offset[offset];
      j1 = j0+y_offset[offset];
      if (i1 >= int(ni) || j1 >= int(nj) || i1 < 0 || j1 < 0)
        offset = (offset+1)%8;
      else
      {
        if (image(i1,j1,k)==1 || image(i1,j1,k) == boundary_label) // this means we visit some
        {                                                          // pixels more than once.
          i0 = i1; j0 = j1;                                        // Needed for degenerate cases
          offset = next_dir[offset];
          break;
        }
        else if (image(i1,j1,k)==background_label)
        {
          offset = (offset+1)%8;
          break;
        }
        else
        {
          image(i1,j1,k) = background_label;
          offset = (offset+1)%8;
        }
      }
    }
  }
}


//:  Fill interior of current boundary.
void fill_boundary(vil3d_image_view<int> &image, int j, int k, int boundary_label, int background_label)
{
  unsigned ni = image.ni();
  unsigned nj = image.nj();

  std::stack<int> x_stack, y_stack;
  int i=0, m;

  // push all boundary pixels onto stack. Needed for degenerate cases

  for (;j<int(nj);++j,i=0) // Why do i=0 here instead of in normal place on next line? No idea...
    for (;i<int(ni);++i)
    {
      if (image(i,j,k) == boundary_label)
      {
        x_stack.push(i);
        y_stack.push(j);
      }
    }

  // assign the x and y offsets for the 8-neighbourhood
  std::vector<int> x_offset(8); std::vector<int> y_offset(8);
  x_offset[0] = -1; y_offset[0] =  0;
  x_offset[1] = -1; y_offset[1] = -1;
  x_offset[2] = 0;  y_offset[2] = -1;
  x_offset[3] = 1;  y_offset[3] = -1;
  x_offset[4] = 1;  y_offset[4] =  0;
  x_offset[5] = 1;  y_offset[5] =  1;
  x_offset[6] = 0;  y_offset[6] =  1;
  x_offset[7] = -1; y_offset[7] =  1;

  int i1,j1;

  // Starting from first pixel on boundary, iteratively fill neighbours in boundary
  while (!x_stack.empty())
  {
    i = x_stack.top();
    j = y_stack.top();
    x_stack.pop();
    y_stack.pop();
    for (m = 0; m < 8; ++m)
    {
      i1 = i+x_offset[m];
      j1 = j+y_offset[m];
      if (i1<int(ni) && i1>=0 && j1<int(nj) && j1 >=0)
      {
        if (image(i1,j1,k) != boundary_label && image(i1,j1,k) != background_label)
        {
          image(i1,j1,k) = boundary_label;
          x_stack.push(i1);
          y_stack.push(j1);
        }
      }
    }
  }
}

//:  Reset background pixels to 0
void reset_background(vil3d_image_view<int> &image, int background_label)
{
  for (unsigned k = 0; k < image.nk(); k++)
  {
    for (unsigned j = 0; j < image.nj(); j++)
    {
      for (unsigned i = 0; i < image.ni(); i++)
      {
        if (image(i,j,k) == background_label)
          image(i,j,k) = 0;
      }
    }
  }
  return;
}
