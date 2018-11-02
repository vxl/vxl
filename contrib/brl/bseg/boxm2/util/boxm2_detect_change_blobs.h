#ifndef boxm2_detect_change_blobs_h
#define boxm2_detect_change_blobs_h
//:
// \file
#include <vector>
#include <iostream>
#include <cstddef>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: pixel pair-i,j value
typedef vgl_point_2d<int> PairType;

//: blob class for boxm2 change blob - just a holder for blob, no bound checks are made
class boxm2_change_blob
{
  public:
    //: store vectors and init currPixel
    boxm2_change_blob() : currPix_(0) {}

    //: add pixel
    void        add_pixel( PairType pair ) { pixels_.push_back(pair); }
    PairType    get_pixel( unsigned idx )  { return pixels_[idx]; }
    PairType    next_pixel()               { return pixels_[currPix_++]; }
    std::size_t  blob_size()                { return pixels_.size(); }

    //: getter
    std::vector<PairType>& get_pixels()     { return pixels_; }

    //: percent overlap from of THIS blob (intersection / this blob area)
    float       percent_overlap(boxm2_change_blob& blob);

  private:
    std::vector<PairType> pixels_;
    int                  currPix_;
};


//: Main boxm2_detect_change_blobs function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_detect_change_blobs(vil_image_view<float>& change,
                                    float thresh,
                                    std::vector<boxm2_change_blob>& blobs);

//: Detects blobs given a bool image
void boxm2_util_detect_blobs(const vil_image_view<bool>& imgIn,
                             std::vector<boxm2_change_blob>& blobs);

//: utility method to remove single pixels from bool image
void boxm2_util_remove_singletons(const vil_image_view<bool>& imgIn,
                                        vil_image_view<bool>& imgOut);

void boxm2_util_dilate_erode(const vil_image_view<bool>& imgIn,
                                   vil_image_view<bool>& imgOut);


void boxm2_util_blob_to_image( std::vector<boxm2_change_blob>& blobs,
                               vil_image_view<vxl_byte>& imgOut);



#endif // boxm2_detect_change_blobs_h
