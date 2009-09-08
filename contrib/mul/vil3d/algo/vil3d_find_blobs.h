// This is contrib/mul/vil3d/algo/vil3d_find_blobs.h

#ifndef vil3d_find_blobs_h_
#define vil3d_find_blobs_h_


//:
// \file
// \brief Identify and enumerate all disjoint blobs in a binary image.
// \author Ian Scott, Kevin de Souza


#include <vil3d/vil3d_fwd.h>


//: Specify 6- or 26- neighbour connectivity
enum vil3d_find_blob_connectivity
{
  vil3d_find_blob_connectivity_6_conn,
  vil3d_find_blob_connectivity_26_conn
};


//: \brief Identify and enumerate all disjoint blobs in a binary image.
// \param src Input binary image, where false=background, true=foreground.
// \retval dst Output int image; 0=background, and 1...n are labels for each disjoint blob.
void vil3d_find_blobs(const vil3d_image_view<bool>& src,
                      vil3d_find_blob_connectivity conn,
                      vil3d_image_view<unsigned>& dst);


#endif // vil3d_find_blobs_h_
