#include <testlib/testlib_test.h>
#include <bundler/bundler.h>


static void test_ply(){
    std::vector<vgl_point_3d<double> > pts(10);

    for(int i = 0; i < 10; i++){
        pts[i].set(i, i, i);
    }

    bundler_write_ply_file("foo.ply", pts);
}


TESTMAIN(test_ply);
