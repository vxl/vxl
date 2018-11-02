// This is core/vpgl/algo/vpgl_triangulate_points.cxx
#include <cmath>
#include <cstdlib>
#include "vpgl_triangulate_points.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vgl/vgl_vector_3d.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_svd.h>


double vpgl_triangulate_points::triangulate(
        const std::vector<vgl_point_2d<double> > &points,
        const std::vector<vpgl_perspective_camera<double> > &cameras,
        vgl_point_3d<double> &point_3d)
{
    constexpr int num_vars = 3;// One var for x, y, z of output 3d point
    const int num_eqs = static_cast<const int>(2 * points.size());

    // Set up the least-squares solution.
    vnl_matrix<double> A(num_eqs, num_vars, 0.0);
    vnl_vector<double> b(num_eqs, 0.0);

    for (unsigned int i = 0; i < points.size(); ++i) {
        const vgl_vector_3d<double> &trans =
            cameras[i].get_translation();

        const vnl_double_3x3 &rot =
            cameras[i].get_rotation().as_matrix();

        const vgl_point_2d<double> pt =
            cameras[i].get_calibration().map_to_focal_plane(points[i]);

        // Set the row for x for this point
        A.put(2 * i, 0, rot.get(0, 0) - pt.x() * rot.get(2, 0) );
        A.put(2 * i, 1, rot.get(0, 1) - pt.x() * rot.get(2, 1) );
        A.put(2 * i, 2, rot.get(0, 2) - pt.x() * rot.get(2, 2) );

        // Set the row for y for this point
        A.put(2*i+1, 0, rot.get(1, 0) - pt.y() * rot.get(2, 0) );
        A.put(2*i+1, 1, rot.get(1, 1) - pt.y() * rot.get(2, 1) );
        A.put(2*i+1, 2, rot.get(1, 2) - pt.y() * rot.get(2, 2) );

        // Set the RHS row.
        b[2*i + 0] = trans.z() * pt.x() - trans.x();
        b[2*i + 1] = trans.z() * pt.y() - trans.y();
    }

    // Find the least squares result
    vnl_svd<double> svd(A);
    vnl_double_3 x = svd.solve(b);

    point_3d.set(x.begin());

    // Find the error
    double error = 0.0;
    for (unsigned int i = 0; i < points.size(); ++i) {
        // Compute projection error
        vnl_double_3 pp = cameras[i].get_rotation().as_matrix() * x;

        pp[0] += cameras[i].get_translation().x();
        pp[1] += cameras[i].get_translation().y();
        pp[2] += cameras[i].get_translation().z();

        double dx = pp[0] / pp[2] - points[i].x();
        double dy = pp[1] / pp[2] - points[i].y();
        error += dx * dx + dy * dy;
    }

    return std::sqrt(error / points.size());
}
