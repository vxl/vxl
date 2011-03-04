#ifndef vil_blob_h_
#define vil_blob_h_
//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Ian Scott

#include <vil/vil_image_view.h>
#include <vil/vil_chord.h>
#include <vcl_vector.h>

//: Specify 4- or 8- neighbour connectivity
enum vil_blob_connectivity
{
  vil_blob_4_conn,
  vil_blob_8_conn
};


//: Produce a label image that enumerates all disjoint blobs in a binary image
void vil_blob_labels(const vil_image_view<bool>& src_binary,
                     vil_blob_connectivity conn,
                     vil_image_view<unsigned>& dest_label);

//: A region is a set of chords that came from a connected blob.
typedef vcl_vector<vil_chord> vil_blob_region;

//: Produce a list of regions label image that enumerates all disjoint blobs in a binary image
void vil_blob_labels_to_regions(const vil_image_view<unsigned>& src_label,
                                vcl_vector<vil_blob_region>& dest_regions);

#endif
