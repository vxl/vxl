// This is brl/bpro/bprb/bprb_batch_process_manager.cxx
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "bprb_batch_process_manager.h"
//:
// \file
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <bprb/bprb_process.h>
#include <bprb/bprb_null_process.h>
#include <bprb/bprb_parameters.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bprb_batch_process_manager::bprb_batch_process_manager() : current_process_(nullptr),
                                                           verbose_(true)
{
}

//: Destructor
bprb_batch_process_manager::~bprb_batch_process_manager()
= default;

bool bprb_batch_process_manager::clear()
{
  return true;
}

bool bprb_batch_process_manager::init_process(std::string const& process_name)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if (p) {
    if (verbose_)
      std::cout << "Process: " << p->name() << std::endl;
    current_process_ = p;
    return true;
  }
  std::cout << "ERROR!!!! Process: " << process_name << " is not FOUND\n";
  return false;
}

//: print the default values of the process into the specified XML file
bool bprb_batch_process_manager::print_default_params(std::string const & process_name, std::string const& params_XML)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if (p) {
    std::cout << "Printing default params for process: " << p->name() << std::endl;
    (p->parameters())->print_def_XML(p->name(), params_XML);
    return true;
  }
  return false;
}

//: read and set the parameters from an XML file for the current process
bool bprb_batch_process_manager::set_params(std::string const& params_XML)
{
  if (!current_process_) {
    std::cout << "In bprb_batch_process_manager::set_params(.) - null process\n";
    return false;
  }
  if (!current_process_->parse_params_XML(params_XML)) {
    std::cout << "In bprb_batch_process_manager::set_params(.) - not able to parse the XML file: "
             << params_XML << std::endl;
    return false;
  }
  return true;
}

//: set the parameters from another parameter instance for the current process
bool bprb_batch_process_manager::set_params(const bprb_parameters_sptr& params)
{
  if (!current_process_) {
    std::cout << "In bprb_batch_process_manager::set_params(.) - null process\n";
    return false;
  }
  current_process_->set_parameters(params);
  return true;
}

//: set primitive data type input on current process
bool bprb_batch_process_manager::
set_input(unsigned i, brdb_value_sptr const& input)
{
  if (!current_process_) {
    std::cout << "In bprb_batch_process_manager::set_input(.) - null process\n";
    return false;
  }
  return current_process_->set_input(i, input);
}

//: set input from the database
bool bprb_batch_process_manager::set_input_from_db(unsigned i,
                                                   unsigned id,
                                                   const std::string& type)
{
  if (!current_process_)
    return false;

  // construct the name of the relation
  std::string relation_name = type + "_data";
  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  if (selec->size()!=1) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    return false;
  }
  brdb_value_sptr value;
  if (!selec->get_value(std::string("value"), value)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
    return false;
  }
  if (!value) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - null value\n";
    return false;
  }
  if (!current_process_->set_input(i, value)){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - can't set input on process\n";
    return false;
  }
  return true;
}

//: set input from the database
bool bprb_batch_process_manager::set_input_from_db(unsigned i,
                                                   unsigned id)
{
  if (!current_process_)
    return false;
 std::string type = current_process_->input_type(i);
  // construct the name of the relation
  std::string relation_name = type + "_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  if (!selec||selec->size()!=1) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    return false;
  }
  brdb_value_sptr value;
  if (!selec->get_value(std::string("value"), value)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
    return false;
  }
  if (!value) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - null value\n";
    return false;
  }
  if (!current_process_->set_input(i, value)){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) - can't set input on process\n";
    return false;
  }
  return true;
}

//: put the output into the database
bool bprb_batch_process_manager::commit_output(unsigned i, unsigned& id)
{
  std::string type;
  return commit_output(i, id, type);
}

//: put the output into the database
bool bprb_batch_process_manager::commit_output(unsigned i, unsigned& id, std::string &type)
{
  if (!current_process_)
    return false;
  /*std::string */type = current_process_->output_type(i);
  // construct the name of the relation
  std::string relation_name = type + "_data";
  // construct the tuple
  id = brdb_database_manager::id();
  brdb_value_sptr vid = new brdb_value_t<unsigned>(id);
  brdb_value_sptr value = current_process_->output(i);
  if (!value) {
    std::cout << "bprb_batch_process_manager::commit_output(.) - null process output\n";
    return false;
  }
  brdb_tuple_sptr t = new brdb_tuple();
  t->add_value(vid); t->add_value(value);
  if (!DATABASE->add_tuple(relation_name, t)) {
    std::cout << "bprb_batch_process_manager::commit_output(.) - failed to add tuple to relation "
             << relation_name << std::endl;
    return false;
  }
  return true;
}

bool bprb_batch_process_manager::commit_all_outputs(std::vector<unsigned>& ids)
{
  if (!current_process_)
    return false;
  ids.clear();
  for (unsigned i = 0; i<current_process_->n_outputs(); ++i){
    unsigned id;
    std::string type;
    if (!this->commit_output(i, id, type))
      return false;
    else
      ids.push_back(id);
  }
  return true;
}

//: remove data from the database
bool bprb_batch_process_manager::remove_data(unsigned id)
{
  bool removed = false;
  std::set<std::string> names = DATABASE->get_all_relation_names();
  for (auto nit = names.begin();
       nit != names.end()&&!removed; ++nit) {
    // query to get the data
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

    brdb_selection_sptr selec = DATABASE->select(*nit, std::move(Q));
    if (selec->size()!=1)
      continue;
    selec->delete_tuples();
    removed = true;
  }
  return removed;
}

//: initialize before execution
bool bprb_batch_process_manager::process_init()
{
  bool to_return = false;
  if (!current_process_)
    return to_return;
  if (verbose_)
    std::cout << "Initializing process: " << current_process_->name()
             << std::endl;

  to_return = current_process_->init();

  return to_return;
}

//: Run a process on the current frame
bool bprb_batch_process_manager::run_process()
{
  bool to_return = false;
  if (!current_process_)
    return to_return;
  if (verbose_)
    std::cout << "Running process: " << current_process_->name() << std::endl;
  // EXECUTE ///////////////////////////////////////////////
  to_return = current_process_->execute();
  //////////////////////////////////////////////////////////

  // RETURN VALUE //////////////////////////////////////////
  return to_return;
  //////////////////////////////////////////////////////////
}

bool bprb_batch_process_manager::finish_process()
{
  bool to_return = false;
  if (!current_process_)
    return to_return;
  if (verbose_)
    std::cout << "Finish process: " << current_process_->name() << std::endl;
  // EXECUTE ///////////////////////////////////////////////
  to_return = current_process_->finish();
  //////////////////////////////////////////////////////////

  // RETURN VALUE //////////////////////////////////////////
  return to_return;
  //////////////////////////////////////////////////////////
}

void bprb_batch_process_manager::print_db()
{
  DATABASE->print();
}


bool bprb_batch_process_manager::set_stdout(const std::string& file)
{
   return std::freopen (file.c_str(),"a",stdout) != nullptr;
}


bool bprb_batch_process_manager::reset_stdout()
{
#ifdef WIN32
  return std::fclose(stdout) == 0 && std::freopen("CON","w",stdout) != NULL;
#else
  return std::fclose(stdout) == 0 && std::freopen("/dev/tty","w",stdout) != nullptr;
#endif
}


void bprb_batch_process_manager::b_write_db(const std::string& path)
{
  brdb_database_manager::save_database(path);
}

void bprb_batch_process_manager::b_read_db(const std::string& path)
{
  brdb_database_manager::load_database(path);
}
