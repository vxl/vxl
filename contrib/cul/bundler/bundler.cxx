#include <iostream>
#include <fstream>
#include "bundler.h"
//
#include <bundler/bundler_inters.h>
#include <cassert>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bundler_driver(
    const bundler_routines &routines,
    const std::vector<vil_image_resource_sptr> &imageset,
    const std::vector<double> &exif_tags,

    std::vector<vpgl_perspective_camera<double> > &cameras,
    std::vector<vgl_point_3d<double> > &points,
    vnl_sparse_matrix<bool> &visibility_graph)
{
    bundler_inters_reconstruction recon;

    routines.features_phase.run_feature_stage(
        imageset, exif_tags, recon);

    return routines.sfm_phase.run_sfm_stage(
        recon, cameras, points, visibility_graph);
}

//----------------------------------------------------------------------

bool bundler_write_ply_file(
    const char* filename,
    const std::vector<vgl_point_3d<double> > &points)
{
    std::ofstream ply;
    ply.open(filename);

    if ( !ply.is_open() ) {
        return false;
    }

    // Write the header. According to the standard, the endline
    // character is "\n", not anything else, so use "\n", not std::endl
    ply << "ply\n"
        << "format ascii 1.0\n"
        << "element face 0\n"
        << "property list uchar int vertex_indices\n"
        << "element vertex " << points.size() << '\n'
        << "property float x\n"
        << "property float y\n"
        << "property float z\n"
        << "end_header\n";

    std::vector<vgl_point_3d<double> >::const_iterator i;
    for (i = points.begin(); i != points.end(); i++) {
        ply << i->x() << ' '
            << i->y() << ' '
            << i->z() << '\n';
    }
    ply.close();
    return true;
}


bool bundler_write_bundle_file(
    const char* filename,
    const std::vector<vpgl_perspective_camera<double> > &cameras,
    const std::vector<vgl_point_3d<double> > &points)
{
    std::ofstream bundle_file;
    bundle_file.open(filename);

    if ( !bundle_file.is_open() ) {
        return false;
    }

    // Write the header
    int num_cameras = cameras.size();
    int num_points = points.size();

    bundle_file << "# Bundle file v0.3\n"
                << num_cameras << " " << num_points << "\n";

    std::vector<vpgl_perspective_camera<double> >::const_iterator c;
    for (c = cameras.begin(); c != cameras.end(); c++) {
        /* Intrinsics */
        const vpgl_calibration_matrix<double> &K = c->get_calibration();
        const vnl_matrix_fixed<double,3,3> &R = c->get_rotation().as_matrix();
        const vgl_vector_3d<double> &t = c->get_translation();

        bundle_file << K.focal_length() << " " << 0 << " " << 0 << "\n";
        bundle_file << R;
        bundle_file << t.x() << " " << t.y() << " " << t.z() << "\n";
    }

    std::vector<vgl_point_3d<double> >::const_iterator p;
    for (p = points.begin(); p != points.end(); p++) {
        bundle_file << p->x() << ' ' << p->y() << ' ' << p->z() << '\n';
        bundle_file << 0 << ' ' << 0 << ' ' << 255 << '\n';
        bundle_file << 0 << '\n';
    }

    bundle_file.close();
    return true;
}


//----------------------------------------------------------------------
bundler_tracks::bundler_tracks():manage_pointers(true)
{
    detect_features =
        new bundler_tracks_impl_detect_sift;

    propose_matches =
        new bundler_tracks_impl_propose_matches_all;

    match =
        new bundler_tracks_impl_match_ann;

    refine =
        new bundler_tracks_impl_refine_epipolar;

    chain_matches =
        new bundler_tracks_impl_chain_matches;
}


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
        new bundler_tracks_impl_chain_matches(
            s.chain_matches_settings);
}


