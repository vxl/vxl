#include <testlib/testlib_test.h>

#include <vpgl/algo/vpgl_triangulate_points.h>
#include <vpgl/vpgl_calibration_matrix.h>

static const vgl_point_3d<double> GOAL(1.0, 1.0, 1.0);
static const double TOL = .001;

// Creates a perspective camera looking at pt, and adds the camera and
// the projection of GOAL to the list.
static void add_pt_and_cam(
    vgl_homg_point_3d<double> pt,
    vgl_vector_3d<double> trans,
    std::vector<vgl_point_2d<double> > &points,
    std::vector<vpgl_perspective_camera<double> > &cameras)
{
    vpgl_calibration_matrix<double> k;
    k.set_focal_length(640);
    k.set_principal_point(vgl_point_2d<double>(20,20));

    vpgl_perspective_camera<double> cam;
    cam.set_calibration(k);
    cam.look_at(pt);
    cam.set_translation(trans);

    cameras.push_back(cam);

    double x,y;
    cam.project(GOAL.x(), GOAL.y(), GOAL.z(), x, y);

    points.emplace_back(x, y);
}

// Adds a bunch of cameras and points to the lists using the above method.
static void get_cams_and_pts(
    std::vector<vgl_point_2d<double> > &points,
    std::vector<vpgl_perspective_camera<double> > &cameras){

    add_pt_and_cam(
        vgl_homg_point_3d<double>(0.0, 1.0, 1.0),
        vgl_vector_3d<double>(-1.0, 1.0, 1.0),
        points, cameras);

    add_pt_and_cam(
        vgl_homg_point_3d<double>(1.0, 0.0, 1.0),
        vgl_vector_3d<double>(1.0, -1.0, 1.0),
        points, cameras);

    add_pt_and_cam(
        vgl_homg_point_3d<double>(1.0, 1.0, 0.0),
        vgl_vector_3d<double>(1.0, 1.0, -1.0),
        points, cameras);

    add_pt_and_cam(
        vgl_homg_point_3d<double>(1.0, 1.0, 1.0),
        vgl_vector_3d<double>(0.0, 0.0, 0.0),
        points, cameras);
}

// Main test routine.
static void test_triangulate()
{
    // First, get the cameras and points we'll be using to triangulate
    std::vector<vgl_point_2d<double> > points;
    std::vector<vpgl_perspective_camera<double> > cameras;
    get_cams_and_pts(points, cameras);

    // Perform the triangulation
    vgl_point_3d<double> pt;
    double err = vpgl_triangulate_points::triangulate(points, cameras, pt);

    std::cout << "Error is " << err << "." << std::endl;

    // Test the result
    TEST_NEAR("Xs are close:", pt.x(), GOAL.x(), TOL);
    TEST_NEAR("Ys are close:", pt.y(), GOAL.y(),TOL);
    TEST_NEAR("Zs are close:", pt.z(), GOAL.z(),TOL);
}

TESTMAIN(test_triangulate);
