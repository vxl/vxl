// Instantiation of vcl_list<HomgPoint2D>
#include <mvl/HomgPoint2D.h>
#include <vcl_list.txx>
bool operator<(HomgPoint2D const&, HomgPoint2D const&) { return false; }
VCL_LIST_INSTANTIATE(HomgPoint2D);