void bundler_tracks::run_feature_stage(
    const std::vector<vil_image_resource_sptr> &imageset,
    const std::vector<double> &focal_lengths,
    bundler_inters_reconstruction &empty_recon) const
{
    assert(imageset.size() == focal_lengths.size());

    // First, run the detect stage.
    std::vector<vil_image_resource_sptr>::const_iterator img_i;
    std::vector<double>::const_iterator exif_i;
    for (img_i = imageset.begin(), exif_i = focal_lengths.begin();
         img_i != imageset.end(); img_i++, exif_i++)
    {
        std::cout<<"Detecting features " << (img_i - imageset.begin()) + 1
                << '/' << imageset.size() << std::endl;

        empty_recon.feature_sets.push_back(
            (*detect_features)(*img_i, *exif_i));
    }

    // Now get the match lists.
    std::vector<bundler_inters_image_pair> matches;

    (*propose_matches)(empty_recon.feature_sets, matches);

    // For every proposed match, run the match stage.
    std::vector<bundler_inters_image_pair>::const_iterator j;
    for (j = matches.begin(); j != matches.end(); j++) {
        bundler_inters_match_set match_set;

        std::cout<<"Matching features " << (j - matches.begin()) + 1
                << '/' << matches.size() << std::endl;

        (*match)(*j, match_set);
        (*refine)(match_set);

        if (match_set.num_features() > 0) {
            empty_recon.match_sets.push_back(match_set);
        }
    }

    // Finally, chain everything into a track
    (*chain_matches)(
        empty_recon.match_sets,
        empty_recon.feature_sets,
        empty_recon.tracks);

    // Clear the visited tags for every feature.
    std::vector<bundler_inters_image_sptr>::const_iterator i;
    for (i = empty_recon.feature_sets.begin();
         i != empty_recon.feature_sets.end(); i++)
    {
        std::vector<bundler_inters_feature_sptr>::iterator j;
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
        bundler_sfm_impl_select_next_images();

    add_next_images =
        new bundler_sfm_impl_add_next_images();

    add_new_points =
        new bundler_sfm_impl_add_new_points();

    bundle_adjust =
        new bundler_sfm_impl_bundle_adjust();
}


bundler_sfm::bundler_sfm(bundler_settings_sfm s)
  : manage_pointers(true)
{
    create_initial_recon =
        new bundler_sfm_impl_create_initial_recon(
            s.initial_recon_settings);

    select_next_images =
        new bundler_sfm_impl_select_next_images(
            s.select_next_images_settings);

    add_next_images =
        new bundler_sfm_impl_add_next_images(
            s.add_next_images_settings);

    add_new_points =
        new bundler_sfm_impl_add_new_points(
            s.add_new_points_settings);

    bundle_adjust =
        new bundler_sfm_impl_bundle_adjust(
            s.bundle_adjust_settings);
}


bool bundler_sfm::run_sfm_stage(
        bundler_inters_reconstruction &recon,
        std::vector<vpgl_perspective_camera<double> > &cameras,
        std::vector<vgl_point_3d<double> > &points,
        vnl_sparse_matrix<bool> visibility_matrix) const
{
    assert(cameras.size() == 0);
    assert(points.size() == 0);

    //Create the initial reconstruction
    std::cout << "Creating the initial reconstruction..." << std::endl;

    if ( ! (*create_initial_recon)(recon) ){
        return false;
    }

    std::cout << "Bundle adjusting initial reconstruction..." << std::endl;
    (*bundle_adjust)(recon);

    {
        /* Write initial reconstruction to a file */
        std::vector<vpgl_perspective_camera<double> > cameras_init;

        std::vector<bundler_inters_image_sptr>::const_iterator cam;
        for (cam = recon.feature_sets.begin(); cam != recon.feature_sets.end(); ++cam) {
            if ( (*cam)->in_recon ){
                cameras_init.push_back((*cam)->camera);
            }
        }

        std::vector<vgl_point_3d<double> > points_init;

        std::vector<bundler_inters_track_sptr>::const_iterator pt;
        for (pt = recon.tracks.begin(); pt != recon.tracks.end(); pt++) {
            if ( (*pt)->observed ) {
                points_init.push_back((*pt)->world_point);
            }
        }

        bundler_write_ply_file("points_init.ply", points_init);
        bundler_write_bundle_file("bundle_init.out", cameras_init, points_init);
    }

    //Now do the add images, add points, bundle adjust loop
    std::vector<bundler_inters_image_sptr> to_add;
    while ( (*select_next_images)(recon, to_add) ) {
        std::cout << std::endl;

        std::vector<bundler_inters_image_sptr> added;

        std::cout << "Adding next set of images..." << std::endl;
        (*add_next_images)(to_add, recon, added);

        assert(to_add.size() > 0);

        std::cout << "Bundle adjusting [after adding images]..." << std::endl;
        (*bundle_adjust)(recon);

        std::cout << "Adding new points..." << std::endl;
        (*add_new_points)(recon, added);

        std::cout << "Bundle adjusting [after adding points]..." << std::endl;
        (*bundle_adjust)(recon);

        to_add.clear();
    }

    //Now get the information from the reconstruction struct.
    std::vector<bundler_inters_image_sptr>::const_iterator cam;
    for (cam = recon.feature_sets.begin();
         cam != recon.feature_sets.end();
         ++cam)
    {
        if ( (*cam)->in_recon ){
            cameras.push_back((*cam)->camera);
        }
    }

    std::vector<bundler_inters_track_sptr>::const_iterator pt;
    for (pt = recon.tracks.begin(); pt != recon.tracks.end(); pt++) {
        if ( (*pt)->observed ) {
            points.push_back((*pt)->world_point);
        }
    }

    visibility_matrix = recon.visibility_matrix;

    return true;
}
