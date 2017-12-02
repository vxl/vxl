// This is core/vil/io/vil_io_memory_chunk.h
#ifndef vil_io_memory_chunk_h_
#define vil_io_memory_chunk_h_
//:
// \file
// \author Tim Cootes

#include <vil/vil_memory_chunk.h>
#include <vsl/vsl_binary_io.h>
#include <vil/io/vil_io_smart_ptr.h> // for vsl_b_read(is, vil_memory_chunk_sptr)

//: Binary save vil_memory_chunk to stream.
// \relatesalso vil_memory_chunk
void vsl_b_write(vsl_b_ostream &os, const vil_memory_chunk& chunk);

//: Binary load vil_memory_chunk from stream.
// \relatesalso vil_memory_chunk
void vsl_b_read(vsl_b_istream &is, vil_memory_chunk& chunk);

//: Binary save vil_memory_chunk to stream  by pointer
// \relatesalso vil_memory_chunk
void vsl_b_write(vsl_b_ostream &os, const vil_memory_chunk* chunk);

//: Binary load vil_memory_chunk from stream  onto the heap
// \relatesalso vil_memory_chunk
void vsl_b_read(vsl_b_istream &is, vil_memory_chunk*& chunk);

//: Print human readable summary of a vil_memory_chunk object to a stream
// \relatesalso vil_memory_chunk
void vsl_print_summary(std::ostream& os,const vil_memory_chunk& chunk);

//: Print human readable summary of a vil_memory_chunk object to a stream
// \relatesalso vil_memory_chunk
inline void vsl_print_summary(std::ostream& os,const vil_memory_chunk* chunk)
{ vsl_print_summary(os, *chunk); }

#endif
