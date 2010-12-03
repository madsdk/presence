import struct
from pd_exceptions import UnpackingError, ValidationError

class UInt16(object):
    def __init__(self, value=0):
        super(UInt16, self).__init__()
        self.value = value
        
    def value():
        doc = """Property for the value member."""
        def fget(self):
            return self._value
        def fset(self, value):
            if not type(value) in (int, long):
                raise ValidationError("Value must be of type int.")
            if value < 0 or value > 65535:
                raise ValidationError("Value must be between 0 and 65535.")
            self._value = value
        return locals()
    value = property(**value())
    
    def pack(self):
        return struct.pack("!H", self.value)

    def unpack(self, string_repr):
        try:
            self.value = struct.unpack("!H", string_repr[:2])[0]
            return 2
        except Exception, excep:
            raise UnpackingError('Error unpacking uint16. Message was="%s"'%excep.args[0])
