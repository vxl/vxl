// This is mul/vil2/io/vil2_io_memory_chunk.h
#ifndef vil2_io_memory_chunk_h_
#define vil2_io_memory_chunk_h_
//:
// \file
// \author Tim Cootes

#include <vil2/vil2_memory_chunk.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil2_memory_chunk to stream.
void vsl_b_write(vsl_b_ostream &os, const vil2_memory_chunk& chunk);

//: Binary load vil2_memory_chunk from stream.
void vsl_b_read(vsl_b_istream &is, vil2_memory_chunk& chunk);

//: Binary load vil2_memory_chunk from stream  onto the heap 
void vsl_b_read(vsl_b_istream &is, vil2_memory_chunk*& chunk);

//: Print human readable summary of a vil2_memory_chunk object to a stream
void vsl_print_summary(vcl_ostream& os,const vil2_memory_chunk& chunk);

#endif
