// This is sdet_appearance.h
#ifndef sdet_appearance_h
#define sdet_appearance_h
//:
//\file
//\brief The base class for storing edgel appearance info
//\author Amir Tamrakar
//\date 09/05/07
//
//\endverbatim

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include "sdet_EMD.h"

//: Base class for image appearance on edgels
class sdet_appearance
{
public:
  
  //: default constructor
  sdet_appearance(){}

  //: copy constructor
  sdet_appearance(const sdet_appearance& /*other*/) {}

  //: destructor
  virtual ~sdet_appearance(){}

  //: compute the distance between two appearance values
  virtual double dist(const sdet_appearance& /*other*/)=0;

  virtual double value()=0;

  //: 
  virtual vcl_string print_info() const=0;

  //: Clone `this': creation of a new object and initialization. This is used so
  // that client classes like sdet_edgel supports any type of sdet_appearance.
  // See Prototype design pattern in Gamma et. al.
  virtual sdet_appearance *clone() const = 0;
};


//: A derived class to store intensity as appearance
class sdet_intensity: public sdet_appearance
{
public:
  double val;      ///< the value of the intensity

  //: default constructor
  sdet_intensity() : val(0.0){}
  
  //: constructor
  sdet_intensity(double value) : val(value){}
  
  //: copy constructor
  sdet_intensity(const sdet_intensity & other) : sdet_appearance(other) {val = other.val; }

  //: destructor
  ~sdet_intensity(){}

  virtual double value(){ return val; }

  //: compute the distance between two appearance values (assumes that the operator '-' is defined for class A)
  virtual double dist(const sdet_appearance& other)
  {
    return val - ((sdet_intensity*)&other)->val;
  }

  //: return a string with the info of this appearance measure
  virtual vcl_string print_info() const { 
    vcl_stringstream ss;
    ss << val << '\0';
    return ss.str();
  }

  //: \see sdet_appearance::clone
  virtual sdet_appearance *clone() const {
    return new sdet_intensity(*this);
  }
};

//: A derived class to store color as appearance
class sdet_color: public sdet_appearance
{
public:
  double c1, c2, c3;      ///< the value of the components

  //: default constructor
  sdet_color() : c1(0.0), c2(0.0), c3(0.0){}
  
  //: constructor
  sdet_color(double v1, double v2, double v3) : c1(v1), c2(v2), c3(v3){}
  
  //: copy constructor
  sdet_color(const sdet_color & other) : sdet_appearance(other) {c1 = other.c1; c2 = other.c2; c3 = other.c3;}

  //: destructor
  ~sdet_color(){}

  virtual double value(){ return c1; } //not meaningful

  //: compute the distance between two appearance values (assumes that the operator '-' is defined for class A)
  virtual double dist(const sdet_appearance& other)
  {
    //euclidean distance
    sdet_color* o = (sdet_color*)&other;

    return vcl_sqrt((c1-o->c1)*(c1-o->c1) + (c2-o->c2)*(c2-o->c2) + (c3-o->c3)*(c3-o->c3));
  }

  //: return a string with the info of this appearance measure
  virtual vcl_string print_info() const{ 
    vcl_stringstream ss;
    ss << "(" << c1 << ", " << c2 << ", " << c3 << ")" << '\0';
    return ss.str();
  }

  //: \see sdet_appearance::clone
  virtual sdet_appearance *clone() const {
    return new sdet_color(*this);
  }
};


//: A derived class to store gray sdet_signature as appearance
class sdet_gray_signature: public sdet_appearance
{
public:
  sdet_signature sig;      ///< the stored signature

  //: default constructor
  sdet_gray_signature(){}
  
  //: constructor
  sdet_gray_signature(sdet_signature newsig) : sig(newsig){}
  
  //: copy constructor
  sdet_gray_signature(const sdet_gray_signature & other):sdet_appearance(other) {sig = other.sig; }

  //: destructor
  ~sdet_gray_signature(){}

  virtual double value(){ return 0.0; }

  //: compute the distance between two appearance values (assumes that the operator '-' is defined for class A)
  virtual double dist(const sdet_appearance& other)
  {
    return vcl_fabs(sig - ((sdet_gray_signature*)&other)->sig);
  }

  //: return a string with the info of this appearance measure
  virtual vcl_string print_info() const
  { 
    vcl_stringstream ss;
    ss << "[" ;
    for (int i=0; i<NBINS; i++)
      ss << sig.bins[i].weight << " ,";
      //ss << "(" << sig.bins[i].value << ", " << sig.bins[i].weight << ") ,";
    ss << "]";
    return ss.str();  
  }

  //: \see sdet_appearance::clone
  virtual sdet_appearance *clone() const {
    return new sdet_gray_signature(*this);
  }
};

//: A derived class to store gray sdet_signature as appearance
class sdet_color_signature: public sdet_appearance
{
public:
  sdet_color_sig sig;      ///< the stored signature

  //: default constructor
  sdet_color_signature(){}
  
  //: constructor
  sdet_color_signature(sdet_color_sig newsig) : sig(newsig){}
  
  //: copy constructor
  sdet_color_signature(const sdet_color_signature & other) : sdet_appearance(other) {sig = other.sig; }

  //: destructor
  ~sdet_color_signature(){}

  virtual double value(){ return 0.0; }

  //: compute the distance between two appearance values (assumes that the operator '-' is defined for class A)
  virtual double dist(const sdet_appearance& other)
  {
    return vcl_fabs(sig - ((sdet_color_signature*)&other)->sig);
  }

  //: return a string with the info of this appearance measure
  virtual vcl_string print_info() const
  { 
    vcl_stringstream ss;
    ss << "[" ;
    //for (int i=0; i<NBINS; i++)
    //  ss << sig.bins[i].weight << " ,";
      //ss << "(" << sig.bins[i].value << ", " << sig.bins[i].weight << ") ,";
    //ss << "]";
    return ss.str();  
  }

  //: \see sdet_appearance::clone
  virtual sdet_appearance *clone() const {
    return new sdet_color_signature(*this);
  }
};

//: independent global function to compute distance between two appearances
inline double sdet_appearance_dist(sdet_appearance* app1, sdet_appearance* app2)
{
  return app1->dist(*app2);
}

#endif // sdet_appearance_h
