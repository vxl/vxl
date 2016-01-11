#ifndef BUNDLER_TEST_UTILS_H
#define BUNDLER_TEST_UTILS_H

#include <bundler/bundler_inters.h>

// If expected_cameras is -1, doesn't check the camera number.
void test_recon(const bundler_inters_reconstruction &recon,
    int expected_cameras = -1);

#endif /*BUNDLER_TEST_UTILS_H*/
