#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "vsph_segment_sphere.h"
//:
// \file
#include <cassert>
#include <vnl/vnl_math.h>
#include <vbl/vbl_graph_partition.h>
#include <vbl/vbl_edge.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void random_rgb(float& r, float&g, float& b)
{
    float rmaxinv = 1.f / RAND_MAX;
    r = static_cast<float>(std::rand())*rmaxinv;
    g = static_cast<float>(std::rand())*rmaxinv;
    b = static_cast<float>(std::rand())*rmaxinv;
}

void vsph_segment_sphere::smooth_data()
{
    seg_valid_=false;
    if (!usph_.neighbors_valid())
        usph_.find_neighbors();
    int nd = data_.size();
    if (nd==0) return;
    smooth_data_.resize(nd, 0.0);
    double point_angle_rad = usph_.point_angle()/vnl_math::deg_per_rad;
    double arg = 0.5*(point_angle_rad*point_angle_rad)/(sigma_*sigma_);
    double neigh_weight = std::exp(-arg);
    if (!usph_.neighbors_valid())
        usph_.find_neighbors();
    unsigned nv = usph_.size();
    for (unsigned i = 0; i<nv; ++i) {
        std::set<int> neighbors = usph_.neighbors(i);
        double sum = 0.0;
        sum += data_[i];
        unsigned nn = neighbors.size();
        if (nn == 0) continue;
        double weight_sum = static_cast<double>(nn)*neigh_weight + 1.0;
        for (const auto & neighbor : neighbors)
            sum += neigh_weight*data_[neighbor];

        sum /= weight_sum;
        smooth_data_[i]=sum;
    }
}

void vsph_segment_sphere::segment()
{
    if (dosmoothing_)
    {
        this->smooth_data();
    }
    else
    {
        smooth_data_ = data_;
    }
    int nd = smooth_data_.size();
    if (nd==0) return;
    std::vector<vsph_edge> sph_edges = usph_.edges();
    int ne = sph_edges.size();
    if (!usph_.neighbors_valid())
        usph_.find_neighbors();
    //construct graph partition
    std::vector<vbl_edge> edges(ne);
    for (int i = 0; i<ne; ++i) {
        vsph_edge& sphe = sph_edges[i];
        int is = sphe.vs_, ie = sphe.ve_;
        edges[i].v0_ = is; edges[i].v1_ = ie;
        double diff = smooth_data_[is]-smooth_data_[ie];
        edges[i].w_ = static_cast<float>(std::sqrt(diff*diff));
    }
    int nv = usph_.size();
    ds_.add_elements(nv);
    // segment graph
    vbl_graph_partition(ds_, edges, static_cast<float>(c_));
    // post process small components
    for (int i = 0; i < ne; i++) {
        int v0 = ds_.find_set(edges[i].v0_);
        int v1 = ds_.find_set(edges[i].v1_);
        if ((v0 != v1) && ((ds_.size(v0) < min_size_) || (ds_.size(v1) < min_size_)))
            ds_.set_union(v0, v1);
    }
    num_ccs_ = ds_.num_sets();
    for (int i = 0; i<nv; ++i) {
        int comp = ds_.find_set(i);
        regions_[comp].push_back(i);
    }
    unsigned maxsize = 0, minsize = nv+1;
    for (std::map<int,  std::vector<int> >::const_iterator rit = regions_.begin();
         rit != regions_.end(); ++rit) {
        unsigned n = rit->second.size();
        if (n<minsize) minsize = n;
        if (n>maxsize) maxsize = n;
    }
    std::cout << "Found " << num_ccs_ << " regions\n";
    std::cout << "minArea = " << minsize << "  MaxArea = " << maxsize << '\n';
    seg_valid_=true;
}

//: function to compute mean of the pixels in a region using the oringal values of the spherical segment
double vsph_segment_sphere::region_mean(int id)
{
    std::vector<int>  region = regions_[id];
    double sum = 0.0;
    for (int i : region)
        sum += data_[i];
    if (region.size() > 0)
        return sum/region.size();
    else return 0.0;
}

