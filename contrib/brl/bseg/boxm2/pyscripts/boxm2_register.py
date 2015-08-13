#############################################################################
# Register Script to ensure boxm2_batch processes are only registered once
#
# to use our boxm2 python binding, be sure to add: 
#   <vxl_build_root>/lib/:<vxl_src_root>/contrib/brl/bseg/boxm2/pyscripts/ 
# to your PYTHONPATH environment variable.  
#############################################################################
import boxm2_batch;
boxm2_batch.not_verbose();
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();

smart_register = False
#Set to True to enable smart_register

#class used for python/c++ pointers in database
class dbvalue(object):
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

  def __del__(self):
    if smart_register:
      boxm2_batch.remove_data(self.id)
    else:
      super(dbvalue, self).__del__()

  def __deepcopy__(self):
    if smart_register:
      pass
    else:
      assert False, 'cannot copy a dbvalue'

  # TODO add a __del__ destructor which removes the data from the db
  # RE: requires taking ownership of the database id; we would either
  # have to specialize the copy method (__deepcopy__()), for example, to 
  # increment the reference count in the database, or make the database
  # fail silently if it can't remove an object.

def remove_data(id):
  if smart_register:
    print 'Warning: remove_data disabled when using smart_register'
  else:
    boxm2_batch.remove_data(id)

def get_output_float(id):
  fval = boxm2_batch.get_output_float(id)
  return fval

def get_output_unsigned(id):
  uval = boxm2_batch.get_output_unsigned(id)
  return uval

