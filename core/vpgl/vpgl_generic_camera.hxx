// This is core/vpgl/vpgl_generic_camera.hxx
#ifndef vpgl_generic_camera_hxx_
#define vpgl_generic_camera_hxx_
//:
// \file

#include <cmath>
#include <iostream>
#include "vpgl_generic_camera.h"
#include <vnl/vnl_numeric_traits.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_plane_3d.h>
#include <vnl/vnl_math.h>

//-------------------------------------------
template <class T>
vpgl_generic_camera<T>::vpgl_generic_camera()
{
    // rays_ is empty and min ray and max ray origins are (0 0 0)
}

//------------------------------------------
template <class T>
vpgl_generic_camera<T>::
    vpgl_generic_camera( vbl_array_2d<vgl_ray_3d<T> > const& rays)
{
    unsigned long nc = rays.cols();
  unsigned long nr = rays.rows();
  assert(nc>0&&nr>0);
    //compute bounds on ray origins
    double min_dist = vnl_numeric_traits<double>::maxval;
    double max_dist = 0.0;
    vgl_point_3d<T> datum(T(0), T(0), T(0));
    for (int v = 0; v<nr; ++v)
        for (int u = 0; u<nc; ++u) {
            vgl_point_3d<T> org = rays[v][u].origin();
            double d = vgl_distance(datum, org);
            if (d>max_dist) {
                max_dist = d;
                max_ray_origin_ = org;
                max_ray_direction_ = rays[v][u].direction();
            }
            if (d<min_dist) {
                min_dist = d;
                min_ray_origin_ = org;
                min_ray_direction_ = rays[v][u].direction();
            }
        }
        // form the pyramid for efficient projection
        // find the number of levels
        double dim = nc;
        if (nr<nc)
            dim = nr;
        double lv = std::log(dim)/std::log(2.0);
        n_levels_ = static_cast<int>(lv);// round down
        if (dim*std::pow(0.5, static_cast<double>(n_levels_-1)) < 32.0) n_levels_--;
        if (n_levels_<=0) n_levels_ = 1;
        rays_.resize(n_levels_);
        nr_.resize(n_levels_);
        nc_.resize(n_levels_);
        rays_[0]=rays;
        nr_[0]=nr; nc_[0]=nc;
        int nrlv = (nr)/2, nclv = (nc)/2;
        for (int lev = 1; lev<n_levels_; ++lev) {
            rays_[lev].resize(nrlv, nclv);
            nr_[lev]=nrlv; nc_[lev]=nclv;
            for (int r = 0; r<nrlv; ++r)
                for (int c = 0; c<nclv; ++c)// nearest neighbor downsampling
                    rays_[lev][r][c] = rays_[lev-1][2*r][2*c];
            //next level
            nrlv =(nrlv) / 2; nclv = (nclv) / 2;
        }
}
//------------------------------------------
template <class T>
vpgl_generic_camera<T>::
    vpgl_generic_camera( std::vector<vbl_array_2d<vgl_ray_3d<T> > > const& rays,
    std::vector<int> nrs,   std::vector<int> ncs  )
{
    assert(rays.size()>0 && nrs.size()>0 && ncs.size()>0);
    //compute bounds on ray origins
    double min_dist = vnl_numeric_traits<double>::maxval;
    double max_dist = 0.0;
    vgl_point_3d<T> datum(T(0), T(0), T(0));
    for (int v = 0; v<nrs[0]; ++v) {
        for (int u = 0; u<ncs[0]; ++u) {
            vgl_point_3d<T> org = rays[0][v][u].origin();
            double d = vgl_distance(datum, org);
            if (d>max_dist) {
                max_dist = d;
                max_ray_origin_ = org;
                max_ray_direction_ = rays[0][v][u].direction();
            }
            if (d<min_dist) {
                min_dist = d;
                min_ray_origin_ = org;
                min_ray_direction_ = rays[0][v][u].direction();
            }
        }
    }
    rays_ = rays;
    nr_ = nrs;
    nc_ = ncs;
    n_levels_ = rays.size();
}
// the ray closest to the given 3-d point is selected
// note that the ray is taken to be an infinite 3-d line
// and so the bound of the ray origin is not taken into account
//
template <class T>
void vpgl_generic_camera<T>::nearest_ray(int level,
                                         vgl_point_3d<T> const& p,
                                         int start_r, int end_r,
                                         int start_c, int end_c,
                                         int& nearest_r, int& nearest_c) const
{
    assert(level>=0 && level<n_levels_);
    assert(start_r>=0 && end_r < nr_[level]);
    assert(start_c>=0 && end_c < nc_[level]);
    nearest_r = 0, nearest_c = 0;
    double min_d = vnl_numeric_traits<double>::maxval;
    for (int r = start_r; r<=end_r; ++r)
        for (int c = start_c; c<=end_c; ++c) {
            double d = vgl_distance(rays_[level][r][c], p);
            if (d<min_d) {
                min_d=d;
                nearest_r = r;
                nearest_c = c;
            }
        }
}

