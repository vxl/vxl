// This is brl/bpro/bprb/bprb_batch_process_manager.cxx
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

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>

//: Constructor
bprb_batch_process_manager::bprb_batch_process_manager() : current_process_(0),
                                                           verbose_(true)
{
}

//: Destructor
bprb_batch_process_manager::~bprb_batch_process_manager()
{
}

bool bprb_batch_process_manager::clear()
{
  return true;
}

bool bprb_batch_process_manager::init_process(vcl_string const& process_name)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if (p) {
    if (verbose_)
      vcl_cout << "Process: " << p->name() << vcl_endl;
    current_process_ = p;
    return true;
  }
  vcl_cout << "ERROR!!!! Process: " << process_name << " is not FOUND\n";
  return false;
}

//: print the default values of the process into the specified XML file
bool bprb_batch_process_manager::print_default_params(vcl_string const & process_name, vcl_string const& params_XML)
{
  bprb_process_sptr p = get_process_by_name(process_name);
  if (p) {
    vcl_cout << "Printing default params for process: " << p->name() << vcl_endl;
    (p->parameters())->print_def_XML(p->name(), params_XML);
    return true;
  }
  return false;
}

//: read and set the parameters from an XML file for the current process
bool bprb_batch_process_manager::set_params(vcl_string const& params_XML)
{
  if (!current_process_) {
    vcl_cout << "In bprb_batch_process_manager::set_params(.) - null process\n";
    return false;
  }
  if (!current_process_->parse_params_XML(params_XML)) {
    vcl_cout << "In bprb_batch_process_manager::set_params(.) - not able to parse the XML file: "
             << params_XML << vcl_endl;
    return false;
  }
  return true;
}

//: set the parameters from another parameter instance for the current process
bool bprb_batch_process_manager::set_params(const bprb_parameters_sptr& params)
{
  if (!current_process_) {
    vcl_cout << "In bprb_batch_process_manager::set_params(.) - null process\n";
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
    vcl_cout << "In bprb_batch_process_manager::set_input(.) - null process\n";
    return false;
  }
  return current_process_->set_input(i, input);
}

//: set input from the database
bool bprb_batch_process_manager::set_input_from_db(unsigned i,
                                                   unsigned id,
                                                   vcl_string type)
{
  if (!current_process_)
    return false;

  // construct the name of the relation
  vcl_string relation_name = type + "_data";
  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select(relation_name, Q);
  if (selec->size()!=1) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    return false;
  }
  brdb_value_sptr value;
  if (!selec->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
    return false;
  }
  if (!value) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - null value\n";
    return false;
  }
  if (!current_process_->set_input(i, value)){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - can't set input on process\n";
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
 vcl_string type = current_process_->input_type(i);
  // construct the name of the relation
  vcl_string relation_name = type + "_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr selec = DATABASE->select(relation_name, Q);
  if (!selec||selec->size()!=1) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - no selections\n";
    return false;
  }
  brdb_value_sptr value;
  if (!selec->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - didn't get value\n";
    return false;
  }
  if (!value) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - null value\n";
    return false;
  }
  if (!current_process_->set_input(i, value)){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) - can't set input on process\n";
    return false;
  }
  return true;
}

//: put the output into the database
bool bprb_batch_process_manager::commit_output(unsigned i, unsigned& id)
{
  vcl_string type;
  return commit_output(i, id, type);
}

//: put the output into the database
bool bprb_batch_process_manager::commit_output(unsigned i, unsigned& id, vcl_string &type)
{
  if (!current_process_)
    return false;
  /*vcl_string */type = current_process_->output_type(i);
  // construct the name of the relation
  vcl_string relation_name = type + "_data";
  // construct the tuple
  id = brdb_database_manager::id();
  brdb_value_sptr vid = new brdb_value_t<unsigned>(id);
  brdb_value_sptr value = current_process_->output(i);
  if (!value) {
    vcl_cout << "bprb_batch_process_manager::commit_output(.) - null process output\n";
    return false;
  }
  brdb_tuple_sptr t = new brdb_tuple();
  t->add_value(vid); t->add_value(value);
  if (!DATABASE->add_tuple(relation_name, t)) {
    vcl_cout << "bprb_batch_process_manager::commit_output(.) - failed to add tuple to relation "
             << relation_name << vcl_endl;
    return false;
  }
  return true;
}

bool bprb_batch_process_manager::commit_all_outputs(vcl_vector<unsigned>& ids)
{
  if (!current_process_)
    return false;
  ids.clear();
  for (unsigned i = 0; i<current_process_->n_outputs(); ++i){
    unsigned id;
    vcl_string type;
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
  vcl_set<vcl_string> names = DATABASE->get_all_relation_names();
  for (vcl_set<vcl_string>::iterator nit = names.begin();
       nit != names.end()&&!removed; ++nit) {
    // query to get the data
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);

    brdb_selection_sptr selec = DATABASE->select(*nit, Q);
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
    vcl_cout << "Initializing process: " << current_process_->name()
             << vcl_endl;

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
    vcl_cout << "Running process: " << current_process_->name() << vcl_endl;
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
    vcl_cout << "Finish process: " << current_process_->name() << vcl_endl;
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


bool bprb_batch_process_manager::set_stdout(vcl_string file)
{
   return vcl_freopen (file.c_str(),"a",stdout) != NULL;
}


bool bprb_batch_process_manager::reset_stdout()
{
#ifdef WIN32
  return vcl_fclose(stdout) == 0 && vcl_freopen("CON","w",stdout) != NULL;
#else
  return vcl_fclose(stdout) == 0 && vcl_freopen("/dev/tty","w",stdout) != NULL;
#endif
}


void bprb_batch_process_manager::b_write_db(const vcl_string& path)
{
  brdb_database_manager::save_database(path);
}

void bprb_batch_process_manager::b_read_db(const vcl_string& path)
{
  brdb_database_manager::load_database(path);
}
