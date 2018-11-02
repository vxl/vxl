//This is brl/bbas/volm/volm_spherical_index_query_matcher.h
#ifndef volm_spherical_index_query_matcher_h_
#define volm_spherical_index_query_matcher_h_
//:
// \file
// \brief A class to construct a match a query with an index
// Units are in meters
//
// \author Vishal Jain
// \date Feb 22, 2012
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vector>
#include <iostream>
#include <map>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_spherical_region_index.h>
#include <volm/volm_spherical_region_query.h>
#include <vsph/vsph_unit_sphere_sptr.h>
class volm_spherical_index_query_matcher
{
public:
    volm_spherical_index_query_matcher(volm_spherical_region_index & index,
                                       volm_spherical_region_query & query,
                                       volm_camera_space_sptr & cam_space);


    bool match();


private:

    volm_spherical_region_index & index_;
    volm_spherical_region_query & query_;
    volm_camera_space_sptr & cam_space_;
    std::vector<double> scores_;
};

#endif // volm_spherical_index_query_matcher_h_
