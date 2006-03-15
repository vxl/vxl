#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_homography2d.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector.h>


// CAUTION: NO boundary check for the purpose of efficiency
static
void
copy_matrix_at( vnl_matrix_fixed<double, 3, 3>& dest, unsigned int rp, unsigned int cp,
                vnl_matrix<double>const& src )
{
  for ( unsigned i=0; i<src.rows(); ++i )
    for ( unsigned j=0; j<src.cols(); ++j )
      dest(rp+i, cp+j) = src(i,j);
}

// CAUTION: NO boundary check for the purpose of efficiency
static
void
copy_column_vector_at( vnl_matrix_fixed<double, 3, 3>& dest, unsigned int rp, unsigned int cp,
                       vnl_vector<double>const& src )
{
  for ( unsigned i=0; i<src.size(); ++i )
    dest(rp+i, cp) = src(i);
}


bool
rgrl_internal_util_upgrade_to_homography2D( vnl_matrix_fixed<double, 3, 3>& init_H,
                                            rgrl_transformation const& cur_transform )
{

  // get initialization
  init_H.set_identity();

  if ( cur_transform.is_type( rgrl_trans_homography2d::type_id() ) ) {

    rgrl_trans_homography2d const& trans = static_cast<rgrl_trans_homography2d const&>( cur_transform );
    init_H = trans.H();
    return true;

  } else if ( cur_transform.is_type( rgrl_trans_affine::type_id() ) ) {
    rgrl_trans_affine const& trans = static_cast<rgrl_trans_affine const&>( cur_transform );
    if( trans.t().size() != 2 )
      return false;
    copy_matrix_at( init_H, 0, 0, trans.A() );
    copy_column_vector_at( init_H, 0, 2, trans.t() );
    return true;

  } else if ( cur_transform.is_type( rgrl_trans_similarity::type_id() ) ) {
    rgrl_trans_similarity const& trans = static_cast<rgrl_trans_similarity const&>( cur_transform );
    if( trans.t().size() != 2 )
      return false;
    copy_matrix_at( init_H, 0, 0, trans.A() );
    copy_column_vector_at( init_H, 0, 2, trans.t() );
    return true;

  } else if ( cur_transform.is_type( rgrl_trans_rigid::type_id() ) ) {
    rgrl_trans_rigid const& trans = static_cast<rgrl_trans_rigid const&>( cur_transform );
    if( trans.t().size() != 2 )
      return false;
    copy_matrix_at( init_H, 0, 0, trans.R() );
    copy_column_vector_at( init_H, 0, 2, trans.t() );
    return true;

  } else if ( cur_transform.is_type( rgrl_trans_translation::type_id() ) ) {
    rgrl_trans_translation const& trans = static_cast<rgrl_trans_translation const&>( cur_transform );
    if( trans.t().size() != 2 )
      return false;
    copy_column_vector_at( init_H, 0, 2, trans.t() );
    return true;

  } else {
    return false;
  }
}
