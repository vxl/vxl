global batch
def core_register():
    batch.register_processes()
    batch.register_datatypes()

smart_register = False
class dbvalue(object):

    def __init__(self, index, type):
        self.id = index    # unsigned integer
        self.type = type   # string

    def __del__(self):
        if smart_register and not hasattr(self, '__ref__'):
            # __ref__ is only an attr if it's a copy, don't remove_data on
            # delete
            boxm2_batch.remove_data(self.id)

    def __copy__(self):
        import copy
        x = dbvalue(self.id, self.type)
        if smart_register:
            x.__ref__ = self  # Create a linked list of refs
        return x

    def __deepcopy__(self, memo):
        return self.__copy__()

