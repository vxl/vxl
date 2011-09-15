//this is /contrib/bvpl/bvpl_bundler_features_2d.h
#ifndef BVPL_BUNDLER_FEATURES_2D_H_
#define BVPL_BUNDLER_FEATURES_2D_H_
//:
// \file
// \brief A class to store feature vectors derived from 
// 3d bundler points.
// \author Brandon A. Mayer b.mayer1@gmail.com
// \date 8-September-2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include<vbl/vbl_ref_count.h>
#include<vbl/vbl_smart_ptr.h>
#include<vcl_map.h>
#include<vgl/vgl_point_3d.h>
#include<vnl/vnl_vector.h>
#include<vsl/vsl_binary_io.h>

class bvpl_vgl_point_3d_coord_compare
{
  public:
    bvpl_vgl_point_3d_coord_compare() {}
    ~bvpl_vgl_point_3d_coord_compare() {}

    bool operator() ( vgl_point_3d<double> const& pa, vgl_point_3d<double> pb ) const
    {
        if      ( pa.x() != pb.x() ) return pa.x() < pb.x();
        else if ( pa.y() != pb.y() ) return pa.y() < pb.y();
        else                         return pa.z() < pb.z();
    }
};


class bvpl_bundler_features_2d: public vbl_ref_count
{
  public:
    typedef vcl_map< vgl_point_3d<double>, 
        vcl_map<unsigned, vnl_vector<double> >, 
        bvpl_vgl_point_3d_coord_compare > point_view_feature_map_type;

    bvpl_bundler_features_2d() {}

    ~bvpl_bundler_features_2d() {}

    void b_write( vsl_b_ostream& os ) const;

    void b_read( vsl_b_istream& is );

    void write_txt( vcl_string const& filename ) const;

    void write_feature_txt( vcl_string const& filename ) const;

    //: relation: 3d point - view number - feature vector
    point_view_feature_map_type pt_view_feature_map;
};

typedef vbl_smart_ptr<bvpl_bundler_features_2d> bvpl_bundler_features_2d_sptr;

#endif //BVPL_BUNDLER_FEATURES_2D_H_
