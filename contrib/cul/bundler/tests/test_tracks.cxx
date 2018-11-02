#include <string>
#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>

#include <bundler/bundler.h>


#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


static double dist_sq(
    vnl_vector<double> const& side1,
    vnl_vector<double> const& side2)
{
    double d = 0;

    for (unsigned int i = 0; i < side1.size(); ++i) {
        const double diff = side1[i] - side2[i];
        d += diff*diff;
    }

    return d;
}

static const char* IMG_PATH = "contrib/cul/bundler/tests/test_data";

static const int NUM_IMGS = 11;

static const double TOL_SQ = 1.0;

static void test_tracks(int argc, char** argv)
{
    std::string filepath;

    if (argc < 2) {
        std::cerr << "Supply a test_data directory name (containing files"
                 << " kermit*.jpg) on the command line!\n";
        TEST("test_tracks", true, false);

        filepath = IMG_PATH;
    }
    else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    std::vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    std::vector<double> exif_tags(NUM_IMGS);

    for (int i = 0; i < NUM_IMGS; ++i) {
        std::stringstream str;
        str << filepath << "/kermit"
            << std::setw(3) << std::setfill('0') << i << ".jpg";

        imgs[i] = vil_load_image_resource(str.str().c_str(), false);
        exif_tags[i] = BUNDLER_NO_FOCAL_LEN;
    }

    //-------------------- Run the tracks stage.
    bundler_inters_reconstruction recon;

    bundler_tracks tracks_stage;
    tracks_stage.run_feature_stage(imgs, exif_tags, recon);


    //-------------------- Consistency checks

    std::vector<bundler_inters_image_sptr>::const_iterator fs_it;
    std::vector<bundler_inters_track_sptr>::iterator trk_it;

    //Check the cross-references in the track set

    //Test that the tracks don't have a corresponding point yet, and
    // that the tracks' points refer back to the correct track.
    for (trk_it = recon.tracks.begin();
         trk_it != recon.tracks.end(); ++trk_it)
    {
        TEST_EQUAL("Equal number of points as contributing pts",
                   (*trk_it)->points.size(),
                   (*trk_it)->contributing_points.size());

        Assert("Observed is false", !(*trk_it)->observed);

        for (unsigned int i = 0; i < (*trk_it)->points.size(); ++i) {
            TEST("The tracks' points refer to the correct track.",
                 (*trk_it)->points[i]->track,
                 *trk_it);

            Assert("There are no contributing points yet",
                   !(*trk_it)->contributing_points[i]);

            TEST("The features know where they are in their tracks",
                 (*trk_it)->points[(*trk_it)->points[i]->index_in_track],
                 (*trk_it)->points[i]);

            //Test that all features in a track come from different images.
            for (unsigned int j = i+1; j < (*trk_it)->points.size(); ++j) {
                const bool images_match =
                    (*trk_it)->points[i]->image ==
                    (*trk_it)->points[j]->image;

                Assert("Features in a track come from different images.",
                    !images_match);

                TEST_NEAR("Features in a track are 'close' to each other.",
                          dist_sq(
                              (*trk_it)->points[i]->descriptor,
                              (*trk_it)->points[j]->descriptor), 0,
                          TOL_SQ);
            }
        }
    }

    //Test that the features refer to the correct feature sets and that
    // the images pointers are consistent. Also test that visited is set
    // to false in all cases, since we are done with it for this loop,
    // and we'll use it in SFM
    for (fs_it = recon.feature_sets.begin();
         fs_it != recon.feature_sets.end(); ++fs_it)
    {
        Assert("Nothing thinks it is in the reconstruction yet.",
               ! (*fs_it)->in_recon);

        for (unsigned int i = 0; i < (*fs_it)->features.size(); ++i)
        {
            Assert("Visited is false",
                   !(*fs_it)->features[i]->visited );

            TEST("The features know their image set.",
                 (*fs_it)->features[i]->image,
                 *fs_it);

            TEST("The features know where they are in their images",
                 (*fs_it)->features[(*fs_it)->features[i]->index_in_image],
                 (*fs_it)->features[i]);


            // Test that all features in an image come from different
            // tracks, unless this feature doesn't have a track. A feature
            // wouldn't have a track if it is unmatched, so it views a
            // 3D point that no other image contains.
            if ( (*fs_it)->features[i]->track != nullptr) {
                for (unsigned int j = i+1; j < (*fs_it)->features.size(); ++j) {
                    Assert(
                        "Features in an image come from different tracks.",
                        (*fs_it)->features[i]->track !=
                            (*fs_it)->features[j]->track);
                }
            }
        }
    }

    std::vector<bundler_inters_match_set>::const_iterator m;
    for (m = recon.match_sets.begin();
         m != recon.match_sets.end(); ++m)
    {
        for (int i = 0; i < m->num_features(); ++i)
        {
            Assert("Anything in a matched pair has a track (first).",
                   m->matches[i].first->track != nullptr);

            Assert("Anything in a matched pair has a track (second).",
                   m->matches[i].second->track != nullptr);

            TEST("A pair of matched features is always in the same track.",
                 m->matches[i].first->track, m->matches[i].second->track);
        }
    }
}

TESTMAIN_ARGS(test_tracks);
