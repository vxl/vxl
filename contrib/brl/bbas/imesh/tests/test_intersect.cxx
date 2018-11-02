#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <imesh/algo/imesh_intersect.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_triangle_3d.h>


namespace {

// treat a point like a vector for fast dot products
double dot_product(const vgl_point_3d<double>& p, const vgl_vector_3d<double>& v)
{
  return p.x()*v.x() + p.y()*v.y() + p.z()*v.z();
}

vgl_point_3d<double> operator * (double s, const vgl_point_3d<double>& p)
{
  return {s*p.x(),s*p.y(),s*p.z()};
}

vgl_point_3d<double> operator + (const vgl_point_3d<double>& p1, const vgl_point_3d<double>& p2)
{
  return {p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z()};
}

}; // end of namespace


static void test_intersect()
{
  std::cout << "Mesh Intersection\n";
  {
    vgl_point_3d<double> p(2,3,4);
    vgl_vector_3d<double> d(-2,-3,-4);
    normalize(d);
    vgl_point_3d<double> a(2,0,0), b(-1,1,0), c(0,-3,0);

    vgl_vector_3d<double> n(cross_product(b-a,c-a));
    double dist = std::numeric_limits<double>::infinity();
    double dist2 = dist;
    double u1,v1,u2,v2;

    bool hit1 = imesh_intersect_triangle(p,d,a,b,c,n,dist,u1,v1);
    bool hit2 = imesh_intersect_triangle_min_dist(p,d,a,b,c,n,dist2,u2,v2);


    vgl_point_3d<double> r1 = p+dist*d;
    vgl_point_3d<double> r2 = (1-u1-v1)*a + u1*b + v1*c;

    TEST("intersection occurred", hit1 , true);
    TEST_NEAR("barycentric coords", (r2-r1).length(), 0.0, 1e-14);

    std::cout << "diff: " << r2 - r1 << std::endl;

    dist2 -= 0.001;
    bool hit3 = imesh_intersect_triangle_min_dist(p,d,a,b,c,n,dist2,u2,v2);
    TEST("min dist intersection", hit2 && !hit3 , true);
  }

  {
    vgl_point_3d<double> a(0,0,0), b(3,0,0), c(-1,1,0);
    vgl_point_3d<double> p1(0,0.25,2); // cp inside
    vgl_point_3d<double> p2(0.5,-1,-1); // cp on ab
    vgl_point_3d<double> p3(-0.5,-1,-1); // cp on a
    vgl_point_3d<double> p4(4,-1,2); // cp on b
    vgl_point_3d<double> p5(3,1,-2); // cp on bc
    vgl_point_3d<double> p6(-1,2,-2); // cp on c
    vgl_point_3d<double> p7(-1,0,-3); // cp on ac
    double u,v,dist;
    unsigned char i = imesh_triangle_closest_point(p1,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Inside point flag", i, 7);
    TEST_NEAR("Inside point dist", dist, vgl_triangle_3d_distance(p1,a,b,c), 1e-14);
    TEST_NEAR("Inside point location", ( imesh_triangle_closest_point(p1,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p1,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p2,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Edge ab point flag", i, 3);
    TEST_NEAR("Edge ab point dist", dist, vgl_triangle_3d_distance(p2,a,b,c), 1e-14);
    TEST_NEAR("Edge ab location", ( imesh_triangle_closest_point(p2,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p2,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p3,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Corner a point flag", i, 1);
    TEST_NEAR("Corner a point dist", dist, vgl_triangle_3d_distance(p3,a,b,c), 1e-14);
    TEST_NEAR("Corner a location", ( imesh_triangle_closest_point(p3,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p3,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p4,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Corner b point flag", i, 2);
    TEST_NEAR("Corner b point dist", dist, vgl_triangle_3d_distance(p4,a,b,c), 1e-14);
    TEST_NEAR("Corner b location", ( imesh_triangle_closest_point(p4,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p4,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p5,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Edge bc point flag", i, 6);
    TEST_NEAR("Edge bc point dist", dist, vgl_triangle_3d_distance(p5,a,b,c), 1e-14);
    TEST_NEAR("Edge bc location", ( imesh_triangle_closest_point(p5,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p5,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p6,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Corner c point flag", i, 4);
    TEST_NEAR("Corner c point dist", dist, vgl_triangle_3d_distance(p6,a,b,c), 1e-14);
    TEST_NEAR("Corner c location", ( imesh_triangle_closest_point(p6,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p6,a,b,c)).length(), 0.0,  1e-14);

    i = imesh_triangle_closest_point(p7,a,b,c,dist,u,v);
    std::cout << int(i) << " ("<<u<<','<<v<<") "<< dist << std::endl;
    TEST("Edge ac point flag", i, 5);
    TEST_NEAR("Edge ac point dist", dist, vgl_triangle_3d_distance(p7,a,b,c), 1e-14);
    TEST_NEAR("Edge ac location", ( imesh_triangle_closest_point(p7,a,b,c,dist)
                   - vgl_triangle_3d_closest_point(p7,a,b,c)).length(), 0.0,  1e-14);
  }
}

TESTMAIN(test_intersect);
