// This is core/vul/vul_debug.h

//:
// \file
// \author Ian Scott


//: Dump a core file.
// Not yet implemented on non-windows platforms.
void vul_debug_core_dump(const char * filename);


#ifdef _WIN32

//: Force a core dump whilst inside a Structured Exception in a windows program.
// To get a core dump from a Windows structured exceptions
// \verbatim
// void main()
// {
//   __try
//   {
//     // Rest of program
//   }
//   __except(vul_debug_core_dump2(filename, GetExceptionInformation()))
//   {}
// }
// \endverbatim
void vul_debug_core_dump_in_windows_seh(const char * filename,
                                        void* pep);
#endif

