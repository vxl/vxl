#include "boxm2_detect_change_blobs.h"
#include "boxm2_point_util.h"
//:
// \file
#include <bil/algo/bil_blob_finder.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_threshold.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


float boxm2_change_blob::percent_overlap(boxm2_change_blob& blob)
{
  //find number of matching pairs, divided by this area's size
  std::vector<PairType> other = blob.get_pixels();
  float numOverlap = 0.0f;
  for (auto & pixel : pixels_) {
    for (auto j : other) {
      if (pixel == j) {
        ++numOverlap;
        break;
      }
    }
  }

  //return percent
  return numOverlap / pixels_.size();
}


//: Main boxm2_detect_change_blobs function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_detect_change_blobs(vil_image_view<float>& change,
                                    float thresh,
                                    std::vector<boxm2_change_blob>& blobs)
{
  //threshold
  vil_image_view<bool> bool_image(change.ni(), change.nj());
  vil_threshold_above<float>( change, bool_image, thresh );
  vil_image_view<bool> imga(change.ni(), change.nj()),
                       imgb(change.ni(), change.nj());

  //dilate and erode once
  boxm2_util_dilate_erode(bool_image, imga);

  //find blobs
  bil_blob_finder finder(imga);

  //blob region is just a vector of vil_chords (rows in image)
  std::vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //new blob
    boxm2_change_blob blob;
    //go over each row
    std::vector<vil_chord>::iterator iter;
    for (iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        blob.add_pixel( PairType(i, iter->j) );
    }

    //Only add blobs smaller than 400 pixels
    blobs.push_back(blob);
  }
#if 0
  std::cout<<"Found "<<blobs.size()<<" blobs"<<std::endl;
#endif
}

//: Detects blobs given a bool image
void boxm2_util_detect_blobs(const vil_image_view<bool>& imgIn,
                             std::vector<boxm2_change_blob>& blobs)
{
  //find blobs
  bil_blob_finder finder(imgIn);

  //blob region is just a vector of vil_chords (rows in image)
  std::vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //new blob
    boxm2_change_blob blob;
    //go over each row
    std::vector<vil_chord>::iterator iter;
    for (iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for (unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        blob.add_pixel( PairType(i, iter->j) );
    }
    blobs.push_back(blob);
  }
}

void boxm2_util_dilate_erode(const vil_image_view<bool>& imgIn,
                                   vil_image_view<bool>& imgOut)
{
  vil_image_view<bool> dest(imgIn.ni(), imgIn.nj());

  //remove singletons
  boxm2_util_remove_singletons(imgIn, imgOut);

  //do a bunch of dilates
  int numDilates = 1;
  for(int i=0; i<numDilates; ++i) {

    //dilate image
    vil_structuring_element selem;
    selem.set_to_disk(1.05);
    vil_binary_dilate(imgOut, dest, selem);

    //erode image
    selem.set_to_disk(1.05);
    vil_binary_erode(dest, imgOut, selem);
  }

  //int numErodes = 3;
  //for(int i=0; i<numErodes; ++i) {

    ////erode image
    //vil_structuring_element selem;
    //selem.set_to_disk(1.05);
    //vil_binary_erode(imgOut, dest, selem);

    ////erode image
    //selem.set_to_disk(1.05);
    //vil_binary_erode(dest, imgOut, selem);
  //}

}

//removes pixels that are surrounded by removed pixels
void boxm2_util_remove_singletons(const vil_image_view<bool>& imgIn,
                                        vil_image_view<bool>& imgOut)
{
  assert(imgIn.ni() == imgOut.ni() && imgIn.nj() == imgOut.nj());
  for (unsigned int i=0; i<imgIn.ni(); ++i) {
    for (unsigned int j=0; j<imgIn.nj(); ++j)
    {
      //if this guy is single, make him false
      imgOut(i,j) = false;

      //check up and down neighbors
      if (imgIn(i,j))
      {
        //up
        if (i > 1 && imgIn(i-1,j))
          imgOut(i,j) = true;
        //down
        else if (i+1 < imgIn.ni() && imgIn(i+1,j))
          imgOut(i,j) = true;
        //left
        else if (j > 1 && imgIn(i,j-1))
          imgOut(i,j) = true;
        //right
        else if (j+1 < imgIn.nj() && imgIn(i,j+1))
          imgOut(i,j) = true;
      }
    }
  }
}


void boxm2_util_blob_to_image(std::vector<boxm2_change_blob>& blobs,
                              vil_image_view<vxl_byte>& imgOut)
{
  //create a blob image
  imgOut.fill(0);
  std::vector<boxm2_change_blob>::iterator iter;
  for (iter=blobs.begin(); iter!=blobs.end(); ++iter) {
    //paint each blob pixel white
    for (unsigned int p=0; p<iter->blob_size(); ++p) {
      PairType pair = iter->get_pixel(p);
      imgOut( pair.x(), pair.y() ) = (vxl_byte) 255;
    }
  }
}
