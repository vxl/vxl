// This is core/vil/vil_property.h
#ifndef vil_property_h_
#define vil_property_h_
// :
// \file
//
// There is no class or function called vil_property.
//
// The image class vil_image_resource has the method :
// \code
//   bool get_property(char const *tag, void *property_value = 0) const;
// \endcode
// which allow format extensions to be added without cluttering the
// interface to vil_image_resource. The idea is that properties can be
// identified by a "tag" (some name or other textual description)
// through which clients can obtain or manipulate extra properties.
//
// A false return value means that the underlying image does not
// understand the given property or that the given data was invalid.
// A true return value means it does understand the property and has
// used the supplied data according to the relevant protocol.
// Passing a null pointer as the second argument can be useful for
// protocols for manipulating boolean properties (i.e. when there is
// no data to be passed).
//
// To make this work in practice, it is necessary to avoid name clashes
// and to make sure everyone agrees on the meaning of the property data.
// That is the purpose of this file. The set of tags is a namespace in
// the general sense of the word. We only have one namespace, so try
// not to clutter it. All property tags described in this file should
// begin with "vil_property_" and that chunk of the namespace is reserved.
//
// \author  fsm

// : Indicate whether this is an in-memory image or an on-disk image
//  By default an image is not in-memory, and since this is a boolean property
//  the return value of get_property(), which is "false" by default, will be
//  correct.  Only in-memory images must implement this property, and return
//  "true".
#define vil_property_memory "memory"

// : Indicate that you can't call put_view on this image.
//  By default an image is not read-only, and since this is a boolean property
//  the return value of get_property(), which is "false" by default, will be
//  correct.  Only images which do not allow put_view must implement this
//  property, and return "true".
#define vil_property_read_only "read-only"

// : Pixel width in metres.
// Strictly this is the pixel spacing, and not some function of
// the sensor's spatial sampling kernel.
// Type is float[2].
#define vil_property_pixel_size "pixel_size"

// : Original image origin in pixels.
// Measured right from left edge, and down from top - i.e. in i and j directions
// Type is float[2].
#define vil_property_offset "offset"

// : The quantisation depth of pixel components.
// This is the maximum information per pixel component. (Bear in mind that particular image
// may not even be using all of it.) e.g. an image with true vil_rgb<vxl_byte> style
// pixels would return 8. If a file image has this property implemented, and purports to
// supply an unsigned-type you can assume that it will give you pixels valued between
// 0 and 2^quantisation_depth - 1.
// Type is unsigned int.
#define vil_property_quantisation_depth "quantisation_depth"

// : For unblocked images, the following properties are not implemented.
//  It is assumed that all blocks are the same size and padded with zeros if
//  necessary. Thus, n_block_i = (ni() + size_block_i - 1)/size_block_i and
//  n_block_j = (nj() + size_block_j - 1)/size_block_j. Both properties must
//  be implemented for blocked images.  Type is unsigned int.

// : Block size in columns
#define vil_property_size_block_i "size_block_i"

// : Block size in rows
#define vil_property_size_block_j "size_block_j"

// : true if image resource is a pyramid image
#define vil_property_pyramid "pyramid"

#endif // vil_property_h_
