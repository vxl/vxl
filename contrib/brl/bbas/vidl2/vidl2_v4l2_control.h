// This is brl/bbas/vidl2/vidl2_v4l2_control.h
#ifndef vidl2_v4l2_control_h_
#define vidl2_v4l2_control_h_
//:
// \file
// \brief A base control class for user controls
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     10 Nov 2008 Created (A. Garrido)
//\endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
//#include "vidl2_v4l2_control_sptr.h"

extern "C" {
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>
};

//: A base class for handle a control.
class vidl2_v4l2_control
{
  protected:
    int fd;
    struct v4l2_queryctrl ctrl_;
    vidl2_v4l2_control(const v4l2_queryctrl& ctr, int f): ctrl_(ctr), fd(f) {}
    void set_value(int v) const;
    int get_value() const;
  public:
    virtual ~vidl2_v4l2_control() {}

    //: Factory method to create new controls
    // \param ctr details from VIDIOC_QUERYCTRL
    // \param f associated file descriptor
    static vidl2_v4l2_control * new_control(const v4l2_queryctrl& ctr, int f);

    v4l2_ctrl_type type() const { return ctrl_.type; }

    vcl_string name() const { return  (const char *) ctrl_.name; }

    //: Print a 1-line brief description
    virtual vcl_string description() const= 0;

    int id() const { return ctrl_.id; }

    //: Control is read only
    bool read_only() const { return ctrl_.flags & V4L2_CTRL_FLAG_READ_ONLY; }
    //: Control can change value of other controls
    bool affect_other_controls() const { return ctrl_.flags & V4L2_CTRL_FLAG_UPDATE; }


    //-------------------------------------------------------
    // reference counting if used through sptr
//     public:
// 
//       //: Increment reference count
//       void ref() { ref_count_++; }
// 
//       //: Decrement reference count
//       void unref(){
//         assert (ref_count_ >0);
//         ref_count_--;
//         if (ref_count_==0)
//         {
//           delete this;
//         }
//       }
// 
//       //: Number of objects referring to this data
//       int ref_count() const { return ref_count_; }
// 
//     private:
//       int ref_count_;
};

class vidl2_v4l2_control_integer: public vidl2_v4l2_control
{
  public:
    vidl2_v4l2_control_integer(const v4l2_queryctrl& ctr, int f): vidl2_v4l2_control(ctr,f) {}
    int minimum() const { return ctrl_.minimum; }
    int maximum() const { return ctrl_.maximum; }
    int step() const { return ctrl_.step; }
    int default_value() const { return ctrl_.default_value; }
    //: Set the value of the control 
    // \param value to be set in range determined by driver
    void set(int value) const;
    //: Change control 
    // \param value to be set in range 0-100
    void set_100 ( int value) const;
    //: Get the value of the control 
    // \return value in range determined by driver
    int get() const { return get_value(); }
    //: Get the value of the control 
    // \return value in range 0-100
    int get_100() const { return (get_value()-ctrl_.minimum)*100/(ctrl_.maximum-ctrl_.minimum); }
    virtual vcl_string description() const;

};

class vidl2_v4l2_control_menu: public vidl2_v4l2_control
{
    vcl_vector<vcl_string> items;
  public:
    vidl2_v4l2_control_menu(const v4l2_queryctrl& ctr, int f);
    //int minimum() const { return 0; }
    //int maximum() const { return ctrl_.maximum; }
    //int step() const { return 1; }
    int n_items() const { return items.size();}
    vcl_string get(unsigned int i) const { return items[i]; }
    void set(unsigned int i) const { if (i>=0 && i<n_items()) set_value((int) i); }
    //: Get the value of the control 
    // \return index in the menu
    unsigned int get() const { return (unsigned int) get_value(); }
    int default_value() const { return ctrl_.default_value; }
    virtual vcl_string description() const;
};

class vidl2_v4l2_control_boolean: public vidl2_v4l2_control
{
  public:
    vidl2_v4l2_control_boolean(const v4l2_queryctrl& ctr, int f): vidl2_v4l2_control(ctr,f) {}
    bool default_value() const { return ctrl_.default_value; }
    void set(bool v) const { set_value(v?1:0); }
    bool get() const { return get_value(); }
    virtual vcl_string description() const 
       { return "Control \""+name()+"\": boolean (default: "+(default_value()?"true":"false")+")"; }
};

class vidl2_v4l2_control_button: public vidl2_v4l2_control
{
  public:
    vidl2_v4l2_control_button(const v4l2_queryctrl& ctr, int f): vidl2_v4l2_control(ctr,f) {}
    void push() const { set_value(1); }
    virtual vcl_string description() const { return "Control \""+name()+"\": button"; }
};



#endif // vidl2_v4l2_control_h_