template <class T>
void vpgl_generic_camera<T>::
    nearest_ray_to_point(vgl_point_3d<T> const& p,
    int& nearest_r, int& nearest_c) const
{
    int lev = n_levels_-1;
    int start_r = 0, end_r = nr_[lev];
    int start_c = 0, end_c = nc_[lev];
    for (; lev >= 0; --lev) {
        if (start_r<0) start_r = 0;
        if (start_c<0) start_c = 0;
        if (end_r>=nr_[lev]) end_r = nr_[lev]-1;
        if (end_c>=nc_[lev]) end_c = nc_[lev]-1;
        nearest_ray(lev, p, start_r, end_r, start_c, end_c,
            nearest_r, nearest_c);
        // compute new bounds
        start_r = 2*nearest_r-1; start_c = 2*nearest_c-1;
        end_r = start_r + 2; end_c = start_c +2;
        // check if the image sizes are odd, so search range is extended
        if ( (lev > 0) && (nr_[lev-1]%2 != 0) ) end_r++;
        if ( (lev > 0) && (nc_[lev-1]%2 != 0) ) end_c++;
    }
}

template <class T>
void vpgl_generic_camera<T>::
    refine_ray_at_point(int nearest_c, int nearest_r,
    vgl_point_3d<T> const& p,
    vgl_ray_3d<T>& ray) const
{
    T u = static_cast<T>(nearest_c), v = static_cast<T>(nearest_r);
    ray = this->ray(u, v);
    vgl_point_3d<T> cp = vgl_closest_point(p, ray);
    vgl_vector_3d<T> t = p-cp;
    vgl_point_3d<T> org = ray.origin();
    org += t; //shift origin by vector from closest point, cp to p
    ray.set(org, ray.direction());
}

//: a ray passing through a given 3-d point
template <class T>
vgl_ray_3d<T> vpgl_generic_camera<T>::
    ray(vgl_point_3d<T> const& p) const
{
    int nearest_c = -1, nearest_r = -1;
    this->nearest_ray_to_point(p, nearest_r, nearest_c);
    vgl_ray_3d<T> r;
    this->refine_ray_at_point(nearest_c, nearest_r, p, r);
    return r;
}

