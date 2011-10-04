#include "boxm2_detect_change_blobs.h"
#include "boxm2_point_util.h"
//:
// \file
#include <bil/algo/bil_blob_finder.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_threshold.h>
#include <vil/vil_save.h>


float boxm2_change_blob::percent_overlap(boxm2_change_blob& blob)
{
  //find number of matching pairs, divided by this area's size
  vcl_vector<PairType> other = blob.get_pixels(); 
  float numOverlap = 0.0f; 
  for(int i=0; i<pixels_.size(); ++i) {
    for(int j=0; j<other.size(); ++j) {
      if(pixels_[i] == other[j]) {
        numOverlap++; 
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
                                    vcl_vector<boxm2_change_blob>& blobs)
{
  //threshold
  vil_image_view<bool> bool_image(change.ni(), change.nj()); 
  vil_threshold_above<float>( change, bool_image, thresh ); 
  vil_image_view<bool> imga(change.ni(), change.nj()), 
                       imgb(change.ni(), change.nj()); 
  
  //remove singletons
  boxm2_util_remove_singletons(bool_image, imga); 

  //dilate image
  vil_structuring_element selem;
  selem.set_to_disk(1.05);  
  vil_binary_dilate(imga, imgb, selem);
  
  //erode image
  selem.set_to_disk(1.05); 
  vil_binary_erode(imgb, imga, selem); 

  //find blobs
  bil_blob_finder finder(imga);

  //blob region is just a vector of vil_chords (rows in image)
  vcl_vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //new blob
    boxm2_change_blob blob;
    //go over each row
    vcl_vector<vil_chord>::iterator iter; 
    for(iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for(unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        blob.add_pixel( PairType(i, iter->j) );  
    }
    blobs.push_back(blob); 
  }
  
  vcl_cout<<"Found "<<blobs.size()<<" blobs"<<vcl_endl;
}

//: Detects blobs given a bool image
void boxm2_util_detect_blobs(const vil_image_view<bool>& imgIn, 
                             vcl_vector<boxm2_change_blob>& blobs)
{
  //find blobs
  bil_blob_finder finder(imgIn);

  //blob region is just a vector of vil_chords (rows in image)
  vcl_vector<vil_chord> region;
  while (finder.next_4con_region(region))
  {
    //new blob
    boxm2_change_blob blob;
    //go over each row
    vcl_vector<vil_chord>::iterator iter; 
    for(iter=region.begin(); iter!=region.end(); ++iter) {
      //add each pixel in this row to blob
      for(unsigned i=iter->ilo; i<iter->ihi+1; ++i)
        blob.add_pixel( PairType(i, iter->j) );  
    }
    blobs.push_back(blob); 
  }
}

//removes pixels that are surrounded by 
void boxm2_util_remove_singletons(const vil_image_view<bool>& imgIn, 
                                        vil_image_view<bool>& imgOut)
{
  for(int i=0; i<imgIn.ni(); ++i) {
    for(int j=0; j<imgIn.nj(); ++j) {
      
      if( imgIn(i,j) ){

        //check up and down neighbors
        int ni, nj; 
        bool isSingle = true; 
        
        //up
        ni=i-1, nj=j; 
        if(ni > 0 && imgIn(ni,nj)) 
          isSingle = false; 
        
        //down
        ni=i+1, nj=j; 
        if(ni < imgIn.ni() && imgIn(ni,nj))
          isSingle = false; 
        
        //left
        nj=j-1, ni=i; 
        if(nj > 0 && imgIn(ni,nj))
          isSingle = false; 
        
        //right
        nj=j+1, ni=i;
        if(nj < imgIn.nj() && imgIn(ni,nj))
          isSingle = false; 
        
        //if this guy is single, make him false
        imgOut(i,j) = (isSingle) ? false : true; 
      }
      else {
        imgOut(i,j) = false; 
      }
      
    }
  }
}
