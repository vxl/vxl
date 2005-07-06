// This is core/vil/algo/vil_checker_board.txx
#ifndef vil_checker_board_txx_
#define vil_checker_board_txx_
//:
// \file
// \brief Build checker board image from two source
// \author Gehua Yang

#include<vil/algo/vil_checker_board.h>
#include<vil/vil_image_view.h>
#include<vcl_cassert.h>

template< typename DEST, typename SRC >
void 
vil_checker_board( vil_image_view< DEST >& des, 
                   vil_image_view< SRC > const& src1,
                   vil_image_view< SRC > const& src2,
                   vil_checker_board_params const& params )
{
  assert( src1.ni() == src2.ni() && src1.nj() == src2.nj() && 
          src1.nplanes() == src2.nplanes() );
  // has to be signed because the offset can be negative
  //
  const int ni=(signed)src1.ni(); 
  const int nj=(signed)src1.nj(); 
  const int np=(signed)src1.nplanes();
  
  // If desc image is empty, create one 
  if( !des ) {
    des.set_size( ni, nj, np );
    des.fill( DEST(0) );
  }
  assert( des.ni() == src1.ni() && des.nj() == src1.nj() && 
          des.nplanes() == src1.nplanes() );
  
  int xsize=params.xsize_;
  int ysize=params.ysize_;
  if( xsize<0 || ysize<0 ) {
    
    if( !params.xnum_blks_ || !params.ynum_blks_ ) {
      // no enough information
      return;
    }
    
    xsize = ni / params.xnum_blks_;
    ysize = nj / params.ynum_blks_;
  }
  
  for( int i=0; i<ni; ++i ) {
    
    // block on i index
    int iblock = ( (i+params.xoff_) / xsize ) % 2;
    
    for( int j=0; j<nj; ++j ) {

      // block on j index
      int jblock = ( (j+params.yoff_) / ysize + iblock ) % 2;
      
      // check the pixel availablility
      bool p1 = true, p2 = true;
      
      if( params.not_use_color_flag_ ) {
        
        p1 = false; p2 = false;
        for(int p=0; p<np; ++p ){
          p1 = p1 || ( !src1(i, j, p ) == params.not_use_color_ );
          p2 = p2 || ( !src2(i, j, p ) == params.not_use_color_ );
        }
      }
      
      if( params.use_alpha_plane_ ) {
        
        p1 = p1 && ( src1(i, j, np-1) == 0 );
        p2 = p2 && ( src2(i, j, np-1) == 0 );
      }
      
      // if pixel on (jblock) image is not available,
      // use the other
      if( jblock ) {      // == 1
        if( p1 && !p2 )
          jblock = 0;
      } else {             // == 0 
        if( !p1 && p2 )
          jblock = 1;
      }
      
      // fill pixel
      if( !jblock )     // == 0
        for( int p=0; p<np; ++p )
          des(i, j, p) = src1(i, j, p);
      else              // == 0
        for( int p=0; p<np; ++p )
          des(i, j, p) = src2(i, j, p);

    }
  }

}

#undef VIL_CHECKER_BOARD_INSTANTIATE
#define VIL_CHECKER_BOARD_INSTANTIATE(srcT, desT) \
template void  \
vil_checker_board( vil_image_view< desT >& des,   \
                   vil_image_view< srcT > const& src1, \
                   vil_image_view< srcT > const& src2, \
                   vil_checker_board_params const& params );

#endif // vil_checker_board_txx_
