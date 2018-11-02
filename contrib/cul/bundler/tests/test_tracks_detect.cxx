#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bundler/bundler_tracks_impl.h>

#include <vil/vil_load.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// TODO: Save an image with the location of all the features.

static void test_tracks_detect(int argc, char* argv[])
{

    vil_image_resource_sptr source_img;

    if(argc < 2){
        std::cerr<<"Supply a filename for the first argument!" << std::endl;

        TEST("test_tracks_detect", true, false);

        source_img = vil_load_image_resource(
        "contrib/cul/bundler/tests/test_data/kermit000.jpg",
        false);

    } else {
        // Load the image
        source_img = vil_load_image_resource(argv[1], false);
    }

    // Create and run the implementation of the detect stage
    bundler_tracks_impl_detect_sift detect;

    bundler_inters_image_sptr image =
        detect(source_img, BUNDLER_NO_FOCAL_LEN);

    // Perform consistency checks
    TEST_EQUAL("The focal length is correct.",
        image->focal_length,
        BUNDLER_NO_FOCAL_LEN);

    TEST("Check that the vil_image_resource is correct.",
        image->source,
        source_img);

    // Now look at all the features individually.
    TEST_FAR("Make sure there are some features.",
        image->features.size(), 0, .5);

    std::vector<bundler_inters_feature_sptr>::iterator i;
    for(i = image->features.begin();
        i != image->features.end(); i++){

        // Make sure all the values are expected.
        Assert("Visited is always false at this point.",
            !(*i)->visited);

        TEST("The feature set cross-reference is set properly.",
            (*i)->image,
            image);

        TEST("The track is not yet set.",
            (*i)->track,
            nullptr);

        TEST_EQUAL("Descriptors are the same size.",
            (*i)->descriptor.size(),
            (*image->features.begin())->descriptor.size());

        TEST("The feature knows where it is in the image list.",
            image->features[(*i)->index_in_image],
            *i);

        // I can't think of a good test for the descriptor or the
        // 2D location. I'm just going to print out the 2d point
        // for a manual check. If anyone can think of a better idea...
        std::cout << "Location of feature: " << (*i)->point << std::endl;
    }
}



TESTMAIN_ARGS(test_tracks_detect);
