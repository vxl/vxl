#ifndef vbl_binary_h_
#define vbl_binary_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_binary.h

//:
// \file
// \brief Save and load vcl_vector<T>s in binary format
// \author fsm@robots.ox.ac.uk
// \deprecated Use vsl instead
//
// \verbatim
// Modifications
// PDA (Manchester) 23/03/2001: Tidied up the documentation
// IMS (Manchester) 25/06/2001: deprecated in favour of vsl
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vcl_deprecated_header.h>

template </*typename*/class T> void vbl_binary_save(vcl_ostream &,
                                                    vcl_vector<T> const &);
template </*typename*/class T> void vbl_binary_load(vcl_istream &,
                                                    vcl_vector<T> &);

// [23.2.5] in a standard library, vector<bool> is specialized. By declaring
// the specializations here, we also stop people from trying to instantiate
// them.
VCL_DEFINE_SPECIALIZATION void vbl_binary_save(vcl_ostream &,
                                               vcl_vector<bool> const &);
VCL_DEFINE_SPECIALIZATION void vbl_binary_load(vcl_istream &,
                                               vcl_vector<bool> &);

#endif // vbl_binary_h_
