// Instantiation of vcl_list<HomgLineSeg2D*>
#include <vcl_list.txx>
#include <mvl/HomgLineSeg2D.h>

static bool operator<(HomgLineSeg2D const&, HomgLineSeg2D const&) { return false; }
VCL_LIST_INSTANTIATE(HomgLineSeg2D);