// refine the projection to sub-pixel accuracy
// use an affine invariant map between the plane passing through p and the
// image plane. The plane normal is given by the direction of the
// nearest ray to p, but negated, so as to point upward.
template <class T>
void vpgl_generic_camera<T>::
    refine_projection(int nearest_c, int nearest_r, vgl_point_3d<T> const& p,
    T& u, T& v) const
{
    // the ray closest to the projected 3-d point
    vgl_ray_3d<T> nr = rays_[0][nearest_r][nearest_c];
    // construct plane with normal given by -nr.direction() through p
    vgl_plane_3d<T> pl(-nr.direction(), p);
    bool valid_inter = true;
    // find intersection of nearest ray with the plane
    std::vector<vgl_point_3d<T> > inter_pts;
    std::vector<vgl_point_2d<T> > img_pts;
    vgl_point_3d<T> ipt;
    valid_inter = vgl_intersection(nr, pl, ipt);
    inter_pts.push_back(ipt);
    //find intersections of neighboring rays with the plane
    //need at least two neighbors
    img_pts.push_back(vgl_point_2d<T>(0.0, 0.0));
    bool horiz = false;
    bool vert = false;
    if (nearest_r>0 && !horiz) {
        vgl_ray_3d<T> r = rays_[0][nearest_r-1][nearest_c];
        valid_inter = vgl_intersection(r, pl, ipt);
        if(std::fabs((ipt-inter_pts[0]).length())  > vnl_math::eps)
        {
            inter_pts.push_back(ipt);
            img_pts.push_back(vgl_point_2d<T>(0.0, -1.0));
            horiz = true;
        }
    }
    if (nearest_c>0 && !vert) {
        vgl_ray_3d<T> r = rays_[0][nearest_r][nearest_c-1];
        valid_inter = vgl_intersection(r, pl, ipt);
        if(std::fabs((ipt-inter_pts[0]).length())  > vnl_math::eps)
        {
            inter_pts.push_back(ipt);
            img_pts.push_back(vgl_point_2d<T>(-1.0, 0.0));
            vert = true;
        }
    }
    int nrght = static_cast<int>(cols())-1;
    if (nearest_c<nrght && !vert ) {
        vgl_ray_3d<T> r = rays_[0][nearest_r][nearest_c+1];
        valid_inter = vgl_intersection(r, pl, ipt);
        if(std::fabs((ipt-inter_pts[0]).length())  > vnl_math::eps)
        {
            inter_pts.push_back(ipt);
            img_pts.push_back(vgl_point_2d<T>(1.0, 0.0));
            vert = true;
        }
    }
    int nbl = static_cast<int>(rows())-1;
    if (nearest_r<nbl && !horiz ) {
        vgl_ray_3d<T> r = rays_[0][nearest_r+1][nearest_c];
        valid_inter = vgl_intersection(r, pl, ipt);
        if(std::fabs((ipt-inter_pts[0]).length())  > vnl_math::eps)
        {
            inter_pts.push_back(ipt);
            img_pts.push_back(vgl_point_2d<T>(0.0, 1.0));
            horiz = true;
        }
    }
    //less than two neighbors, shouldn't happen!
    if (!valid_inter||inter_pts.size()<3) {
        u = static_cast<T>(nearest_c);
        v = static_cast<T>(nearest_r);
        return;
    }
    // compute 2-d plane coordinates for points
    vgl_point_3d<T> p0 = inter_pts[0];// origin
    // coordinate axes
    vgl_vector_3d<T> v0 = inter_pts[1]- p0;
    vgl_vector_3d<T> v1 = inter_pts[2]- p0;
    vgl_vector_3d<T> vp = p-p0;
    // compute coordinates of p in the plane
    T v0v0 = dot_product(v0,v0);
    T v0v1 = dot_product(v0,v1);
    T v1v1 = dot_product(v1,v1);
    T one_over_det = static_cast<T>(1)/(v0v0*v1v1 - v0v1*v0v1);
    // b0 and b1 are rows of coordinate transformation matrix
    vgl_vector_3d<T> b0 = one_over_det * (v1v1*v0 - v0v1*v1);
    vgl_vector_3d<T> b1 = one_over_det * (v0v0*v1 - v0v1*v0);
    // x0,x1 are coordinates of p in the plane
    T x0 = dot_product(b0, vp), x1 = dot_product(b1, vp);
    // in image space
    vgl_point_2d<T> ip0 = img_pts[0];
    vgl_vector_2d<T> iv0 = img_pts[1]-ip0;
    vgl_vector_2d<T> iv1 = img_pts[2]-ip0;
    vgl_vector_2d<T>  del = x0*iv0 + x1*iv1;
    u = nearest_c + del.x();
    v = nearest_r + del.y();
}

// projects by exhaustive search in a pyramid.
template <class T>
void vpgl_generic_camera<T>::project(const T x, const T y, const T z,
                                     T& u, T& v) const
{
    vgl_point_3d<T> p(x, y, z);
    int nearest_c=-1, nearest_r=-1;
    this->nearest_ray_to_point(p, nearest_r, nearest_c);
    // refine to sub-pixel accuracy using a Taylor series approximation
    this->refine_projection(nearest_c, nearest_r, p, u, v);
}


