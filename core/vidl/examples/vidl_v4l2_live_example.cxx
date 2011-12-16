// This is core/vidl/examples/vidl_v4l2_live_example.cxx

#include <vidl/vidl_v4l2_devices.h>
#include <vidl/vidl_v4l2_istream.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vul/vul_timer.h>
#include <vcl_iostream.h>

int select_device(vidl_v4l2_devices& devs)
{
  vcl_cout << "Looking for devices...\n"
           << "Numbers of found devices: " << devs.size() << vcl_endl;

  for (unsigned int i=0; i< devs.size(); ++i) {
    vcl_cout << "DEVICE  " << i << ":\n" <<  devs(i);
  }

  int dnum=-1; // device selected

  if (devs.size()==0)
    vcl_cout << "No devices." << vcl_endl;
  else if (devs.size()==1) {
    dnum=0;
  }
  else {
    do {
      vcl_cout << "Select Device Number: ";
      vcl_cin >> dnum;
    } while (dnum<0 || dnum>=(int) devs.size());
  }
  return dnum;
}

void change_width_height(vidl_v4l2_device& dev)
{
  int nw,nh;
  vcl_cout << "Enter new width: ";
  vcl_cin >> nw;
  vcl_cout << "Enter new height: ";
  vcl_cin >> nh;
  // same format but try to change width/height
  if (!dev.set_v4l2_format(dev.get_v4l2_format(),nw,nh)) {
    vcl_cout << "New format is not set" << vcl_endl;
  }
  else vcl_cout << "New format set" << vcl_endl; // w,h could be changed by driver

  vcl_cout << "Using device:" << dev;
}

// return if success
bool configure_input(vidl_v4l2_device& dev)
{
  int inum=-1;
  if (dev.n_inputs()==1)
    inum= 0;
  else
    do {
      vcl_cout << "Select Input Number: ";
      vcl_cin >> inum;
    } while (inum<0 || inum>=(int)dev.n_inputs());
  if (!dev.set_input(inum))  {
    vcl_cerr << "Input " << inum << " not set\n";
    return false;
  }
  else {
    vcl_cout << "Input: " << dev.input(inum).name() << " selected." << vcl_endl;
    dev.try_formats();
    if (! dev.format_is_set()) {
      vcl_cout << "A default format has not been set." << vcl_endl;
      if (dev.set_v4l2_format(V4L2_PIX_FMT_YUV420,640,480)) { // for example
        vcl_cout << "  V4L2_PIX_FMT_YUV420 format is possible." << vcl_endl;
      }
      return false;
    }
    if (!dev) {
      vcl_cerr <<"Error in device: " <<dev.get_error() << vcl_endl;
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
    vcl_cout << "Grabbing " << i << " frames in directory ./dump" << vcl_endl;
    while ( i-- && test.advance())
    {
      if (dev.get_control_id(V4L2_CID_BRIGHTNESS))
        dev.get_control_integer_id(V4L2_CID_BRIGHTNESS)->set_100(i);
      vcl_cout << "Grabbing: " <<i<<"(frame number: " << test.frame_number() << ")\n"
               << "  Device frame number: " << dev.sequence() << vcl_endl;
      if (!test_out.write_frame(test.current_frame()))
        vcl_cerr << "Couldn't write frame\n";
    }
    timer.print(vcl_cout);
  }
  else {
    vcl_cerr << "Couldn't use stream\n";
    if (!dev)
      vcl_cerr <<"Error in device: " <<dev.get_error() << vcl_endl;
  }
}


int main()
{
  vidl_v4l2_devices& devs= vidl_v4l2_devices::all();  // simpler name

  int dnum=select_device(devs);;

  if (dnum>=0) {
    if (configure_input(devs(dnum))) {
      for (int i=0;i<devs(dnum).n_controls(); ++i)
        vcl_cout << i << "->" << devs(dnum).get_control(i)->description() << vcl_endl;
      save_frames(devs(dnum));
    }
  }
#if 0
  else {
    vcl_string name;
    vcl_cout << "Select device name: ";
    vcl_cin >> name;
    vidl_v4l2_device dev(name.c_str());
    if (dev)
      save_frames(dev);
  }
#endif // 0
}
