#ifndef jvx_manager_h_
#define jvx_manager_h_
//--------------------------------------------------------------------------------
// .NAME jvx_manager - Video player
// .SECTION Description:
//   the manager for playing video sequences
// .SECTION Author
//   J.L. Mundy
// .SECTION Modifications:
//   J.L. Mundy Apr 14, 2002    Initial version.
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D.h>
#include <vidl/vidl_movie.h>
class jvx_manager : public vgui_grid_tableau
{
 public:
  jvx_manager();
  ~jvx_manager();
  virtual bool handle(const vgui_event&);
  unsigned get_height(){return _height;}
  unsigned get_width(){return _width;}
  void load_video_file();
  void play();
  void play_video();
  void stop_video();
  void go_to_frame();
  void next_frame();
  void prev_frame();
  void set_speed();
 protected:
  vgui_viewer2D_sptr get_vgui_viewer2D_at(unsigned col, unsigned row);
 private:
  unsigned _width;
  unsigned _height;
  vidl_movie_sptr _my_movie;
  vcl_vector<vgui_viewer2D_sptr> _tabs;
};
#endif // jvx_manager_h_
