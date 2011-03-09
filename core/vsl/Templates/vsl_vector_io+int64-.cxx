
#if VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG

//: Write  to vsl_b_ostream
void vsl_b_write(vsl_b_ostream& os,vxl_int_64 n );
//: Read  from vsl_b_istream
void vsl_b_read(vsl_b_istream& is,vxl_int_64& n );
//: Print to a stream
inline void vsl_print_summary(vcl_ostream& os, vxl_int_64 n )
{
#if defined(VCL_VC_6)  // IMS. This Hack could be replaced by code which
  os << "****";        // splits the 64bit int and doesn the right thing.
#else
  os << n;
#endif
}

//: Write  to vsl_b_ostream
void vsl_b_write(vsl_b_ostream& os,vxl_uint_64 n );
//: Read  from vsl_b_istream
void vsl_b_read(vsl_b_istream& is,vxl_uint_64& n );
//: Print to a stream
inline void vsl_print_summary(vcl_ostream& os, vxl_uint_64 n )
{
#if defined(VCL_VC_6)  // IMS. This Hack could be replaced by code which
  os << "****";        // splits the 64bit int and doesn the right thing.
#else
  os << n;
#endif
}

#endif // VXL_HAS_INT_64


#include <vsl/vsl_vector_io.txx>
VSL_VECTOR_IO_INSTANTIATE(long);
