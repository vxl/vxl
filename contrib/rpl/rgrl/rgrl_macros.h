#ifndef rgrl_macros_h
#define rgrl_macros_h

#include <vcl_typeinfo.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#if defined(VCL_VC_DOTNET) || defined(VCL_GCC)
# define RGRL_HERE __FUNCTION__
#else
# define RGRL_HERE __FILE__
#endif

//: Macro to print the debug message in a class member functions
#define DebugMacro(level, x) \
{ if (this->debug_flag() >= level) { \
    vcl_cout<< "Debug: "<< RGRL_HERE << " ( line "<< \
            __LINE__ <<" )\n" << "       "<<x; } \
} 

//: Macro to print the debug message without preprocessor info
#define DebugMacro_abv(level, x) \
{ if (this->debug_flag() >= level) { vcl_cout<<"       "<<x; } \
} 

//: Macro to print the warning message in a class member functions
#define WarningMacro(x) \
{  if ( this->warning() ) { \
     vcl_cout<< "WARNING: "<< RGRL_HERE <<" ( line "<< \
             __LINE__ <<" )\n" \
             << "       "<<x; }  \
} 

//: Macro to print the warning message without class info
#define WarningMacro_abv(x) \
{ if ( this->warning() ) { \
    vcl_cout<<<"WARNING: "<<x; } \
} 

//: Macro to print the debug message in a stand-alone function
#define DebugFuncMacro(flag, level, x) \
{ if (flag >= level) { \
    vcl_cout<< "Debug: " << RGRL_HERE << " ( line "<< \
            __LINE__ <<" )\n" << "       "<<x; } \
} 

//: Macro to print the debug message in a stand-alone function without
//  preprocessor info
#define DebugFuncMacro_abv(flag, level, x) \
{ if (flag >= level) { vcl_cout<<"       "<<x; } \
} 

//: Macro to define type-related funciotns of the class
//
//  Please note, type_id() is static, and therefore
//  non-virtual. Always do class::type_id(), not
//  class_instance.type_id(). If the type_id of a class instance is
//  needed, use RTTI (run-time-type-identification), i.e.
//  typeid(class_instance).
//
//  is_type(.) checks if the instance is a type of \a classname or \a
//  supperclass.
#define rgrl_type_macro( classname, superclassname) \
     typedef classname       self; \
     typedef superclassname  superclass; \
     static const vcl_type_info& type_id() \
         { return typeid(self); } \
     virtual bool is_type( const vcl_type_info& type ) const\
         { return (typeid(self) == type)!=0 || this->superclass::is_type(type); }

#endif // end of rgrl_macros.h

//  //: Macro to define the type_id() funciotn of the class
//  //
//  //  Please note, type_id() is static, and therefore
//  //  non-virtual. Always do class::type_id(), not
//  //  class_instance.type_id(). If the type name of a class instance is
//  //  needed, use RTTI (run-time-type-identification),
//  //  typeid(class_instance).name().
//  #define rgrl_type_macro( classname ) \
//     static const vcl_type_info& type_id() \
//         {  return typeid(classname); }

//  //: Macro to define the is_type(.) funciotn of the class
//  #define IsTypeMacro( classname ) \
//     virtual bool is_type( const vcl_type_info& type ) \
//         {  return typeid(classname) == type || this->superclass::is_type(type); }




