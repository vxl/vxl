#include <testlib/testlib_test.h>
#include <bundler/bundler_tracks_impl.h>

#include <bpgl/algo/bpgl_fm_compute_ransac.h>

#include <vil/vil_load.h>

static const char* IMG1_PATH =
    "contrib/cul/bundler/tests/test_data/kermit000.jpg";

static const char* IMG2_PATH =
    "contrib/cul/bundler/tests/test_data/kermit001.jpg";


static int num_matches(
    const bundler_inters_feature_sptr& f,
    std::vector< bundler_inters_feature_pair > const& m){

    int num = 0;

    std::vector<bundler_inters_feature_pair>::const_iterator i;
    for(i = m.begin(); i != m.end(); i++){
        if(i->first == f || i->second == f){
            num++;
        }
    }

    return num;
}

static void test_refine(int argc, char** argv)
{
    vil_image_resource_sptr img1;
    vil_image_resource_sptr img2;

    if (argc < 3) {
        std::cerr<<"Supply a filename for the first two args!\n";
        TEST("test_refine", true, false);

        img1 = vil_load_image_resource(IMG1_PATH, false);
        img2 = vil_load_image_resource(IMG2_PATH, false);

    } else {
        // Load the image
        img1 = vil_load_image_resource(argv[1], false);
        img2 = vil_load_image_resource(argv[2], false);
    }

    // ------------------Set up the intermediate types for the refinement.
    bundler_inters_image_pair to_match;

    bundler_tracks_impl_detect_sift detect;
    to_match.f1 = detect(img1, -1);
    to_match.f2 = detect(img2, -1);

    // ------------------Do the matching.
    bundler_inters_match_set matches;

    bundler_tracks_impl_match_ann match;
    match(to_match, matches);


    // ------------------Do the refining.
    bundler_tracks_impl_refine_epipolar refine;
    refine(matches);


    // ------------------Consistency checks

    // Check that a feature in side1 is only matched with
    // one feature in side2, and vice versa. We check this by
    // seeing if each feature is unique in its list.
    std::vector<bundler_inters_feature_pair>::const_iterator i;
    for(i = matches.matches.begin(); i != matches.matches.end(); i++){

        int n = num_matches(i->first, matches.matches);
        TEST_EQUAL("Feature is unique", n, 1);

        n = num_matches(i->second, matches.matches);
        TEST_EQUAL("Feature is unique", n, 1);
    }

    // TODO: Check the epipolar aspect of the refinement.
}


TESTMAIN_ARGS(test_refine);
