//:
// \file
// \brief Fill in contour bounded regions in slices of 3D image
// \author Kola Babalola

#include <vil3d/algo/vil3d_fill_boundary.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_stack.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/algo/vil3d_threshold.h>
#include <vcl_fstream.h>

//: Compute a mask where the regions in each slice of a 3D
//  image bounded by contours are set to "on"

void vil3d_fill_boundary(vil3d_image_view<bool>& bool_image)
{
  unsigned ni = bool_image.ni();
  unsigned nj = bool_image.nj();
  unsigned nk = bool_image.nk();
  unsigned nplanes = bool_image.nplanes();

  // input image is binary, converted to int (need to speak to tim about data types accepted by threshold
  vil3d_image_view<int> image(ni,nj,nk,nplanes);
  vil3d_convert_cast(bool_image,image);
  
  vcl_ptrdiff_t istep = image.istep(), jstep = image.jstep(), kstep = image.kstep();

  // scan the image and look for a boundary pixel
  int *page0 = image.origin_ptr();

  int *p0 = page0;

  int boundary_label;
  int background_label = 2;
  for (int k = 0; k < nk; k++)
  {
    boundary_label = 3;
    for(int j = 0; j < nj; j++)
    {
      for(int i = 0; i < ni; i++, p0+=istep)
      {
        if (*p0 == 1)
        {
          label_boundary_and_bkg(image,p0,boundary_label,background_label);
          fill_boundary(image,p0,boundary_label,background_label,j,i);
          boundary_label+=1;
        }
      }
    }
  }

  // threshold the image to obtain the binary mask
  vil3d_threshold_above(image,bool_image,3);
}

//:  Follow the current boundary in the current slice 
//  labelling boundary pixels and background pixels  
//  that border the boundary.
void label_boundary_and_bkg(vil3d_image_view<int> &image, int *p, int boundary_label, int background_label)
{

  vcl_ptrdiff_t istep = image.istep(), jstep = image.jstep();
  vcl_vector<vcl_ptrdiff_t> neighbourhood(8);
  vcl_vector<int> next_dir(8);

  // assign the offsets for the 8-neighbourhood
  neighbourhood[0] = -istep;
  neighbourhood[1] = -istep-jstep;
  neighbourhood[2] = -jstep;
  neighbourhood[3] =  istep-jstep;
  neighbourhood[4] =  istep;
  neighbourhood[5] =  istep+jstep;
  neighbourhood[6] =  jstep;
  neighbourhood[7] = -istep+jstep;

  // assign the offsets for the next direction to look in
  next_dir[0] = next_dir[1] = 7;//-istep+jstep;
  next_dir[2] = next_dir[3] = 1;//-istep-jstep;
  next_dir[4] = next_dir[5] = 3;//istep-jstep;
  next_dir[6] = next_dir[7] = 5;//istep+jstep;

  int i,offset=0;
  int *p1, *p0 = p;
  int counter = 0;

  // check the 8-neighbourhood of the pixels on the boundary
  while (*p0 != boundary_label || p != p0)
  {
    *p0 = boundary_label;
    counter++;
    
    for(i = 0; i < 8; i++)
    {
      p1 = p0+neighbourhood[offset];
      if((*(p1) == 1) || (*(p1) == boundary_label))  // this means we visit some pixels
      {                        // more than once. needed for degenerate cases
        p0 = p1;
        offset = next_dir[offset];
        break;
      }
      else
      {
        *(p1) = background_label;
        offset = ++offset%8;
      }
    }
  }
}


//:  Fill interior of current boundary.
void fill_boundary(vil3d_image_view<int> &image, int *p0, int boundary_label, int background_label,
           int row, int col)
{
  vcl_stack<int*> pixel_stack;
  int i;

  vcl_ptrdiff_t istep = image.istep(), jstep = image.jstep();
  vcl_vector<vcl_ptrdiff_t> neighbourhood(8);

  // push all boundary pixels onto stack. Needed for degenerate cases
  for (;row<image.nj();row++,col=0)
    for (;col<image.ni();col++,p0+=istep)
    {
      if (*p0 == boundary_label)
        pixel_stack.push(p0);
    }

  // assign the offsets for the 8-neighbourhood
  neighbourhood[0] = -istep;
  neighbourhood[1] = -istep-jstep;
  neighbourhood[2] = -jstep;
  neighbourhood[3] =  istep-jstep;
  neighbourhood[4] =  istep;
  neighbourhood[5] =  istep+jstep;
  neighbourhood[6] =  jstep;
  neighbourhood[7] = -istep+jstep;

  int *p1;

  //pixel_stack.push(p0);
  while(!pixel_stack.empty())
  {
    p0 = pixel_stack.top();
    pixel_stack.pop();
    for(i = 0;i < 8;i++)
    {
      p1 = p0+neighbourhood[i];
      if(*p1 != boundary_label && *p1 != background_label)
      {
        *p1 = boundary_label;
        pixel_stack.push(p1);
      }
    }
  }
}
