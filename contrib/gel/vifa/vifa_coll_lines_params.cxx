// This is gel/vifa/vifa_coll_lines_params.cxx

#include <vcl_iostream.h>
#include <vifa/vifa_coll_lines_params.h>


vifa_coll_lines_params::
vifa_coll_lines_params(float midpt_distance,
                       float angle_tolerance,
                       float discard_threshold,
                       float endpt_distance) :
  midpt_distance_(midpt_distance),
  angle_tolerance_(angle_tolerance),
  discard_threshold_(discard_threshold),
  endpt_distance_(endpt_distance)
{
  // Do nothing
}

vifa_coll_lines_params::
vifa_coll_lines_params(const vifa_coll_lines_params& old_params)
  : gevd_param_mixin(), vul_timestamp(), vbl_ref_count()
{
  midpt_distance_ = old_params.midpt_distance_;
  angle_tolerance_ = old_params.angle_tolerance_;
  discard_threshold_ = old_params.discard_threshold_;
  endpt_distance_ = old_params.endpt_distance_;
}

void vifa_coll_lines_params::
print_info(void)
{
  vcl_cout << "vifa_coll_lines_params:\n"
           << "  midpoint distance = " << midpt_distance_ << vcl_endl
           << "  angle tolerance   = " << angle_tolerance_ << vcl_endl
           << "  discard threshold = " << discard_threshold_ << vcl_endl
           << "  endpoint distance = " << endpt_distance_ << vcl_endl;
}
