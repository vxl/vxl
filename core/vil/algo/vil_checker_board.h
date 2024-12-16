// This is core/vil/algo/vil_checker_board.h
#ifndef vil_checker_board_h_
#define vil_checker_board_h_
//:
// \file
// \brief Build checker board image from two source
// \author Gehua Yang

#include <vil/vil_fwd.h>

//: parameters for building checkboard
class vil_checker_board_params
{
private:
  int xsize_{ -1 }, ysize_{ -1 };
  int xnum_blks_{ 0 }, ynum_blks_{ 0 };
  int xoff_{ 0 }, yoff_{ 0 };
  bool use_alpha_plane_{ false };
  bool not_use_color_flag_{ false };
  int not_use_color_{ 0 };

public:
  vil_checker_board_params()

    = default;

  vil_checker_board_params(int xsize, int ysize)
    : xsize_(xsize)
    , ysize_(ysize)
    , xnum_blks_(0)
    , ynum_blks_(0)
    , xoff_(0)
    , yoff_(0)
    , use_alpha_plane_(false)
    , not_use_color_flag_(false)
    , not_use_color_(0)
  {}

  void
  set_offset(int xoff, int yoff)
  {
    xoff_ = xoff;
    yoff_ = yoff;
  }

  void
  use_alpha_plane(bool use)
  {
    use_alpha_plane_ = use;
  }

  void
  not_use_color(int c)
  {
    not_use_color_ = c;
    not_use_color_flag_ = true;
  }

  template <typename DEST, typename SRC>
  friend void
  vil_checker_board(vil_image_view<DEST> & des,
                    const vil_image_view<SRC> & src1,
                    const vil_image_view<SRC> & src2,
                    const vil_checker_board_params & params);
};


//: build a checker board image from two images
//  \param params specify the block size/offset, transparency
template <typename DEST, typename SRC>
void
vil_checker_board(vil_image_view<DEST> & des,
                  const vil_image_view<SRC> & src1,
                  const vil_image_view<SRC> & src2,
                  const vil_checker_board_params & params);


#endif // vil_checker_board_h_
