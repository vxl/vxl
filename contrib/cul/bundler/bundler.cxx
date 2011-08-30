#include <bundler/bundler.h>
#include <bundler/bundler_inters.h>
#include <vcl_cassert.h>


//----------------------------------------------------------------------
bundler_tracks::bundler_tracks():manage_pointers(true) {
    detect_features =
        new bundler_tracks_impl_detect_sift;

    propose_matches =
        new bundler_tracks_impl_propose_matches_all;

    match =
        new bundler_tracks_impl_match_ann;

    refine =
        new bundler_tracks_impl_refine_epipolar;

    chain_matches =
        new bundler_tracks_default_chain_matches;
};


bundler_tracks::bundler_tracks(
    bundler_settings_tracks s) : manage_pointers(true)
{
    detect_features =
        new bundler_tracks_impl_detect_sift(
            s.detect_settings);

    propose_matches =
        new bundler_tracks_impl_propose_matches_all(
            s.propose_matches_settings);

    match =
        new bundler_tracks_impl_match_ann(
            s.match_settings);

    refine =
        new bundler_tracks_impl_refine_epipolar(
            s.refine_settings);

    chain_matches =
        new bundler_tracks_default_chain_matches(
            s.chain_matches_settings);
};


void bundler_tracks::run_feature_stage(
    const vcl_vector<vil_image_resource_sptr> &imageset,
    const vcl_vector<double> &focal_lengths,
    bundler_inters_track_set &track_set)
{
    // First, run the detect stage.
    vcl_vector<vil_image_resource_sptr>::const_iterator img_i;
    vcl_vector<double>::const_iterator exif_i;
    for (img_i = imageset.begin(), exif_i = focal_lengths.begin();
         img_i != imageset.end(); img_i++, exif_i++)
    {
        track_set.feature_sets.push_back(
            (*detect_features)(*img_i, *exif_i));
    }

    // Now get the match lists.
    vcl_vector<bundler_inters_feature_set_pair> matches;

    (*propose_matches)(track_set.feature_sets, matches);

    // For every proposed match, run the match stage.
    vcl_vector<bundler_inters_feature_set_pair>::const_iterator j;
    for (j = matches.begin(); j != matches.end(); j++) {
        bundler_inters_match_set match_set;
        (*match)(*j, match_set);
        (*refine)(match_set);

        if (match_set.num_features() > 0) {
            track_set.match_sets.push_back(match_set);
        }
    }

    // Finally, chain everything into a track
    (*chain_matches)(track_set.match_sets, track_set.tracks);

    // Clear the visited tags for every feature.
    vcl_vector<bundler_inters_feature_set_sptr>::const_iterator i;
    for (i = track_set.feature_sets.begin();
         i != track_set.feature_sets.end(); i++)
    {
        vcl_vector<bundler_inters_feature_sptr>::iterator j;
        for (j = (*i)->features.begin(); j != (*i)->features.end(); j++) {
            (*j)->visited = false;
        }
    }
}


// ---------------------------------------------------------------------
bundler_sfm::bundler_sfm()
  : manage_pointers(true)
{
    create_initial_recon =
        new bundler_sfm_impl_create_initial_recon();

    select_next_images = new
        bundler_sfm_default_select_next_images();

    add_next_images =
        new bundler_sfm_default_add_next_images();

    bundle_adjust =
        new bundler_sfm_default_bundle_adjust();
}


bundler_sfm::bundler_sfm(bundler_settings_sfm s)
  : manage_pointers(true)
{
    create_initial_recon =
        new bundler_sfm_impl_create_initial_recon(
            s.initial_recon_settings);

    select_next_images =
        new bundler_sfm_default_select_next_images(
            s.select_next_images_settings);

    add_next_images =
        new bundler_sfm_default_add_next_images(
            s.add_next_images_settings);

    bundle_adjust =
        new bundler_sfm_default_bundle_adjust(
            s.bundle_adjust_settings);
}


void bundler_sfm::run_sfm_stage(
        bundler_inters_track_set &track_set,
        vcl_vector<vpgl_perspective_camera<double> > &cameras,
        vcl_vector<vgl_point_3d<double> > &points,
        vnl_sparse_matrix<bool> visibility_matrix)
{
    bundler_inters_reconstruction recon;

    //Create the initial reconstruction
    (*create_initial_recon)(track_set, recon);

    //Now do the add images, add points, bundle adjust loop
    vcl_vector<bundler_inters_feature_set_sptr> to_add;
    while ( (*select_next_images)(recon, track_set, to_add) ) {
        vcl_vector<bundler_inters_camera> added;

        (*add_next_images)(recon, added, to_add);
        (*add_new_points)(recon, track_set, added);
        (*bundle_adjust)(recon);

        to_add.clear();
    }

    //Now get the information from the reconstruction struct.
    vcl_vector<bundler_inters_camera>::const_iterator cam;
    for (cam = recon.cameras.begin(); cam != recon.cameras.end(); cam++) {
        cameras.push_back(cam->camera);
    }

    vcl_vector<bundler_inters_3d_point>::const_iterator pt;
    for (pt = recon.points.begin(); pt != recon.points.end(); pt++) {
        points.push_back(pt->point_3d);
    }

    visibility_matrix = recon.visibility_matrix;
}

