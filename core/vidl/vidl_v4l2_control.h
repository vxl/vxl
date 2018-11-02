// This is core/vidl/vidl_v4l2_control.h
#ifndef vidl_v4l2_control_h_
#define vidl_v4l2_control_h_
//:
// \file
// \brief A base control class for user controls
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     10 Nov 2008 Created (A. Garrido)
//\endverbatim

#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

extern "C" {
//#include <asm/types.h>          /* for videodev2.h */
#include <sys/time.h>
#include <linux/videodev2.h>
};

//: A base class for handle a control.
class vidl_v4l2_control
{
  protected:
    int fd;
    struct v4l2_queryctrl ctrl_;
    vidl_v4l2_control(const v4l2_queryctrl& ctr, int f):  fd(f), ctrl_(ctr) {}
    void set_value(int v) const;
    int get_value() const;
  public:
    virtual ~vidl_v4l2_control() {}
    //: Factory method to create new controls
    // \param ctr details from VIDIOC_QUERYCTRL
    // \param f associated file descriptor
    // \return pointer to base class of new control
    static vidl_v4l2_control * new_control(const v4l2_queryctrl& ctr, int f);
    //: Type of control
    // \return type as indicated in v4l2 specification
    v4l2_ctrl_type type() const { return (v4l2_ctrl_type)ctrl_.type; }
    //: Name of control
    // \return name the driver assign to the control
    std::string name() const { return  (const char *) ctrl_.name; }
    //: A 1-line brief description
    virtual std::string description() const= 0;
    //: Id of control
    // \return ID (for example, V4L2_CID_BRIGHTNESS corresponds to brightness)
    int id() const { return ctrl_.id; }
    //: Control is read only
    bool read_only() const
    {
#ifdef V4L2_CTRL_FLAG_READ_ONLY
      return ctrl_.flags & V4L2_CTRL_FLAG_READ_ONLY;
#else
      return false;
#endif
    }
    //: Control can change value of other controls
    bool affect_other_controls() const
    {
#ifdef V4L2_CTRL_FLAG_UPDATE
      return ctrl_.flags & V4L2_CTRL_FLAG_UPDATE;
#else
      return false;
#endif
     }
    //: Reset control
    // \note no-op fot button controls
    virtual void reset() const {}
};

//: A class for handle a control of type integer
class vidl_v4l2_control_integer: public vidl_v4l2_control
{
  public:
    vidl_v4l2_control_integer(const v4l2_queryctrl& ctr, int f): vidl_v4l2_control(ctr,f) {}
    //: Minimum value of the control
    int minimum() const { return ctrl_.minimum; }
    //: Maximum value of the control
    int maximum() const { return ctrl_.maximum; }
    //: Step size
    // Indicates the increment between values which are actually different on the hardware
    int step() const { return ctrl_.step; }
    //: Default value of this control
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
    //: A 1-line brief description
    virtual std::string description() const;
    //: Reset control
    virtual void reset() const { set(default_value()); }
};

//: A class for handle a control of type menu
class vidl_v4l2_control_menu: public vidl_v4l2_control
{
    std::vector<std::string> items;
  public:
    vidl_v4l2_control_menu(const v4l2_queryctrl& ctr, int f);
    //: Number of items
    // \return number of entries in the menu
    unsigned int n_items() const { return items.size();}
    //: Item i
    // \return string assigned to item i
    std::string get(unsigned int i) const { return items[i]; }
    //: Select item i
    void set(unsigned int i) const { if (i<n_items()) set_value((int) i); }
    //: Get the value of the control
    // \return index in the menu
    unsigned int get() const { return (unsigned int) get_value(); }
    //: Default value of this control
    unsigned int default_value() const { return ctrl_.default_value; }
    //: A 1-line brief description
    virtual std::string description() const;
    //: Reset control
    virtual void reset() const { set(default_value()); }
};

//: A class for handle a control of type boolean
class vidl_v4l2_control_boolean: public vidl_v4l2_control
{
  public:
    vidl_v4l2_control_boolean(const v4l2_queryctrl& ctr, int f): vidl_v4l2_control(ctr,f) {}
    //: Set the value of the control
    void set(bool v) const { set_value(v?1:0); }
    //: Get the value of the control
    bool get() const { return get_value(); }
    //: Default value of this control
    bool default_value() const { return ctrl_.default_value; }
    //: A 1-line brief description
    virtual std::string description() const
       { return "Control \""+name()+"\": boolean (default: "+(default_value()?"true":"false")+")"; }
    //: Reset control
    virtual void reset() const { set(default_value()); }
};

//: A class for handle a control of type button
class vidl_v4l2_control_button: public vidl_v4l2_control
{
  public:
    vidl_v4l2_control_button(const v4l2_queryctrl& ctr, int f): vidl_v4l2_control(ctr,f) {}
    //: Push button
    void push() const { set_value(1); }
    //: A 1-line brief description
    virtual std::string description() const { return "Control \""+name()+"\": button"; }
};

#endif // vidl_v4l2_control_h_
