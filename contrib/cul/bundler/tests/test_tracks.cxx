#include <testlib/testlib_test.h>

#include <bundler/bundler.h>


#include <vil/vil_load.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>

static const char* IMG_PATH =
    "/home/anjruu/vxl/contrib/cul/bundler/tests/test_data";

static const int NUM_IMGS = 11;

static void test_tracks(int argc, char** argv)
{
    vcl_string filepath;

    if (argc < 2) {
        vcl_cerr<<"Supply a filename for the first two args!\n";
        TEST("test_tracks", true, false);

        filepath = IMG_PATH;

    } else {
        filepath = argv[1];
    }

    //-------------------- Load all the images.
    vcl_vector<vil_image_resource_sptr> imgs(NUM_IMGS);
    vcl_vector<double> exif_tags(NUM_IMGS);

    for(int i = 0; i < NUM_IMGS; i++){
        vcl_stringstream str;
        str << filepath << "/kermit" 
            << vcl_setw(3) << vcl_setfill('0') << i << ".jpg";

        imgs[i] = vil_load_image_resource(str.str().c_str(), false);
        exif_tags[i] = BUNDLER_NO_FOCAL_LEN;
    }

    //-------------------- Run the tracks stage.
    bundler_inters_reconstruction recon;

    bundler_tracks tracks_stage;
    tracks_stage.run_feature_stage(imgs, exif_tags, recon);
    

    //-------------------- Consistency checks
    //Check the cross-references in the track set

    //Test that the tracks don't have a corresponding point yet, and
    // that the tracks' points refer back to the correct track.
    vcl_vector<bundler_inters_track_sptr>::iterator trk_it;
    for (trk_it = recon.tracks.begin();
        trk_it != recon.tracks.end(); trk_it++){

        TEST_EQUAL("Equal number of points as contributing pts",
            (*trk_it)->points.size(),
            (*trk_it)->contributing_points.size());

        for (int i = 0; i < (*trk_it)->points.size(); i++) {
            TEST("The tracks' points refer to the correct track.",
                (*trk_it)->points[i]->track,
                *trk_it);

            Assert("There are no contributing points yet",
                !(*trk_it)->contributing_points[i]);

            TEST("The features know where they are in their tracks",
                (*trk_it)->points[(*trk_it)->points[i]->index_in_track],
                (*trk_it)->points[i]);

            //Test that all features in a track come from different images.
            for(int j = i+1; j < (*trk_it)->points.size(); j++){
                const bool images_match = 
                    (*trk_it)->points[i]->image == 
                    (*trk_it)->points[j]->image;

                Assert(
                    "Features in a track come from different images.",  
                    !images_match);
            }
        }
    }

    //Test that the features refer to the correct feature sets and that
    // the images pointers are consistent. Also test that visited is set
    // to false in all cases, since we are done with it for this loop,
    // and we'll use it in SFM
    vcl_vector<bundler_inters_image_sptr>::const_iterator fs_it;
    for (fs_it = recon.feature_sets.begin();
         fs_it != recon.feature_sets.end(); fs_it++){

        for (int i = 0; i < (*fs_it)->features.size(); i++) {

            Assert("Visited is false",
                !(*fs_it)->features[i]->visited );

            TEST("The features know their image set.",
                (*fs_it)->features[i]->image,
                 *fs_it);
        
            //Test that all features in an image come from different tracks.
            /*for(int j = i+1; j < (*fs_it)->features.size(); j++){
                Assert(
                    "Features in an image come from different tracks.", 
                    (*fs_it)->features[i]->track != 
                        (*fs_it)->features[j]->track);
            }*/
        }
    }

    vcl_vector<bundler_inters_match_set>::const_iterator m;
    for(m = recon.match_sets.begin(); 
        m != recon.match_sets.end(); m++){

        for(int i = 0; i < m->num_features(); i++){
            TEST("A pair of matched features is always in the same track.",
                m->matches[i].first->track, m->matches[i].second->track);
        }
    }
}

TESTMAIN_ARGS(test_tracks);
