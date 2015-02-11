#############################################################################
# Reconfigure boxm2_register's dbvalue as a smart pointer such that dbvalues
# that are no longer referenced by python are automatically removed from the
# database (have their reference count decremented).
#
# This script is for end-users; pyadaptors should continue to use 
# boxm2_register and boxm2_batch. 
# This setup scirpt must be called first, before any other adaptor imports
# boxm2_register. 
# If by chance you get a naked reference to a database object that does not
# get wrapped in a dbvalue (such as a float or int), it must be removed 
# directly with boxm2_batch.remove_data(), as the version in boxm2_register 
# will be disabled (should not happen as they should imediately be converted 
# to their primitive type...).
#############################################################################

import boxm2_register
def boxm2_register__dbvalue____del__(self):
  boxm2_register.boxm2_batch.remove_data(self.id)
boxm2_register.dbvalue.__del__ = boxm2_register__dbvalue____del__
def boxm2_register__dbvalue____copy__(self): 
  assert False, 'cannot copy a dbvalue'
boxm2_register.dbvalue.__copy__ = boxm2_register__dbvalue____copy__
def boxm2_register__dbvalue____deepcopy__(self, memo): 
  assert False, 'cannot copy a dbvalue'
boxm2_register.dbvalue.__deepcopy__ = boxm2_register__dbvalue____deepcopy__
def boxm2_register__remove_data(id):
  pass
boxm2_register.remove_data = boxm2_register__remove_data
