from nodename import NodeName
from packed_data import UInt32, UnpackingError, ValidationError

class PresencePeer(object):
    def __init__(self, name="unnamed", address=0):
        super(PresencePeer, self).__init__()
        self._name = NodeName(name)
        self._address = UInt32()
        self.address = address
        
    def name(): #@NoSelf
        doc = """Property for name."""
        def fget(self):
            return self._name.name
        def fset(self, value):
            self._name.name = value
        return locals()
    name = property(**name())

    def address(): #@NoSelf
        doc = """Property for address."""
        def fget(self):
            return self._address.value
        def fset(self, value):
            self._address.value = value
        def fdel(self):
            del self._address
        return locals()
    address = property(**address())
    
    def address_str(): #@NoSelf
        doc = """Address property that returns a string representation."""
        def fget(self):
            return self._address.pack()
        return locals()
    address_str = property(**address_str())
    
    def unpack(self, string_repr):
        """Unpacks a PresencePeer object from a serialized string representation.
        This implementation assumes that the given string is valid - otherwise 
        exceptions will be thrown when unpacking."""
        try:
            idx = self._name.unpack(string_repr)
            idx += self._address.unpack(string_repr[idx:])
            return idx
        except (UnpackingError, ValidationError), excep:
            raise UnpackingError('Error unpacking peer address. Message was="%s"', excep.args[0])
