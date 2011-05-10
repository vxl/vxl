#include <vpgl/algo/vpgl_bundler.h>
#include <vpgl/algo/vpgl_bundler_inters.h>


/*--------------------------------------------------------------------*/
vpgl_bundler_tracks::vpgl_bundler_tracks():manage_pointers(true){
    detect_features = 
        new vpgl_bundler_tracks_impl_detect_sift;

    propose_matches =
        new vpgl_bundler_tracks_impl_propose_matches_all;

    match = 
        new vpgl_bundler_tracks_impl_match_ann;

    refine = 
        new vpgl_bundler_tracks_impl_refine_epipolar;

    chain_matches = 
        new vpgl_bundler_tracks_default_chain_matches;
};        


vpgl_bundler_tracks::vpgl_bundler_tracks(
    vpgl_bundler_settings_tracks s) : manage_pointers(true){

    detect_features = 
        new vpgl_bundler_tracks_impl_detect_sift(
            s.detect_settings);

    propose_matches =
        new vpgl_bundler_tracks_impl_propose_matches_all(
            s.propose_matches_settings);

    match = 
        new vpgl_bundler_tracks_impl_match_ann(
            s.match_settings);

    refine = 
        new vpgl_bundler_tracks_impl_refine_epipolar(
            s.refine_settings);

    chain_matches = 
        new vpgl_bundler_tracks_default_chain_matches(
            s.chain_matches_settings);
};


void vpgl_bundler_tracks::run_feature_stage(
    const vcl_vector<vil_image_resource_sptr> &imageset,
    const vcl_vector<double> exif_tags,
    vpgl_bundler_inters_track_set &track_set){
    
    /*First, run the detect stage.*/
    vcl_vector<vil_image_resource_sptr>::const_iterator img_i;
    vcl_vector<double>::const_iterator exif_i;
    for(img_i = imageset.begin(), exif_i = exif_tags.begin(); 
        img_i != imageset.end(); img_i++, exif_i++){

        track_set.feature_sets.push_back(
            (*detect_features)(*img_i, *exif_i));
    }

    /*Now get the match lists.*/
    vcl_vector<vpgl_bundler_inters_feature_set_pair> matches;

    (*propose_matches)(track_set.feature_sets, matches);


    /*For every proposed match, run the match stage.*/
    vcl_vector<vpgl_bundler_inters_feature_set_pair>::const_iterator j;
    for(j = matches.begin(); j != matches.end(); j++){
        vpgl_bundler_inters_match_set match_set;
        (*match)(*j, match_set);
        (*refine)(match_set);

        if(match_set.num_features() > 0){
            track_set.match_sets.push_back(match_set);
        }
    }

    /*Finally, chain everything into a track*/
    (*chain_matches)(track_set.match_sets, track_set.tracks);
}



/*-----------------------------------------------------------------------*/

vpgl_bundler_sfm::vpgl_bundler_sfm() : 

    manage_pointers(true) {
    create_initial_recon = 
        new vpgl_bundler_sfm_impl_create_initial_recon();

    select_next_images = new 
        vpgl_bundler_sfm_default_select_next_images();

    add_next_images = 
        new vpgl_bundler_sfm_default_add_next_images();

    bundle_adjust = 
        new vpgl_bundler_sfm_default_bundle_adjust();
}


vpgl_bundler_sfm::vpgl_bundler_sfm(
    vpgl_bundler_settings_sfm s) : manage_pointers(true) {

    create_initial_recon = 
        new vpgl_bundler_sfm_impl_create_initial_recon(
            s.initial_recon_settings);

    select_next_images = 
        new vpgl_bundler_sfm_default_select_next_images(
            s.select_next_images_settings);

    add_next_images = 
        new vpgl_bundler_sfm_default_add_next_images(
            s.add_next_images_settings);

    bundle_adjust = 
        new vpgl_bundler_sfm_default_bundle_adjust(
            s.bundle_adjust_settings);
}


void vpgl_bundler_sfm::run_sfm_stage(
        vpgl_bundler_inters_track_set &track_set,

        vcl_vector<vpgl_perspective_camera<double> > &cameras,
        vcl_vector<vgl_point_3d<double> > &points,
        vnl_sparse_matrix<bool> visiblity_matrix){
    /*TODO*/
}

