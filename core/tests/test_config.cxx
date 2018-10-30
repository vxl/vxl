// Do not edit this file.  It is created automatically from make_test_config.pl

#include <iostream>
#include <vcl_compiler.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>

// It is quite difficult to do anything out of the norm with the C preprocessor

// quote() has the effect of preprocessor expansion of the argument
// Inner parentheses are needed with quote() to protect empty definitions
// and definitions that have commas
// quote0() protects (") and quotes ("") the expanded argument
#define quote0(x) #x
#define quote(x) quote0((x))

void test_config()
{
  // This test does not test much.  Mostly, it prints configuration
  // macros so they can be viewed through Dart.  Some sanity checks are
  // at the end.

  std::cout << "All macro definitions are shown inside parentheses\n";

   std::cout << "template <> ";
#ifdef template <>
  std::cout << "is set to " << quote(template <>);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_NULL_TMPL_ARGS ";
#ifdef VCL_NULL_TMPL_ARGS
  std::cout << "is set to " << quote(VCL_NULL_TMPL_ARGS);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_DO_NOT_INSTANTIATE(text,ret) ";
#ifdef VCL_DO_NOT_INSTANTIATE
  std::cout << "is set to " << quote(VCL_DO_NOT_INSTANTIATE(text,ret));
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_OVERLOAD_CAST(T,x) ";
#ifdef VCL_OVERLOAD_CAST
  std::cout << "is set to " << quote(VCL_OVERLOAD_CAST(T,x));
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_DEFAULT_TMPL_ARG(arg) ";
#ifdef VCL_DEFAULT_TMPL_ARG
  std::cout << "is set to " << quote(VCL_DEFAULT_TMPL_ARG(arg));
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_LITTLE_ENDIAN ";
#ifdef VXL_LITTLE_ENDIAN
  std::cout << "is set to " << quote(VXL_LITTLE_ENDIAN);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_BIG_ENDIAN ";
#ifdef VXL_BIG_ENDIAN
  std::cout << "is set to " << quote(VXL_BIG_ENDIAN);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_BYTE ";
#ifdef VXL_HAS_BYTE
  std::cout << "is set to " << quote(VXL_HAS_BYTE);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_BYTE_STRING ";
#ifdef VXL_BYTE_STRING
  std::cout << "is set to " << quote(VXL_BYTE_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_INT_8 ";
#ifdef VXL_HAS_INT_8
  std::cout << "is set to " << quote(VXL_HAS_INT_8);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_INT_8_STRING ";
#ifdef VXL_INT_8_STRING
  std::cout << "is set to " << quote(VXL_INT_8_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_INT_16 ";
#ifdef VXL_HAS_INT_16
  std::cout << "is set to " << quote(VXL_HAS_INT_16);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_INT_16_STRING ";
#ifdef VXL_INT_16_STRING
  std::cout << "is set to " << quote(VXL_INT_16_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_INT_32 ";
#ifdef VXL_HAS_INT_32
  std::cout << "is set to " << quote(VXL_HAS_INT_32);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_INT_32_STRING ";
#ifdef VXL_INT_32_STRING
  std::cout << "is set to " << quote(VXL_INT_32_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_INT_64 ";
#ifdef VXL_HAS_INT_64
  std::cout << "is set to " << quote(VXL_HAS_INT_64);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_INT_64_STRING ";
#ifdef VXL_INT_64_STRING
  std::cout << "is set to " << quote(VXL_INT_64_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_INT_64_IS_LONG ";
#ifdef VXL_INT_64_IS_LONG
  std::cout << "is set to " << quote(VXL_INT_64_IS_LONG);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_IEEE_32 ";
#ifdef VXL_HAS_IEEE_32
  std::cout << "is set to " << quote(VXL_HAS_IEEE_32);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_IEEE_32_STRING ";
#ifdef VXL_IEEE_32_STRING
  std::cout << "is set to " << quote(VXL_IEEE_32_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_IEEE_64 ";
#ifdef VXL_HAS_IEEE_64
  std::cout << "is set to " << quote(VXL_HAS_IEEE_64);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_IEEE_64_STRING ";
#ifdef VXL_IEEE_64_STRING
  std::cout << "is set to " << quote(VXL_IEEE_64_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_IEEE_96 ";
#ifdef VXL_HAS_IEEE_96
  std::cout << "is set to " << quote(VXL_HAS_IEEE_96);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_IEEE_96_STRING ";
#ifdef VXL_IEEE_96_STRING
  std::cout << "is set to " << quote(VXL_IEEE_96_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_IEEE_128 ";
#ifdef VXL_HAS_IEEE_128
  std::cout << "is set to " << quote(VXL_HAS_IEEE_128);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_IEEE_128_STRING ";
#ifdef VXL_IEEE_128_STRING
  std::cout << "is set to " << quote(VXL_IEEE_128_STRING);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_PTHREAD_H ";
#ifdef VXL_HAS_PTHREAD_H
  std::cout << "is set to " << quote(VXL_HAS_PTHREAD_H);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_SEMAPHORE_H ";
#ifdef VXL_HAS_SEMAPHORE_H
  std::cout << "is set to " << quote(VXL_HAS_SEMAPHORE_H);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_USECONDS_T ";
#ifdef VXL_UNISTD_HAS_USECONDS_T
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_USECONDS_T);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_INTPTR_T ";
#ifdef VXL_UNISTD_HAS_INTPTR_T
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_INTPTR_T);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_UALARM ";
#ifdef VXL_UNISTD_HAS_UALARM
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_UALARM);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_USLEEP ";
#ifdef VXL_UNISTD_HAS_USLEEP
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_USLEEP);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_LCHOWN ";
#ifdef VXL_UNISTD_HAS_LCHOWN
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_LCHOWN);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_PREAD ";
#ifdef VXL_UNISTD_HAS_PREAD
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_PREAD);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_PWRITE ";
#ifdef VXL_UNISTD_HAS_PWRITE
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_PWRITE);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_TELL ";
#ifdef VXL_UNISTD_HAS_TELL
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_TELL);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_GETPID ";
#ifdef VXL_UNISTD_HAS_GETPID
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_GETPID);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_HAS_GETHOSTNAME ";
#ifdef VXL_UNISTD_HAS_GETHOSTNAME
  std::cout << "is set to " << quote(VXL_UNISTD_HAS_GETHOSTNAME);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_UNISTD_USLEEP_IS_VOID ";
