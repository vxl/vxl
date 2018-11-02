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

#include <iostream>
#include <string>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_null_process.h>
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_tuple.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define REG_PROCESS(proc, T) \
  T ::register_process( bprb_process_sptr( new proc() ) )

#define REG_PROCESS_FUNC(proc, T, func, nm) \
  T ::register_process( bprb_process_sptr( new proc(func, nm) ) )

#define REG_PROCESS_FUNC_INIT_FINALIZE(proc, T, func, nm, init, fin) \
  T ::register_process( bprb_process_sptr( new proc(func, nm, 0, init, fin) ) )

#define REG_PROCESS_FUNC_CONS(proc, T, func, nm) \
 T ::register_process( bprb_process_sptr( new proc(func, nm, func##_cons, 0, 0) ) )


#define REG_PROCESS_FUNC_CONS2(func) \
bprb_batch_process_manager::register_process( bprb_process_sptr( new bprb_func_process(func, #func, func##_cons, 0, 0) ) )

#define REG_PROCESS_FUNC_CONS_INIT(proc, T, func, nm) \
 T ::register_process( bprb_process_sptr( new proc(func, nm, func##_cons, func##_init, 0) ) )
#define REG_PROCESS_FUNC_CONS_FIN(proc, T, func, nm) \
 T ::register_process( bprb_process_sptr( new proc(func, nm, func##_cons, 0, func##_finish) ) )
#define REG_PROCESS_FUNC_CONS_INIT_FIN(proc, T, func, nm) \
 T ::register_process( bprb_process_sptr( new proc(func, nm, func##_cons, func##_init, func##_finish) ) )

#define REGISTER_DATATYPE_LONG_FORM(T, FUNCSUFFIX) \
  std::string s##FUNCSUFFIX = #FUNCSUFFIX; \
  s##FUNCSUFFIX += "_data"; \
  if(!DATABASE->exists(s##FUNCSUFFIX)){ \
  std::vector<std::string> r_##FUNCSUFFIX##_names(2); \
  std::vector<std::string> r_##FUNCSUFFIX##_types(2); \
  r_##FUNCSUFFIX##_names[0]="id"; \
  r_##FUNCSUFFIX##_names[1]="value"; \
  r_##FUNCSUFFIX##_types[0]=brdb_value_t<unsigned>::type(); \
  r_##FUNCSUFFIX##_types[1]=brdb_value_t<T >::type(); \
  brdb_relation_sptr r_##FUNCSUFFIX  = new brdb_relation(r_##FUNCSUFFIX##_names,r_##FUNCSUFFIX##_types); \
  DATABASE->add_relation(s##FUNCSUFFIX, r_##FUNCSUFFIX); \
  }

#define REGISTER_DATATYPE(T) \
   REGISTER_DATATYPE_LONG_FORM(T,T)

#define DECLARE_FUNC(func) \
bool func(bprb_func_process& pro)

#define DECLARE_FUNC_CONS(func) \
bool func(bprb_func_process& pro); \
bool func##_cons(bprb_func_process& pro)

#define DECLARE_FUNC_CONS_INIT(func) \
bool func(bprb_func_process& pro); \
bool func##_cons(bprb_func_process& pro); \
bool func##_init(bprb_func_process& pro)

#define DECLARE_FUNC_CONS_FIN(func) \
bool func(bprb_func_process& pro); \
bool func##_cons(bprb_func_process& pro); \
bool func##_finish(bprb_func_process& pro)

#define DECLARE_FUNC_CONS_INIT_FIN(func) \
bool func(bprb_func_process& pro); \
bool func##_cons(bprb_func_process& pro); \
bool func##_init(bprb_func_process& pro); \
bool func##_finish(bprb_func_process& pro)

#endif // bprb_macros_h_
