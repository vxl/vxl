// This is core/vpgl/xio/vpgl_xio_lvcs.h
#ifndef vpgl_xio_lvcs_h
#define vpgl_xio_lvcs_h
//:
// \file
// \brief XML write a vpgl_lvcs
//  Two functions which write a valid XML fragment to an ostream.
// - x_write() writes a single XML element node with text content only
//
// \author Andrew Miller
// \date 17 Nov, 2011
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <string>
#include <iosfwd>
#include <vpgl/vpgl_lvcs.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: XML save vpgl_lvcs to stream.
// \relatesalso vpgl_lvcs
void x_write(std::ostream & os, vpgl_lvcs const& v,
             std::string element_name="vpgl_lvcs");

#endif // vpgl_xio_lvcs_h
