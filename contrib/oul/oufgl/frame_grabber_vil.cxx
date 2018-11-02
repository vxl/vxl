#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include "frame_grabber_vil.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Constructor
// Sets up the framegrabber.

using namespace std;

FrameGrabberVil::FrameGrabberVil(const string &device_name,
                                 int width, int height)
{
  fd = open(device_name.c_str(), O_RDWR);
  if (fd<0)
  {
    std::perror("Couldn't open device");
    std::exit(-1);
  }

  ioctl(fd, VIDIOCSCHAN, 1);

  struct video_capability vcap;
  ioctl(fd, VIDIOCGCAP, &vcap);
  std::cout << "camera name = " << vcap.name << std::endl
           << "max image size = " << vcap.maxwidth << ' '
           << vcap.maxheight << std::endl;
  if (vcap.type && VID_TYPE_CAPTURE)
    std::cout << "can capture\n";
  else std::cout << "can't capture\n";

  struct video_picture vp;
  ioctl(fd, VIDIOCGPICT, &vp);
  std::cout << "vp.pallette = " << vp.palette << std::endl;

  vp.palette = VIDEO_PALETTE_YUV420P;
  if (ioctl(fd, VIDIOCSPICT, &vp)>=0)
    std::cout << "Successfully set palette to YUV420P\n";
  else
  {
    std::perror("Error setting pallette\n");
    std::cerr << "Capture may not work\n";
  }
  struct video_window vw;
  ioctl(fd, VIDIOCGWIN, &vw);
  vw.x = vw.y = 0;
  vw.width = width;
  vw.height = height;
  std::cout << "trying to set to window = " << vw.x << ' ' << vw.y
           << ' ' << vw.width << ' ' << vw.height << std::endl;
  // try setting the image size
  ioctl(fd, VIDIOCSWIN, &vw);
  // now read the actual size back
  ioctl(fd, VIDIOCGWIN, &vw);
  std::cout << "actually setting window to = " << vw.x << ' ' << vw.y
           << ' ' << vw.width << ' ' << vw.height << std::endl;
  // set the size to the actual size
  width = vw.width;
  height = vw.height;

  // Query the actual buffers available
  if (ioctl(fd, VIDIOCGMBUF, &vm) < 0) {
    std::perror("VIDIOCGMBUF");
    std::exit(-1);
  }
  std::cout << "vm.size = " << vm.size << " vm.frames = "
           << vm.frames << std::endl;
  vm.frames = 1;

  // MMap all available buffers
  bigbuf=(unsigned char*)mmap(0,vm.size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  if (bigbuf==(unsigned char *)-1) {
    std::perror("mmap");
    std::exit(-1);
  }

  mm.height = height; /* Your own height */
  mm.width  = width;  /* Your own width */
  mm.format = VIDEO_PALETTE_YUV420P; /* Your own format */
  mm.frame = 0;

  typedef vil_image_view<vxl_byte>* Type;
  frame = new Type[vm.frames];

  for (int i=0; i<vm.frames; i++)
  {
    std::cout << "offset = " << vm.offsets[i] << std::endl;
    frame[i] =
      new vil_image_view<vxl_byte>(bigbuf+vm.offsets[i],width,height,
                                   1,1,width,1);
  }
#if 0
  if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) {
    std::perror("VIDIOCMCAPTURE");
    std::exit(-1);
  }
#endif
}

FrameGrabberVil::~FrameGrabberVil()
{
  close(fd);
  delete frame;
  munmap(bigbuf, vm.size);
}

vil_image_view<vxl_byte> *FrameGrabberVil::grab_frame()
{
  int frame_num = mm.frame;
  mm.frame = (mm.frame+1)%vm.frames;
  if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) {
    std::perror("VIDIOCMCAPTURE");
    std::exit(-1);
  }

  int i = -1;
  while (i < 0) {
    i = ioctl(fd, VIDIOCSYNC, &frame_num);
    if (i < 0 && errno == EINTR)
    {
      std::perror("VIDIOCSYNC problem");
      continue;
    }
    if (i < 0) {
      std::perror("VIDIOCSYNC");
      // You may want to exit here, because something has gone
      // pretty badly wrong...
    }
    break;
  }
  return frame[i];
}
