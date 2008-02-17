// This is brl/bpro/bprb/bprb_macros.h
#ifndef bprb_macros_h_
#define bprb_macros_h_
//:
// \file
// \brief Macros for process management
// \author J.L. Mundy
// \date February 4, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_null_process.h>
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_tuple.h>
#include <vcl_string.h>

#define REG_PROCESS(proc, T) \
  T ::register_process( bprb_process_sptr( new proc() ) )

#define REGISTER_DATATYPE(T) \
  vcl_vector<vcl_string> r_##T##_names(2); \
  vcl_vector<vcl_string> r_##T##_types(2); \
  r_##T##_names[0]="id"; \
  r_##T##_names[1]="value"; \
  r_##T##_types[0]=brdb_value_t<unsigned>::type(); \
  r_##T##_types[1]=brdb_value_t<T>::type(); \
  brdb_relation_sptr r_##T  = new brdb_relation(r_##T##_names,r_##T##_types); \
  vcl_string s##T = #T; \
  s##T += "_data"; \
  DATABASE->add_relation(s##T, r_##T)

#endif // bprb_macros_h_
