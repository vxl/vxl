#include "rgrl_matcher.h"

#include "rgrl_view.h"

rgrl_matcher::
~rgrl_matcher()
{
}

rgrl_match_set_sptr
rgrl_matcher::
compute_matches( rgrl_feature_set const&    from_features,
                 rgrl_feature_set const&    to_features,
                 rgrl_view const&           current_view,
                 rgrl_scale const&          current_scale ) const
{
  return compute_matches( from_features, to_features, current_view,
                          *current_view.xform_estimate(), current_scale );
}

rgrl_match_set_sptr
rgrl_matcher::
compute_matches( rgrl_feature_set const&    from_features,
                 rgrl_feature_set const&    to_features,
                 rgrl_transformation const& current_xform,
                 rgrl_mask_box const&       region,
                 rgrl_scale const&          current_scale ) const
{
  rgrl_view view( region, region, region, region, 0, 0, 0);

  return this->compute_matches(from_features,
                               to_features,
                               view,
                               current_xform,
                               current_scale);
}
