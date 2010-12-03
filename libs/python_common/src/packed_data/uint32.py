import struct
from pd_exceptions import UnpackingError, ValidationError

class UInt32(object):
    def __init__(self, value=0):
        super(UInt32, self).__init__()
        self.value = value
        
    def value():
        doc = """Property for the value member."""
        def fget(self):
            return self._value
        def fset(self, value):
            if not type(value) in (int, long):
                raise ValidationError("Value must be of type int.")
            if value < 0 or value > 4294967295:
                raise ValidationError("Value must be between 0 and 4294967295.")
            self._value = value
        return locals()
    value = property(**value())
    
    def pack(self):
        return struct.pack("!I", self.value)

    def unpack(self, string_repr):
        try:
            self.value = struct.unpack("!I", string_repr[:4])[0]
            return 4
        except Exception, excep:
            raise UnpackingError('Error unpacking uint32. Message was="%s"'%excep.args[0])
