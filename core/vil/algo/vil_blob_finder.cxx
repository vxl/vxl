//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Tim Cootes

#include "vil_blob_finder.h"
#include <vil/algo/vil_trace_4con_boundary.h>

//: Default constructor
vil_blob_finder::vil_blob_finder()
  : i_(-1),j_(-1) {}

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
    int i = bi[k];
    while (i<ni && image(i,j)) { image(i,j)=false; ++i; }

    // Delete all neighbours to the left
    i=bi[k]-1;
    while (i>=0 && image(i,j)) { image(i,j)=false; --i; }
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
        delete_blob(image_,bi,bj);
        return true;
      }
    }
  }

  return false;  // Reached end of image without finding another blob
}

//: Get largest blob in current image
//  Assumes image has been initialised, and that next_4con_region not
//  yet called.  Erases internal image during this call, so any
//  subsequent calls will not work.
void vil_blob_finder::largest_4con_region(vcl_vector<int>& bi, vcl_vector<int>& bj)
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