//: function to compute median of the pixels in a region using the oringal values of the spherical segment
double vsph_segment_sphere::region_median(int id)
{
    std::vector<double> vals;
    std::vector<int>  region = regions_[id];
    vals.reserve(region.size());
for (int i : region)
        vals.push_back( data_[i] );
    std::sort(vals.begin(), vals.end());
    if ( vals.size() > 0)
        return vals[vals.size()/2];
    else
        return 0.0;
}

std::vector<double> vsph_segment_sphere::region_data() const
{
    if (!seg_valid_) return std::vector<double>();
    std::vector<double> rdata(data_.size());
    auto rit = regions_.begin();
    for (; rit != regions_.end(); ++rit) {
        const std::vector<int>& pt_ids = rit->second;
        int n = pt_ids.size();
        auto dn = static_cast<double>(n);
        if (dn == 0.0) dn = 1.0;
        double sum = 0.0;
        for (int i = 0; i<n; ++i)
            sum += data_[pt_ids[i]];
        double mean = sum/dn;
        for (int i = 0; i<n; ++i)
            rdata[pt_ids[i]]=mean;
    }
    return rdata;
}

std::vector<std::vector<float> > vsph_segment_sphere::region_color() const
{
    if (!seg_valid_) return std::vector<std::vector<float> >();
    std::vector<std::vector<float> > cdata(data_.size());
    auto rit = regions_.begin();
    for (; rit != regions_.end(); ++rit) {
        const std::vector<int>& pt_ids = rit->second;
        float r, g, b;
        random_rgb(r, g, b);
        std::vector<float> color(3);
        color[0]=r; color[1]=g; color[2]=b;
        int n = pt_ids.size();
        for (int i = 0; i<n; ++i)
            cdata[pt_ids[i]]=color;
    }
    return cdata;
}

bool vsph_segment_sphere::extract_region_bounding_boxes()
{
    if (!seg_valid_) return false;
    if (!usph_.neighbors_valid())
        usph_.find_neighbors();
    const std::vector<vsph_sph_point_2d>& spts = usph_.sph_points_ref();
    auto rit = regions_.begin();
    for (; rit != regions_.end(); ++rit) {
        int reg_set_id = rit->first;
        std::vector<int>& rays = rit->second;
        int n = rays.size();
        int ra=-1, rb=-1, rc=-1;
        bool done = false;
        for (int i = 0; i<n&&!done; ++i) {
            int ray = rays[i];
            std::set<int> neigh = usph_.neighbors(ray);
            for (auto nit = neigh.begin();
                 nit != neigh.end()&&!done; ++nit) {
                int nid = *nit;
                int nbr_set_id = ds_.find_set(nid);
                if (reg_set_id == nbr_set_id) {
                    if (ra == -1) {
                        ra = ray;
                        rb = nid;
                    }
                    else if (rc==-1) {
                        rc = nid;
                        done = true;
                    }
                }
            }
            if (!done) {
                ra = -1; rb = -1; rc = -1;
            }
        }
        assert(done);
        assert(ra!=-1&&rb!=-1&&rc!=-1);
        // now have the three neighboring points construct the box
        vsph_sph_box_2d rbox;
        rbox.add(spts[ra]);     rbox.add(spts[rb]);     rbox.add(spts[rc]);
        for (int i = 0; i<n; ++i) {
            if (rays[i]==ra || rays[i]==rb || rays[i]==rc)
                continue;
            rbox.add(spts[rays[i]]);
        }
        double delta = usph_.point_angle()/2.0/vnl_math::deg_per_rad;
        vsph_sph_point_2d pmin (rbox.min_theta()-delta,rbox.min_phi()-delta);
        vsph_sph_point_2d pmax (rbox.max_theta()+delta,rbox.max_phi()+delta);

        rbox.add(pmin);
        rbox.add(pmax);
        bboxes_[reg_set_id]=rbox;
    }
    return true;
}
