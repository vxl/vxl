#include <vcsl/vcsl_axis.h>
// Once the compiler sees the primary template
// definition of vbl_smart_ptr<T>::ref() and unref()
// in vbl_smart_ptr.txx, it is no longer allowed to
// use a smart pointer of incomplete class as a data
// member in another class. So the above #include
// is not redundant, but necessary to satisfy the
// container requirements for std::vector<>. fsm.

#include <vcsl/vcsl_cartesian.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vcsl_cartesian);
