// This is core/vil2/vil2_property.h
#ifndef vil2_property_h_
#define vil2_property_h_
//:
// \file
//
// There is no class or function called vil2_property.
//
// The image class vil2_image_resource has the method :
// \code
//   bool get_property(char const *tag, void *property_value = 0) const;
// \endcode
// which allow format extensions to be added without cluttering the
// interface to vil2_image_view. The idea is that properties can be
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
// begin with "vil2_property_" and that chunk of the namespace is reserved.
//
// \author  fsm

//: Indicate whether this is an in-memory image or an on-disk image
//  By default an image is not in-memory, and since this is a boolean property
//  the return value of get_property(), which is "false" by default, will be
//  correct.  Only in-memory images must implement this property, and return
//  "true".
#define vil2_property_memory "memory"

//: Indicate that you can't call put_view on this image.
//  By default an image is not read-only, and since this is a boolean property
//  the return value of get_property(), which is "false" by default, will be
//  correct.  Only images which do not allow put_view must implement this
//  property, and return "true".
#define vil2_property_read_only "read-only"

#endif // vil2_property_h_
