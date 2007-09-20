#ifndef vil_blob_finder_h_
#define vil_blob_finder_h_
//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>

//: Finds connected regions in a boolean image
//  Class to iterate through image, finding boundaries of all regions
//  (connected true pixels).
//
// Can deal with both 4-connected and 8-connected regions.
//
//  Note that the usual use of the class copies the boolean image input,
//  then erases the true pixels as it goes through finding the blobs (in
//  order to avoid finding the same blob twice).  If the image is expendable,
//  then the deep copy can be avoided using set_work_image(im).  In this
//  case the supplied image (im) will be erased as the blob finder goes
//  about it's humble task.
//
//  Where a blob has holes, only the outside of the blob is located.
//  The algorithm will locate nested blobs (ie a blob within a hole
//  inside another blob).
//
//  \code
//  vcl_vector<int> bi,bj;
//  vil_blob_finder finder(bool_image);
//  while (finder.next_4con_region(bi,bj))
//  {
//    vcl_cout<<"Blob boundary length: "<<bi.size()<<vcl_endl;
//  }
//  \endcode
class vil_blob_finder
{
 private:
  //: Workspace (may be view of external image)
  vil_image_view<bool> image_;

  //: Current point of interest
  unsigned int i_,j_;

 public:
  //: Default constructor
  vil_blob_finder();

  //: Construct to work on image (takes deep copy)
  vil_blob_finder(const vil_image_view<bool>& image);

  //: Define image to work on (deep copy taken)
  void set_image(const vil_image_view<bool>& image);

  //: Define image to work on and use as workspace
  //  Image will be erased during process.
  //  Avoids the deep copy in set_image()
  void set_work_image(vil_image_view<bool>& image);

  //: Get boundary pixels of next blob in current image.
  //  Uses four connected boundary representation.
  //  Return false if no more regions
  bool next_4con_region(vcl_vector<int>& bi, vcl_vector<int>& bj);

  //: Get longest blob boundary in current image
  //  Assumes image has been initialised, and that next_4con_region not
  //  yet called.  Erases internal image during this call, so any
  //  subsequent calls will not work.
  //
  //  bi,bj empty if no blobs found.
  void longest_4con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj);

  //: Get number of blobs in given image
  //  Overrides any internal state
  unsigned n_4con_regions(const vil_image_view<bool>& image);

  //: Get boundary pixels of next blob in current image.
  //  Uses four connected boundary representation.
  //  Return false if no more regions
  bool next_8con_region(vcl_vector<int>& bi, vcl_vector<int>& bj);

  //: Get longest blob boundary in current image
  //  Assumes image has been initialised, and that next_8con_region not
  //  yet called.  Erases internal image during this call, so any
  //  subsequent calls will not work.
  //
  //  bi,bj empty if no blobs found.
  void longest_8con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj);

  //: Get number of blobs in given image
  //  Overrides any internal state
  unsigned n_8con_regions(const vil_image_view<bool>& image);
};

#endif
