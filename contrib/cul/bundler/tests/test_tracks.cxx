#include <testlib/testlib_test.h>

#include <bundler/bundler.h>


#include <vil/vil_load.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

static const char* IMG_PATH =
    "/home/anjruu/vxl/contrib/cul/bundler/tests/test_data";

static void test_tracks(int argc, char** argv)
{
    vcl_string filepath;

    if (argc < 2) {
        vcl_cerr<<"Supply a filename for the first two args!\n";
        TEST("test_tracks", true, false);

        // TODO Get an argument into this test!
        filepath = IMG_PATH;

    } else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    vcl_vector<vil_image_resource_sptr> imgs(11);
    vcl_vector<double> exif_tags(11);

    for(int i = 0; i <= 10; i++){
        vcl_stringstream str;
        str << filepath << "/kermit" 
            << vcl_setw(3) << vcl_setfill('0') << i << ".jpg";

        imgs[i] = vil_load_image_resource(str.str().c_str(), false);
        exif_tags[i] = BUNDLER_NO_FOCAL_LEN;
    }

    //-------------------- Run the tracks stage.
    bundler_inters_track_set track_set;

    bundler_tracks tracks_stage;
    tracks_stage.run_feature_stage(imgs, exif_tags, track_set);
    

    //-------------------- Consistency checks
    //Check the cross-references in the track set

    //Test that the tracks don't have a corresponding point yet, and
    // that the tracks' points refer back to the correct track.
    vcl_vector<bundler_inters_track_sptr>::iterator trk_it;
    for (trk_it = track_set.tracks.begin();
        trk_it != track_set.tracks.end(); trk_it++){

        TEST( "The corresponding point is NULL",
            (*trk_it)->corresponding_point,
            NULL);

        for (int i = 0; i < (*trk_it)->points.size(); i++) {
            TEST("The tracks' points refer to the correct track.",
                (*trk_it)->points[i]->track,
                *trk_it);
        }
    }

    //Test that the features refer to the correct feature sets and that
    // the images pointers are consistent. Also test that visited is set
    // to false in all cases, since we are done with it for this loop,
    // and we'll use it in SFM
    vcl_vector<bundler_inters_feature_set_sptr>::const_iterator fs_it;
    for (fs_it = track_set.feature_sets.begin();
         fs_it != track_set.feature_sets.end(); fs_it++){

        for (int i = 0; i < (*fs_it)->features.size(); i++) {
            Assert("Visited is false",
                !(*fs_it)->features[i]->visited );

            TEST("The features know their feature set.",
                (*fs_it)->features[i]->feature_set,
                 *fs_it);

            TEST("The features know their source image.",
                (*fs_it)->features[i]->source_image,
                (*fs_it)->source_image);
        }
    }

    //TODO: Test stuff like "all tracks are cliques in the graph".
}

TESTMAIN_ARGS(test_tracks);
