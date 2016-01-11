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
    if smart_register and not hasattr(self, '__ref__'):
      #__ref__ is only an attr if it's a copy, don't remove_data on delete
      boxm2_batch.remove_data(self.id)

  def __copy__(self):
    import copy
    x = dbvalue(self.id, self.type)
    if smart_register:
      x.__ref__ = self #Create a linked list of refs
    return x

  def __deepcopy__(self, memo):
    return self.__copy__()

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

