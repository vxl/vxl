// Instantiation of vcl_list<HomgLineSeg2D>
#include <mvl/HomgLineSeg2D.h>
#include <vcl_list.txx>
bool operator<(HomgLineSeg2D const&, HomgLineSeg2D const&) { return false; }
VCL_LIST_INSTANTIATE(HomgLineSeg2D);
