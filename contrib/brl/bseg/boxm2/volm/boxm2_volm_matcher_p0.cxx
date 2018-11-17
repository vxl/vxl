// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p0.cxx
#include <utility>
#include "boxm2_volm_matcher_p0.h"
//:
// \file
#include "boxm2_volm_wr3db_index.h"
#include <vul/vul_timer.h>


struct scorecomp
{
    bool operator()( std::pair<float,int> const& a, std::pair<float,int> const& b) const
    {
        return a.first >b.first;
    }
} scorecomp_obj;

boxm2_volm_matcher_p0::~boxm2_volm_matcher_p0()
= default;

boxm2_volm_matcher_p0::boxm2_volm_matcher_p0(const volm_camera_space_sptr& cam_space,volm_spherical_region_query srq,float threshold)
: count_(0),cam_space_(cam_space),query_(std::move(srq)),threshold_(threshold)
{
    unsigned int count = 0;
    for ( camera_space_iterator iter = cam_space_->begin(); iter != cam_space_->end(); ++iter,++count)
    {
        cam_angles camera = iter->camera_angles();

        unsigned roll_index;
        unsigned fov_index;
        unsigned head_index;
        unsigned tilt_index;
        iter->cam_indices(roll_index,fov_index,head_index,tilt_index);
        volm_spherical_regions_layer q_regions =query_.query_regions(roll_index);

        std::vector<vsph_sph_box_2d> xfomred_boxes;
        vsph_sph_box_2d imbox;
        for (int i = 0; i< q_regions.size(); ++i)
        {
            imbox.add(q_regions.regions()[i].bbox_ref().min_point());
            imbox.add(q_regions.regions()[i].bbox_ref().max_point());
        }


        vsph_sph_box_2d imbox_xfomred = imbox.transform(camera.tilt_-cam_space_->tilt_mid(),
                                                        camera.heading_-cam_space_->head_mid(),
                                                        (camera.top_fov_)/cam_space_->top_fov(0),
                                                        180-camera.tilt_,90-camera.heading_,false);
        imbox_xformed_[count] = imbox_xfomred;
        for (int i = 0; i< q_regions.size(); ++i)
        {
            // transform query_region box
            volm_spherical_region query_region = q_regions.regions()[i];
            vsph_sph_box_2d qbox = query_region.bbox_ref();
            //: convert google coordinate axis( z is down and x is north) to spherical coordinate system (z is up and x is east)
            vsph_sph_box_2d qbox_xfomred = qbox.transform(camera.tilt_-cam_space_->tilt_mid(),
                                                          camera.heading_-cam_space_->head_mid(),
                                                          (camera.top_fov_)/cam_space_->top_fov(0),
                                                          180-camera.tilt_,90-camera.heading_,false);

            xfomred_boxes.push_back(qbox_xfomred);
        }

        query_xformed_boxes_[count] = xfomred_boxes;
    }
}

