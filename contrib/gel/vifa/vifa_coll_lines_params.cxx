// This is gel/vifa/vifa_coll_lines_params.cxx

#include <vcl_iostream.h>
#include <vifa/vifa_coll_lines_params.h>


vifa_coll_lines_params::
vifa_coll_lines_params(float midpt_distance,
                       float angle_tolerance,
                       float discard_threshold,
                       float endpt_distance) :
  _midpt_distance(midpt_distance),
  _angle_tolerance(angle_tolerance),
  _discard_threshold(discard_threshold),
  _endpt_distance(endpt_distance)
{
  // Do nothing
}

vifa_coll_lines_params::
vifa_coll_lines_params(const vifa_coll_lines_params& old_params)
{
  _midpt_distance = old_params._midpt_distance;
  _angle_tolerance = old_params._angle_tolerance;
  _discard_threshold = old_params._discard_threshold;
  _endpt_distance = old_params._endpt_distance;
}

void vifa_coll_lines_params::
print_info(void)
{
  vcl_cout << "vifa_coll_lines_params:\n"
           << "  midpoint distance = " << _midpt_distance << vcl_endl
           << "  angle tolerance   = " << _angle_tolerance << vcl_endl
           << "  discard threshold = " << _discard_threshold << vcl_endl
           << "  endpoint distance = " << _endpt_distance << vcl_endl;
}