// a ray specified by an image location (can be sub-pixel)
template <class T>
vgl_ray_3d<T> vpgl_generic_camera<T>::ray(const T u, const T v) const
{
    double du = static_cast<double>(u);
    double dv = static_cast<double>(v);
    int nright = static_cast<int>(cols())-1;
    int nbelow = static_cast<int>(rows())-1;
    if( ! (du>=-0.5 && dv>=-0.5 && du<=nright+0.5 && dv<=nbelow+0.5) ) {
        assert(false);
        return vgl_ray_3d<T>();
    }
    int iu, iv;
    iu = du<nright ? static_cast<int>(du) : nright-1;
    iv = dv<nbelow ? static_cast<int>(dv) : nbelow-1;
    //check for integer pixel coordinates
    if ((du-iu) == 0.0 && (dv-iv) == 0.0)
        return rays_[0][iv][iu];
    // u or v is sub-pixel so find interpolated ray
    //find neighboring rays and pixel distances to the sub-pixel location
    std::vector<double> dist;
    std::vector<vgl_ray_3d<T> > nrays;
    // closest ray (the lower left corner pixel in this case)
    vgl_ray_3d<T> ru = rays_[0][iv][iu];
    nrays.push_back(ru);
    double d = (1 - (dv-iv))*(1 - (du-iu));
    dist.push_back(d);
    if(iu<nright) {
        // ray to the right
        vgl_ray_3d<T> rr = rays_[0][iv][iu+1];
        nrays.push_back(rr);
        double d = (1 - (dv-iv))*(1 - (iu+1-du));
        dist.push_back(d);
    }
    if(iv<nbelow) {
        // ray below
        vgl_ray_3d<T> rd = rays_[0][iv+1][iu];
        nrays.push_back(rd);
        double d = (1 - (iv+1-dv))*(1 - (du-iu));
        dist.push_back(d);
    }
    if(iu<nright && iv<nbelow) {
        // ray to the lower-right diagonal
        vgl_ray_3d<T> rlr = rays_[0][iv+1][iu+1];
        nrays.push_back(rlr);
        double d = (1 - (iv+1-dv))*(1 - (iu+1-du));
        dist.push_back(d);
    }
    assert(dist.size() == 4);
    // compute the interpolated ray
    double ox = 0.0, oy = 0.0, oz = 0.0, dx = 0.0, dy = 0.0, dz = 0.0;
    for (unsigned i = 0; i<nrays.size(); ++i) {
        vgl_ray_3d<T> r = nrays[i];
        vgl_point_3d<T> org = r.origin();
        vgl_vector_3d<T> dir = r.direction();
        double w = dist[i];
        ox += w*org.x(); oy += w*org.y(); oz += w*org.z();
        dx += w*dir.x(); dy += w*dir.y(); dz += w*dir.z();
    }
    vgl_point_3d<T> avg_org(static_cast<T>(ox),
        static_cast<T>(oy),
        static_cast<T>(oz));
    vgl_vector_3d<T> avg_dir(static_cast<T>(dx),
        static_cast<T>(dy),
        static_cast<T>(dz));
    return vgl_ray_3d<T>(avg_org, avg_dir);
}


template <class T>
void vpgl_generic_camera<T>::print_orig(int level)
{
    for (int r = 0; r<nr_[level]; ++r) {
        for (int c = 0; c<nc_[level]; ++c) {
            vgl_point_3d<T> o = rays_[level][r][c].origin();
            std::cout << '(' << o.x() << ' ' << o.y() << ") ";
        }
        std::cout << '\n';
    }
}

template <class T>
void vpgl_generic_camera<T>::print_to_vrml(int level, std::ostream& os)
{
    for (int r = 0; r<nr_[level]; ++r) {
        for (int c = 0; c<nc_[level]; ++c) {
            vgl_point_3d<T> o = rays_[level][r][c].origin();
            os<< "Transform {\n"
                << "translation " << o.x() << ' ' << o.y() << ' '
                << ' ' << o.z() << '\n'
                << "children [\n"
                << "Shape {\n"
                << " appearance DEF A1 Appearance {\n"
                << "   material Material\n"
                << "    {\n"
                << "      diffuseColor " << 1 << ' ' << 0 << ' ' << 0 << '\n'
                << "      emissiveColor " << .3 << ' ' << 0 << ' ' << 0 << '\n'
                << "    }\n"
                << "  }\n"
                << " geometry Sphere\n"
                << "{\n"
                << "  radius " << 20 << '\n'
                << "   }\n"
                << "  }\n"
                << " ]\n"
                << "}\n";
        }
    }
}

// Code for easy instantiation.
#undef vpgl_GENERIC_CAMERA_INSTANTIATE
#define vpgl_GENERIC_CAMERA_INSTANTIATE(T) \
    template class vpgl_generic_camera<T >


#endif // vpgl_generic_camera_hxx_