//: matcher function
bool boxm2_volm_matcher_p0::match(volm_spherical_region_index & index, const volm_score_sptr& score)
{
    vul_timer t;
    t.mark();
    spherical_region_attributes attributes_to_match[] = {ORIENTATION,
                                                         NLCD,
                                                         SKY};
    std::vector<std::pair<float,int> > scores;
    volm_spherical_regions_layer index_layer = index.index_regions();
    std::vector<volm_spherical_region> i_regions = index_layer.regions();
    int count = 0;

    for ( camera_space_iterator iter = cam_space_->begin(); iter != cam_space_->end(); ++iter,++count)
    {
        unsigned roll_index;
        unsigned fov_index;
        unsigned head_index;
        unsigned tilt_index;
        double score = 0.0;
        iter->cam_indices(roll_index,fov_index,head_index,tilt_index);
        volm_spherical_regions_layer q_regions =query_.query_regions(roll_index);
        std::map<unsigned char, double> scores_by_lcd;
        //this->match_order(index_layer,score,q_regions,iter,imbox_xformed_[count]);
        std::map<unsigned char, int> score_nlcd ;
        for (int i = 0; i< q_regions.size(); ++i)
        {
            volm_spherical_region query_region = q_regions.regions()[i];
            unsigned char qval = 0;
            vsph_sph_box_2d qbox_xfomred = query_xformed_boxes_[iter->cam_index()][i];
            if (qbox_xfomred.area()<=0.0) continue;
            for (auto & k : attributes_to_match)
            {
                if (!query_region.attribute_value(k,qval))
                    continue;
                std::vector<unsigned int> attribute_poly_ids = index_layer.attributed_regions_by_value(k,qval);
                for (unsigned int attribute_poly_id : attribute_poly_ids)
                {
                    volm_spherical_region index_region = i_regions[attribute_poly_id];
                    //: just considering horizontal and vertical
                    double int_area = intersection_area(qbox_xfomred,index_region.bbox_ref())/(qbox_xfomred.area());
                    if (k == ORIENTATION)
                        score+= 0.01 * int_area;
                    else if (k == SKY)
                        score+= 1.0 * int_area;
                    else
                    {
                        if (int_area > 0.0)
                            score_nlcd[qval]= 1;
                    }
                }
            }
        }
        for (auto & map_iter : score_nlcd)
            score+=(float)map_iter.second;
        scores.push_back(std::make_pair<float,int>((float)score,(*iter).cam_index()));
    }
    //std::cout<<"Time taken is "<< t.all()<<std::endl;
    float max_score = -1e10;
    int max_cam_id = -1;
    int grndtruthindex = cam_space_->closest_index(cam_angles(cam_space_->roll_mid(),cam_space_->top_fovs()[0],cam_space_->head_mid(),cam_space_->tilt_mid()));
    std::cout<<"INDEX of Ground Truth cam "<<grndtruthindex<<" and the score is "<< scores[grndtruthindex].first<<std::endl;
    //: selecting the top 200 cameras and the best camera location
    std::sort(scores.begin(),scores.end(),scorecomp_obj);
    int cnt = 0;
    for (auto iter = scores.begin(); iter!=scores.end() && cnt < 1000; ++iter, ++cnt)
    {
        if (iter->first > max_score )
        {
            max_score = iter->first;
            max_cam_id = iter->second;
        }
        score->cam_id_.push_back(iter->second);
        //std::cout<<' '<<iter->second<<' ' <<iter->first;
    }
    score->max_cam_id_ = max_cam_id;
    score->max_score_ = max_score;

    //std::cout<<" BEST CAMERA ";
    //cam_space_->camera_angles(max_cam_id).print();
    //std::cout<<"Total # of cameras "<<scores.size()<<" and the central camera is at "<<scores.size()/2<<std::endl;
    //std::cout<<"Closest to Canocnical camera  "<<grndtruthindex;


    return true;
}


bool boxm2_volm_matcher_p0::match_order(volm_spherical_regions_layer & index_layer, double & score, volm_spherical_regions_layer & q_regions, camera_space_iterator & iter, vsph_sph_box_2d & imbox_xfomred)
{
    //: compute mean or min depth for each query box
    cam_angles camera = iter->camera_angles();
    std::vector<volm_spherical_region> i_regions = index_layer.regions();
    std::vector<unsigned int> attribute_poly_ids = index_layer.attributed_regions_by_type_only(DEPTH_INTERVAL);

    std::vector<float> depths ;
    std::vector<unsigned char> depth_intervals ;
    std::vector<unsigned int> reduced_attribute_poly_ids;
    for (unsigned int attribute_poly_id : attribute_poly_ids)
    {
        volm_spherical_region index_region = i_regions[attribute_poly_id];
        if (intersection_area(imbox_xfomred,index_region.bbox_ref()) > 0.0)
            reduced_attribute_poly_ids.push_back(attribute_poly_id);
    }
    for (int i = 0; i< q_regions.size(); ++i)
    {
        // transform query_region box
        volm_spherical_region query_region = q_regions.regions()[i];
        unsigned char qval ;
        q_regions.regions()[i].attribute_value(DEPTH_ORDER, qval);
        if ( qval == 255 || q_regions.regions()[i].is_attribute(SKY) ||
             q_regions.regions()[i].is_attribute(GROUND) )
            continue;
        depth_intervals.push_back(qval);
        vsph_sph_box_2d qbox_xfomred = query_xformed_boxes_[iter->cam_index()][i];

        float mean_depth = 0.0;
        float sum_weights = 0;
        for (unsigned int reduced_attribute_poly_id : reduced_attribute_poly_ids)
        {
            volm_spherical_region index_region = i_regions[reduced_attribute_poly_id];
            float area = intersection_area(qbox_xfomred,index_region.bbox_ref());
            auto ival = (unsigned char)0;
            index_region.attribute_value(DEPTH_INTERVAL,ival);
            mean_depth+=area*std::pow(1.01f,(float)ival);
            sum_weights+=area;
        }
        mean_depth =sum_weights > 0.0 ? mean_depth/sum_weights: 0.0;
        depths.push_back(mean_depth);
    }

    for (unsigned i = 0; i< depth_intervals.size()-1; ++i)
        for (unsigned j = i; j< depth_intervals.size(); ++j)
            if ( (depth_intervals[i] < depth_intervals[j] || depths[i] > depths[j] ) &&
                 (depth_intervals[i] > depth_intervals[j] || depths[i] < depths[j] ) )
                score+=0.1;

    return true;
}
