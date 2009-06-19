#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vil1/vil1_memory_image.h>
#include <vil1/vil1_save.h>
#include <oufgl/frame_grabber_v4l.h>
#include <vcl_iostream.h>
#include <getopt.h>
#include <vul/vul_timer.h>

int main(int argc, char *argv[])
{
  extern char *optarg;
  int opt;
  bool debug=false;
  int width=320, height=240;
  int num_to_grab=1;

  const char * camera_name_default = "/dev/video0";
  char * camera_name = new char[vcl_strlen(camera_name_default)+1];
  vcl_strcpy (camera_name, camera_name_default);

  while ((opt=getopt(argc, argv, "x:y:n:c:d")) != -1)
  {
    switch (opt)
    {
     case 'x':
      width = vcl_atoi(optarg);
      break;
     case 'y':
      height = vcl_atoi(optarg);
      break;
     case 'n':
      num_to_grab = vcl_atoi(optarg);
      break;
     case 'c':
      camera_name = new char[vcl_strlen(optarg)+1];
      vcl_strcpy (camera_name, optarg);
      break;
     case 'd':
      debug = !debug;
      break;
     default:
      vcl_cout << "Usage: example1 [-x width] [-y height] [-n num_to_grab] [-c device_name] [-d]\n"
               << "\t-d turn debug info on" << vcl_endl;
      break;
    }
  }
  FrameGrabberV4lGrey fg(width, height, debug, camera_name);

  vul_timer t;

  for (int i=0; i<num_to_grab; i++)
    fg.acquire_frame_synch();

  long time_taken = t.real();

  vcl_cout << "That took " << time_taken << " ms\n"
           << "which means "<< 1000.0*(double)num_to_grab/(double)time_taken
           << " frames per second\n";

  // only save the last one
  vil1_memory_image *img = fg.get_current_frame();
  vil1_save(*img, "image.pgm");
}
