#ifndef vplayer_file_h_
#define vplayer_file_h_

//--------------------------------------------------------------------------------
// .NAME	vplayer_file
// .SECTION Description:
//   File menu implementation
//
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic    05-SEP-2000    Initial version.
//--------------------------------------------------------------------------------

class vgui_menu;

class vplayer_file
{
  static void show_frame(int frame);
public:
  static void load_video_sequence();
  static void load_video_file();
  static void save_video_sequence();
  static void load_geometry_sequence();
  static void save_geometry_sequence();
  static void exit_vplayer();
  static vgui_menu create_file_menu();
private:
};

#endif // vplayer_file_
