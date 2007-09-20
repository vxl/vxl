//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Tim Cootes

#include "vil_blob_finder.h"
#include "vil_trace_4con_boundary.h"
#include "vil_trace_8con_boundary.h"
#include "vil_flood_fill.h"

//: Default constructor
vil_blob_finder::vil_blob_finder()
  : i_((unsigned int)(-1)),j_((unsigned int)(-1)) {} // certainly outside image

//: Construct to work on image (takes deep copy)
vil_blob_finder::vil_blob_finder(const vil_image_view<bool>& image)
  : i_(0),j_(0)
{
  image_.deep_copy(image);
}

//: Define image to work on (deep copy taken)
void vil_blob_finder::set_image(const vil_image_view<bool>& image)
{
  i_=0; j_=0;
  image_.deep_copy(image);
}

//: Define image to work on and use as workspace
//  Image will be erased during process. Avoids the copy in set_image()
void vil_blob_finder::set_work_image(vil_image_view<bool>& image)
{
  i_=0; j_=0;
  image_ = image;
}

//: Delete region in image with boundary (bi,bj)
void delete_blob(vil_image_view<bool>& image,
                 const vcl_vector<int>& bi, const vcl_vector<int>& bj)
{
  const unsigned n = bi.size();
  const unsigned ni = image.ni();
  for (unsigned k=0;k<n;++k)
  {
    int j = bj[k];

    // Delete all neighbours to the right
    unsigned int i = bi[k];
    while (i<ni && image(i,j)) image(i++,j)=false;

    // Delete all neighbours to the left
    i=bi[k];
    while (i>0 && image(i-1,j)) image(--i,j)=false;
  }
}

//: Get boundary pixels of next blob in current image.
// Uses four connected boundary representation.
// Return false if no more regions
bool vil_blob_finder::next_4con_region(vcl_vector<int>& bi, vcl_vector<int>& bj)
{
  // Start from current pixel (i_,j_), run over rows until matching pixel found
  for (; j_<image_.nj(); ++j_,i_=0)
  {
    for (; i_<image_.ni(); ++i_)
    {
      if (image_(i_,j_))
      {
        vil_trace_4con_boundary(bi,bj,image_,i_,j_);
         // Delete blob by flood filling it with false
        vil_flood_fill4(image_,bi[0],bj[0],true,false);
        return true;
      }
    }
  }

  return false;  // Reached end of image without finding another blob
}

//: Get pixels of next blob in current image.
// Uses four connected boundary representation.
// Return false if no more regions
bool vil_blob_finder::next_4con_region(vcl_vector<vil_chord>& region)
{
  region.resize(0);
  // Start from current pixel (i_,j_), run over rows until matching pixel found
  for (; j_<image_.nj(); ++j_,i_=0)
  {
    for (; i_<image_.ni(); ++i_)
    {
      if (image_(i_,j_))
      {
         // Delete blob by flood filling it with false
        vil_flood_fill4(image_,i_,j_,true,false,region);
        return true;
      }
    }
  }

  return false;  // Reached end of image without finding another blob
}

//: Get pixels of next blob in current image.
// Uses 8 connected boundary representation.
// Return false if no more regions
bool vil_blob_finder::next_8con_region(vcl_vector<vil_chord>& region)
{
  region.resize(0);
  // Start from current pixel (i_,j_), run over rows until matching pixel found
  for (; j_<image_.nj(); ++j_,i_=0)
  {
    for (; i_<image_.ni(); ++i_)
    {
      if (image_(i_,j_))
      {
         // Delete blob by flood filling it with false
        vil_flood_fill8(image_,i_,j_,true,false,region);
        return true;
      }
    }
  }

  return false;  // Reached end of image without finding another blob
}

//: Get longest blob boundary in current image
//  Assumes image has been initialised, and that next_4con_region not
//  yet called.  Erases internal image during this call, so any
//  subsequent calls will not work.
void vil_blob_finder::longest_4con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj)
{
  bi.resize(0); bj.resize(0);
  vcl_vector<int> tmp_bi,tmp_bj;
  while (next_4con_region(tmp_bi,tmp_bj))
  {
    if (tmp_bi.size()>bi.size())
    {
      vcl_swap(bi,tmp_bi);
      vcl_swap(bj,tmp_bj);
    }
  }
}

//: Get number of blobs in given image
unsigned vil_blob_finder::n_4con_regions(const vil_image_view<bool>& image)
{
  set_image(image);
  unsigned n=0;
  vcl_vector<int> tmp_bi,tmp_bj;
  while (next_4con_region(tmp_bi,tmp_bj)) n++;
  return n;
}


//-----------------------------------------------------

//: Get boundary pixels of next blob in current image.
// Uses four connected boundary representation.
// Return false if no more regions
bool vil_blob_finder::next_8con_region(vcl_vector<int>& bi, vcl_vector<int>& bj)
{
  // Start from current pixel (i_,j_), run over rows until matching pixel found
  for (; j_<image_.nj(); ++j_,i_=0)
  {
    for (; i_<image_.ni(); ++i_)
    {
      if (image_(i_,j_))
      {
        vil_trace_8con_boundary(bi,bj,image_,i_,j_);
         // Delete blob by flood filling it with false
        vil_flood_fill8(image_,bi[0],bj[0],true,false);
        return true;
      }
    }
  }

  return false;  // Reached end of image without finding another blob
}

//: Get longest blob boundary in current image
//  Assumes image has been initialised, and that next_8con_region not
//  yet called.  Erases internal image during this call, so any
//  subsequent calls will not work.
void vil_blob_finder::longest_8con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj)
{
  bi.resize(0); bj.resize(0);
  vcl_vector<int> tmp_bi,tmp_bj;
  while (next_8con_region(tmp_bi,tmp_bj))
  {
    if (tmp_bi.size()>bi.size())
    {
      vcl_swap(bi,tmp_bi);
      vcl_swap(bj,tmp_bj);
    }
  }
}

//: Get largest blob region in current image
unsigned vil_blob_finder::largest_8con_region(vcl_vector<vil_chord>& region)
{
  region.resize(0); 
  vcl_vector<vil_chord> tmp_region;
  unsigned max_area=0;
  while (next_8con_region(tmp_region))
  {
    unsigned area = vil_area(tmp_region);
    if (area>max_area)
    {
      vcl_swap(region,tmp_region);
      max_area=area;
    }
  }
  return max_area;
}

//: Get largest blob region in current image
unsigned vil_blob_finder::largest_4con_region(vcl_vector<vil_chord>& region)
{
  region.resize(0); 
  vcl_vector<vil_chord> tmp_region;
  unsigned max_area=0;
  while (next_4con_region(tmp_region))
  {
    unsigned area = vil_area(tmp_region);
    if (area>max_area)
    {
      vcl_swap(region,tmp_region);
      max_area=area;
    }
  }
  return max_area;
}

//: Get number of blobs in given image
unsigned vil_blob_finder::n_8con_regions(const vil_image_view<bool>& image)
{
  set_image(image);
  unsigned n=0;
  vcl_vector<int> tmp_bi,tmp_bj;
  while (next_8con_region(tmp_bi,tmp_bj)) n++;
  return n;
}


