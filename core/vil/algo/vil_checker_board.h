// This is core/vil/algo/vil_checker_board.h
#ifndef vil_checker_board_h_
#define vil_checker_board_h_
//:
// \file
// \brief Build checker board image from two source
// \author Gehua Yang

#include<vil/vil_fwd.h>

//: parameters for building checkboard
class vil_checker_board_params
{
private:
  int xsize_, ysize_;
  int xnum_blks_, ynum_blks_;
  int xoff_, yoff_;
  bool use_alpha_plane_;
  bool not_use_color_flag_;
  int  not_use_color_;
public:
  vil_checker_board_params()
  : xsize_(-1), ysize_(-1), xnum_blks_(0), ynum_blks_(0),
    xoff_(0), yoff_(0),
    use_alpha_plane_(false),
    not_use_color_flag_(false), not_use_color_(0)
  {
  }

  vil_checker_board_params( int xsize, int ysize )
  : xsize_(xsize), ysize_(ysize), xnum_blks_(0), ynum_blks_(0),
    xoff_(0), yoff_(0),
    use_alpha_plane_(false),
    not_use_color_flag_(false), not_use_color_(0)
  { 
  }
    
  void set_offset( int xoff, int yoff )
  { xoff_=xoff;  yoff_=yoff; }
  
  void use_alpha_plane( bool use )
  { use_alpha_plane_ = use; }; 

  void not_use_color( int c )
  { not_use_color_ = c;  not_use_color_flag_ = true; }
  
  template< typename DEST, typename SRC >
  friend
  void 
  vil_checker_board( vil_image_view< DEST >& des, 
                     vil_image_view< SRC > const& src1,
                     vil_image_view< SRC > const& src2,
                     vil_checker_board_params const& params );
  
};


//: build a checker board image from two images
//  params specify the block size/offset, transpanrancy
template< typename DEST, typename SRC >
void 
vil_checker_board( vil_image_view< DEST >& des, 
                   vil_image_view< SRC > const& src1,
                   vil_image_view< SRC > const& src2,
                   vil_checker_board_params const& params );


#endif // vil_checker_board_h_
