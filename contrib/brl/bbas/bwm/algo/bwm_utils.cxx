#include "bwm_utils.h"
#include <vgui/vgui_dialog.h>

vcl_string bwm_utils::select_file()
{
  vgui_dialog params ("File Open");
  vcl_string ext, file, empty="";

  params.file ("Open...", ext, file);  
  if (!params.ask())
    return empty;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return empty;
  }
  return file;
}