#ifdef VXL_UNISTD_USLEEP_IS_VOID
  std::cout << "is set to " << quote(VXL_UNISTD_USLEEP_IS_VOID);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_HAS_IEEEFP_H ";
#ifdef VXL_HAS_IEEEFP_H
  std::cout << "is set to " << quote(VXL_HAS_IEEEFP_H);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_GCC ";
#ifdef VCL_GCC
  std::cout << "is set to " << quote(VCL_GCC);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_WIN32 ";
#ifdef VCL_WIN32
  std::cout << "is set to " << quote(VCL_WIN32);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VCL_VC ";
#ifdef VCL_VC
  std::cout << "is set to " << quote(VCL_VC);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_WARN_DEPRECATED ";
#ifdef VXL_WARN_DEPRECATED
  std::cout << "is set to " << quote(VXL_WARN_DEPRECATED);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_WARN_DEPRECATED_ONCE ";
#ifdef VXL_WARN_DEPRECATED_ONCE
  std::cout << "is set to " << quote(VXL_WARN_DEPRECATED_ONCE);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "VXL_WARN_DEPRECATED_ABORT ";
#ifdef VXL_WARN_DEPRECATED_ABORT
  std::cout << "is set to " << quote(VXL_WARN_DEPRECATED_ABORT);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "CMAKE_DEFINES_VARIABLE_UNIX ";
#ifdef CMAKE_DEFINES_VARIABLE_UNIX
  std::cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_UNIX);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "CMAKE_DEFINES_VARIABLE_CYGWIN ";
#ifdef CMAKE_DEFINES_VARIABLE_CYGWIN
  std::cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_CYGWIN);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "CMAKE_DEFINES_VARIABLE_MINGW ";
#ifdef CMAKE_DEFINES_VARIABLE_MINGW
  std::cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_MINGW);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "CMAKE_DEFINES_VARIABLE_WIN32 ";
#ifdef CMAKE_DEFINES_VARIABLE_WIN32
  std::cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_WIN32);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "CMAKE_DEFINES_VARIABLE_APPLE ";
#ifdef CMAKE_DEFINES_VARIABLE_APPLE
  std::cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_APPLE);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__cplusplus ";
#ifdef __cplusplus
  std::cout << "is set to " << quote(__cplusplus);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__STDC__ ";
