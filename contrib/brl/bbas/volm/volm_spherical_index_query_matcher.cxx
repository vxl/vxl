#include <volm/volm_spherical_index_query_matcher.h>
#include <vsph/vsph_sph_box_2d.h>

volm_spherical_index_query_matcher::volm_spherical_index_query_matcher(volm_spherical_region_index & index,
                                                                       volm_spherical_region_query & query,
                                                                       volm_camera_space_sptr & cam_space)
: index_(index),query_(query), cam_space_(cam_space)
{
}

bool volm_spherical_index_query_matcher::match()
{
    volm_spherical_regions_layer index_layer = index_.index_regions();
    std::vector<volm_spherical_region> i_regions = index_layer.regions();
    for (camera_space_iterator iter = cam_space_->begin(); iter != cam_space_->end(); ++iter)
    {
        cam_angles camera = iter->camera_angles();
        unsigned roll_index;
        unsigned fov_index;
        unsigned head_index;
        unsigned tilt_index;
        iter->cam_indices(roll_index,fov_index,head_index,tilt_index);

        double score = 0.0;
        volm_spherical_regions_layer q_regions =query_.query_regions(roll_index);
        for (int i = 0; i< q_regions.size(); ++i)
        {
            // transform query_region box
            volm_spherical_region query_region = q_regions.regions()[i];
            unsigned char qval = 0;
            if (!query_region.attribute_value(ORIENTATION,qval))
                continue;
            vsph_sph_box_2d qbox = query_region.bbox_ref();
            // convert google coordinate axis( z is down and x is north) to spherical coordinate system ( z is up and x is east)
            vsph_sph_box_2d qbox_xfomred = qbox.transform(camera.tilt_-cam_space_->tilt_mid(),
                                                          camera.heading_-cam_space_->head_mid(),
                                                          (camera.top_fov_)/cam_space_->top_fov(0),
                                                          180-camera.tilt_,90-camera.heading_,false);
            // match it with index bboxes;
            std::vector<unsigned int> attribute_poly_ids
                = index_layer.attributed_regions_by_value(ORIENTATION, qval);
            for (unsigned int attribute_poly_id : attribute_poly_ids)
            {
                volm_spherical_region index_region = i_regions[attribute_poly_id];
                unsigned char ival = 0;
                if (!index_region.attribute_value(ORIENTATION,ival))
                    continue;
                // just considering horizontal and vertical
                //if (qval >=1 && ival>=2 && qval <= 3 && ival<=9 )
                if (qval == ival)
                {
                    std::vector<vsph_sph_box_2d> intersection_box ;
                    if (intersection(qbox_xfomred,index_region.bbox_ref(),intersection_box))
                    {
                        for (auto & k : intersection_box)
                        {
                            score+=k.area()/(qbox_xfomred.area());
                        }
                    }
                }
            }
        }
        scores_.push_back(score);
    }
    return true;
}
