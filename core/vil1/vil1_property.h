#ifndef vil_property_h_
#define vil_property_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// There is no class called vil_property.
//
// The image class vil_image has the methods :
//
//   bool get_property(char const *tag, void *property_value = 0);
//   bool set_property(char const *tag, void const *property_value = 0) 
//
// which allow format extensions to be added without cluttering the
// interface to vil_image. The idea is that properties can be 
// identified by a "tag" (some name or other textual description) 
// through which clients can obtain or manipulate extra properties.
//
// A false return value means that the underlying image does not
// understand the given property or that the given data was invalid.
// A true return value means it does understand the property and has
// used the supplied data according to the relevant protocol.
// Passing a null pointer as the second argument can be useful for
// protocols for manipulating boolean properties (i.e when there is
// no data to be passed).
//
// To make this work in practice, it is necessary to avoid name clashes 
// and to make sure everyone agrees on the meaning of the property data.
// That is the purpose of this file. The set of tags is a namespace in
// the general sense of the word. We only have one namespace, so try 
// not to clutter it. All property tags described in this file should
// begin with "vil_property_" and that chunk of the namespace is reserved.

//: preferred direction for access.
// tags: "vil_property_preferred_x_direction"
//       "vil_property_preferred_y_direction"
// type: int
// If supported, the returned property value is
//  -1 if the preferred direction is decreasing.
//   0 if there is no preferred direction.
//  +1 if the preferred direction is increasing.

#endif
