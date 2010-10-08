#include <vsph/vsph_view_sphere.txx>
#include <vsph/vsph_view_point.h>
#include <icam/icam_view_metadata.h>

typedef vsph_view_point<icam_view_metadata> type;

VSPH_VIEW_SPHERE_INSTANTIATE(type);
