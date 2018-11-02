#ifndef vil_blob_h_
#define vil_blob_h_
//:
// \file
// \brief Finds connected regions in a boolean image.
// \author Ian Scott
// These functions can be combined to get blobs as images, chorded regions, edge pixels lists, etc.
// For example to get an edge pixel list of im_binary
// \code
// vil_blob_labels(im_binary, vil_blob_4_conn, im_labels);
// vil_blob_labels_to_edge_labels(im_binary, vil_blob_4_conn, im_labels);
// vil_blob_labels_to_pixel_lists(im_edges, pixel_lists);
// \endcode
// To get the area of a blob efficiently
// \code
// vil_blob_labels(im_binary, vil_blob_4_conn, im_labels);
// vil_blob_labels_to_regions(im_labels, regions);
// unsigned area_of_first_blob = vil_area(regions[0]);
// \endcode

#include <vector>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_chord.h>

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

//: Set all non-blob-edge pixels in a blob label image to zero.
// A 4-conn edge is a 4-conn area itself, and not just those pixels which have dissimilar
// 4-conn neighbours.
void vil_blob_labels_to_edge_labels(const vil_image_view<unsigned>& src_label,
                                    vil_blob_connectivity conn,
                                    vil_image_view<unsigned>& dest_label);


//: A region is a vector of chords that came from a connected blob.
typedef std::vector<vil_chord> vil_blob_region;

//: Convert a label image into a list of chorded regions.
// A blob label value of n will be returned in dest_regions[n-1].
void vil_blob_labels_to_regions(const vil_image_view<unsigned>& src_label,
                                std::vector<vil_blob_region>& dest_regions);


//: A pixel list is a vector of <i,j> pixel positions.
typedef std::vector<std::pair<unsigned, unsigned> > vil_blob_pixel_list;

//: Convert a label image into a list of pixel lists.
// A blob label value of n will be returned in dest_pixels_lists[n-1].
void vil_blob_labels_to_pixel_lists(const vil_image_view<unsigned>& src_label,
                                    std::vector<vil_blob_pixel_list>& dest_pixel_lists);

#endif