#ifdef __STDC__
  std::cout << "is set to " << quote(__STDC__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__STDC_VERSION__ ";
#ifdef __STDC_VERSION__
  std::cout << "is set to " << quote(__STDC_VERSION__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "NDEBUG ";
#ifdef NDEBUG
  std::cout << "is set to " << quote(NDEBUG);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__STRICT_ANSI__ ";
#ifdef __STRICT_ANSI__
  std::cout << "is set to " << quote(__STRICT_ANSI__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__GNUC__ ";
#ifdef __GNUC__
  std::cout << "is set to " << quote(__GNUC__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__GNUG__ ";
#ifdef __GNUG__
  std::cout << "is set to " << quote(__GNUG__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__GNUC_MINOR__ ";
#ifdef __GNUC_MINOR__
  std::cout << "is set to " << quote(__GNUC_MINOR__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__GNUC_PATCHLEVEL__ ";
#ifdef __GNUC_PATCHLEVEL__
  std::cout << "is set to " << quote(__GNUC_PATCHLEVEL__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__VERSION__ ";
#ifdef __VERSION__
  std::cout << "is set to " << quote(__VERSION__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__OPTIMIZE__ ";
#ifdef __OPTIMIZE__
  std::cout << "is set to " << quote(__OPTIMIZE__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__ICC ";
#ifdef __ICC
  std::cout << "is set to " << quote(__ICC);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__INTEL_COMPILER ";
#ifdef __INTEL_COMPILER
  std::cout << "is set to " << quote(__INTEL_COMPILER);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__INTEL_CXXLIB_ICC ";
#ifdef __INTEL_CXXLIB_ICC
  std::cout << "is set to " << quote(__INTEL_CXXLIB_ICC);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "_MSC_VER ";
#ifdef _MSC_VER
  std::cout << "is set to " << quote(_MSC_VER);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "_COMPILER_VERSION ";
#ifdef _COMPILER_VERSION
  std::cout << "is set to " << quote(_COMPILER_VERSION);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__CYGWIN__ ";
#ifdef __CYGWIN__
  std::cout << "is set to " << quote(__CYGWIN__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__MINGW32__ ";
#ifdef __MINGW32__
  std::cout << "is set to " << quote(__MINGW32__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "_WIN32 ";
#ifdef _WIN32
  std::cout << "is set to " << quote(_WIN32);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__APPLE__ ";
#ifdef __APPLE__
  std::cout << "is set to " << quote(__APPLE__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "macintosh ";
#ifdef macintosh
  std::cout << "is set to " << quote(macintosh);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "unix ";
#ifdef unix
  std::cout << "is set to " << quote(unix);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__unix ";
#ifdef __unix
  std::cout << "is set to " << quote(__unix);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__unix__ ";
#ifdef __unix__
  std::cout << "is set to " << quote(__unix__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__linux ";
#ifdef __linux
  std::cout << "is set to " << quote(__linux);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__linux__ ";
#ifdef __linux__
  std::cout << "is set to " << quote(__linux__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__DATE__ ";
#ifdef __DATE__
  std::cout << "is set to " << quote(__DATE__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << "__TIME__ ";
#ifdef __TIME__
  std::cout << "is set to " << quote(__TIME__);
#else
  std::cout << "is not set";
#endif
  std::cout << std::endl;

  std::cout << std::endl;

  // These are types that we expect every machine to have.

  testlib_test_begin( "Size of vxl_byte" );
#if VXL_HAS_BYTE
  testlib_test_perform( sizeof( vxl_byte ) == 1 );
#else
  std::cout << "(no vxl_byte)" << std::endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_8" );
#if VXL_HAS_INT_8
  testlib_test_perform( sizeof( vxl_uint_8 ) == 1 );
#else
  std::cout << "(no vxl_uint_8)" << std::endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_16" );
#if VXL_HAS_INT_16
  testlib_test_perform( sizeof( vxl_uint_16 ) == 2 );
#else
  std::cout << "(no vxl_uint_16)" << std::endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_32" );
#if VXL_HAS_INT_32
  testlib_test_perform( sizeof( vxl_uint_32 ) == 4 );
#else
  std::cout << "(no vxl_uint_32)" << std::endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_ieee_32" );
#if VXL_HAS_IEEE_32
  testlib_test_perform( sizeof( vxl_ieee_32 ) == 4 );
#else
  std::cout << "(no vxl_ieee_32)" << std::endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_ieee_64" );
#if VXL_HAS_IEEE_64
  testlib_test_perform( sizeof( vxl_ieee_64 ) == 8 );
#else
  std::cout << "(no vxl_ieee_64)" << std::endl;
  testlib_test_perform( false );
#endif

  std::cout << std::endl;

  // These may not exist on some platforms.  If they exist, they should
  // be of the correct size.

#if VXL_HAS_INT_64
  testlib_test_begin( "Size of vxl_uint_64" );
  testlib_test_perform( sizeof( vxl_uint_64 ) == 8 );
#else
  std::cout << "(no vxl_uint_64)" << std::endl;
#endif

#if VXL_HAS_IEEE_96
  testlib_test_begin( "Size of vxl_ieee_96" );
  testlib_test_perform( sizeof( vxl_ieee_96 ) == 12 );
#else
  std::cout << "(no vxl_ieee_96)" << std::endl;
#endif

#if VXL_HAS_IEEE_128
  testlib_test_begin( "Size of vxl_ieee_128" );
  testlib_test_perform( sizeof( vxl_ieee_128 ) == 16 );
#else
  std::cout << "(no vxl_ieee_128)" << std::endl;
#endif

  std::cout << std::endl;

  std::cout << "sizeof(char) is " << sizeof(char) << std::endl;
  std::cout << "sizeof(short) is " << sizeof(short) << std::endl;
  std::cout << "sizeof(int) is " << sizeof(int) << std::endl;
  std::cout << "sizeof(long) is " << sizeof(long) << std::endl;
  std::cout << "sizeof(float) is " << sizeof(float) << std::endl;
  std::cout << "sizeof(double) is " << sizeof(double) << std::endl;
}

TESTMAIN(test_config);
