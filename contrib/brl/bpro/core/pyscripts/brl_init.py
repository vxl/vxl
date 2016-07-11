from types import ModuleType
import os
import imp
import sys

class MetaFinder(object):
    def __init__(self):
        self.modules = {}

    def find_module(self, fullname, path=None):
        try:
            print fullname, path
            adaptor, rest = fullname.split('_adaptor_')
            batch, _ = rest.split('_batch')
            module_name = fullname

            file, pathname, description = imp.find_module(adaptor+'_adaptor')
            code = file.read()
            file.close()
            pathname = os.path.join(os.path.split(pathname)[0], module_name+'.py')

            code = compile(code, pathname, "exec", dont_inherit=True)

            module = imp.new_module(module_name)
        except:
            return None

        try: #This try except is separate from the rest to try to maintain a
        #clear and useful debug message
            sys.modules[module_name] = module
            exec code in module.__dict__
        except:
            sys.modules.pop(module_name)
            __import__(adaptor+'_adaptor')
            return None

        try:
            assert isinstance(module.batch, DummyBatch)
            #This is how we verify this is a PROPER VXL core module!!!

            _register_batch(module, batch)

            self.modules[fullname] = module

            return self
        except:
            sys.modules.pop(module_name)
            pass

    def load_module(self, fullname):
        #I COULD pop self.modules... but it seems too risky. I don't know 
        #enough about what calls load_modules
        return self.modules[fullname]

import sys
sys.meta_path.append(MetaFinder())

class DummyBatch(object):
    def __getattr__(self, *args, **kwargs):
        print 'You MUST called register_batch on your core adaptors'
        print 'For example:'
        print '>> import vil_adaptor'
        print ">> vil_adaptor.register_batch('boxm2')"
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
    mod.dbvalue = type('dbvalue', (dbvalue,), {'batch':mod.batch})

def get_register_batch(this):
    def register_batch(batch):
        ''' Register this adaptor against a specific batch

            batch - Name or imported module, name can optionally include _batch 
                    suffix'''
        _register_batch(this, batch)
    return register_batch


def reimport(name, batch):
    import os
    import imp
    import sys
    
    if not name.endswith('_adaptor'):
        name+='_adaptor'

    if not isinstance(batch, ModuleType):
        if not batch.endswith('_batch'):
            batch+='_batch'
        batch = __import__(batch)


    file, pathname, description = imp.find_module(name)

    #pathname=os.path.split(pathname)
    module_name = name+'_'+batch.__name__
    pathname = module_name+'.py'

    code = compile(file.read(), pathname, "exec", dont_inherit=True)
    file.close()

    module = imp.new_module(module_name)
    sys.modules[module_name] = module
    exec code in module.__dict__

    _register_batch(module, batch)

    return module

def set_smart_register(value=True):
    set_smart_register.value = value
set_smart_register.value=False

class dbvalue(object):
    batch = None

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
