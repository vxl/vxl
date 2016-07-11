from types import ModuleType
import os
import imp
import sys

class MetaFinder(object):
    def __init__(self):
        self.modules = {}

    def find_module(self, fullname, path=None):
        try:
            assert(path is None)
            #brl python vxl does not have subpackages, so it should always be 
            #None

            adaptor, rest = fullname.split('_adaptor_')
            batch, _ = rest.split('_batch')

            file, pathname, description = imp.find_module(adaptor+'_adaptor')
            code = file.read()
            file.close()
            pathname = os.path.join(os.path.split(pathname)[0], fullname+'.py')

            code = compile(code, pathname, "exec", dont_inherit=True)

            module = imp.new_module(fullname)
        except:
            return None

        try: #This try except is separate from the rest to try to maintain a
        #clear and useful debug message in case the adaptor has an error
            exec code in module.__dict__
        except:
            print 'There was a problem importing', adaptor+'_adaptor'
            __import__(adaptor+'_adaptor')
            return None

        try:
            assert isinstance(module.batch, DummyBatch)
            #This is how we verify this is a PROPER VXL core module!!!

            _register_batch(module, batch)

            self.modules[fullname] = module

            return self
        except:
            pass

    def load_module(self, fullname):
        #I COULD pop self.modules... but it seems too risky. I don't know 
        #enough about what calls load_modules

        sys.modules[fullname] = self.modules[fullname]
        #Add to the system dictionary

        return self.modules[fullname]

import sys
sys.meta_path.append(MetaFinder())

class DummyBatch(object):
    def __getattr__(self, *args, **kwargs):
        print 'Core adaptors MUST be imported differently. Your must specify'
        print 'what batch to use. For example:'
        print
        print '>> import brl_init'
        print '>> import vil_adaptor_boxm2_batch'
        print
        print 'To import vil_adaptor against boxm2_batch'
        print
        print
        print 'Non-core adaptors must have the following lines at the'
        print 'beginning of each file:'
        print '  import brl_init'
        print '  import {Some}_batch'
        print '  batch = {Some}_batch'
        print '  dbvalue = brl_init.dbvalue_factory(batch)'
        print
        print 'Where {Some} is the batch for that non-core adaptor, such as'
        print 'bvxm, boxm2, etc..'
        print 
        print 'Contrib (non-core) adaptors are imported like normal, for'
        print 'example:'
        print
        print '>> import boxm2_adaptor'

        raise Exception('Batch not registered')

def _register_batch(mod, batch):
    ''' This should be called by each adaptor'''
    if isinstance(batch, ModuleType):
        mod.batch = batch
    else:
        if not batch.endswith('_batch'):
            batch+='_batch'
        mod.batch = __import__(batch)
    mod.batch.register_processes()
    mod.batch.register_datatypes()
    mod.dbvalue = dbvalue_factory(mod.batch)

def set_smart_register(value=True):
    set_smart_register.value = value
set_smart_register.value=False

def dbvalue_factory(batch):
    #dbvalue factory for different batches
    return type('dbvalue', (dbvalue,), {'batch':batch})

class dbvalue(object):
    batch = DummyBatch()

    def __init__(self, index, type):
        self.id = index    # unsigned integer
        self.type = type   # string

    def __del__(self):
        if set_smart_register.value and not hasattr(self, '__ref__'):
            # __ref__ is only an attr if it's a copy, don't remove_data on
            # delete
            self.batch.remove_data(self.id)

    def __copy__(self):
        import copy
        x = dbvalue(self.id, self.type)
        if set_smart_register.value:
            x.__ref__ = self  # Create a linked list of refs
        return x

    def __deepcopy__(self, memo):
        return self.__copy__()
