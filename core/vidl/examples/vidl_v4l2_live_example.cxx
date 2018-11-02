// This is core/vidl/examples/vidl_v4l2_live_example.cxx

#include <iostream>
#include <vidl/vidl_v4l2_devices.h>
#include <vidl/vidl_v4l2_istream.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int select_device(vidl_v4l2_devices& devs)
{
  std::cout << "Looking for devices...\n"
           << "Numbers of found devices: " << devs.size() << std::endl;

  for (unsigned int i=0; i< devs.size(); ++i) {
    std::cout << "DEVICE  " << i << ":\n" <<  devs(i);
  }

  int dnum=-1; // device selected

  if (devs.size()==0)
    std::cout << "No devices." << std::endl;
  else if (devs.size()==1) {
    dnum=0;
  }
  else {
    do {
      std::cout << "Select Device Number: ";
      std::cin >> dnum;
    } while (dnum<0 || dnum>=(int) devs.size());
  }
  return dnum;
}

void change_width_height(vidl_v4l2_device& dev)
{
  int nw,nh;
  std::cout << "Enter new width: ";
  std::cin >> nw;
  std::cout << "Enter new height: ";
  std::cin >> nh;
  // same format but try to change width/height
  if (!dev.set_v4l2_format(dev.get_v4l2_format(),nw,nh)) {
    std::cout << "New format is not set" << std::endl;
  }
  else std::cout << "New format set" << std::endl; // w,h could be changed by driver

  std::cout << "Using device:" << dev;
}

// return if success
bool configure_input(vidl_v4l2_device& dev)
{
  int inum=-1;
  if (dev.n_inputs()==1)
    inum= 0;
  else
    do {
      std::cout << "Select Input Number: ";
      std::cin >> inum;
    } while (inum<0 || inum>=(int)dev.n_inputs());
  if (!dev.set_input(inum))  {
    std::cerr << "Input " << inum << " not set\n";
    return false;
  }
  else {
    std::cout << "Input: " << dev.input(inum).name() << " selected." << std::endl;
    dev.try_formats();
    if (! dev.format_is_set()) {
      std::cout << "A default format has not been set." << std::endl;
      if (dev.set_v4l2_format(V4L2_PIX_FMT_YUV420,640,480)) { // for example
        std::cout << "  V4L2_PIX_FMT_YUV420 format is possible." << std::endl;
      }
      return false;
    }
    if (!dev) {
      std::cerr <<"Error in device: " <<dev.get_error() << std::endl;
      return false;
    }
    else {
      change_width_height(dev);
      return true;
    }
  }
}


void save_frames(vidl_v4l2_device& dev)
{
  vidl_v4l2_istream test(dev );

  //dev.start_capturing();
  if (test.is_valid()) {
    vidl_image_list_ostream test_out("./dump","%05d","ppm");

    vul_timer timer;
    int i = 100;
    std::cout << "Grabbing " << i << " frames in directory ./dump" << std::endl;
    while ( i-- && test.advance())
    {
      if (dev.get_control_id(V4L2_CID_BRIGHTNESS))
        dev.get_control_integer_id(V4L2_CID_BRIGHTNESS)->set_100(i);
      std::cout << "Grabbing: " <<i<<"(frame number: " << test.frame_number() << ")\n"
               << "  Device frame number: " << dev.sequence() << std::endl;
      if (!test_out.write_frame(test.current_frame()))
        std::cerr << "Couldn't write frame\n";
    }
    timer.print(std::cout);
  }
  else {
    std::cerr << "Couldn't use stream\n";
    if (!dev)
      std::cerr <<"Error in device: " <<dev.get_error() << std::endl;
  }
}


int main()
{
  vidl_v4l2_devices& devs= vidl_v4l2_devices::all();  // simpler name

  int dnum=select_device(devs);;

  if (dnum>=0) {
    if (configure_input(devs(dnum))) {
      for (int i=0;i<devs(dnum).n_controls(); ++i)
        std::cout << i << "->" << devs(dnum).get_control(i)->description() << std::endl;
      save_frames(devs(dnum));
    }
  }
#if 0
  else {
    std::string name;
    std::cout << "Select device name: ";
    std::cin >> name;
    vidl_v4l2_device dev(name.c_str());
    if (dev)
      save_frames(dev);
  }
#endif // 0
}
