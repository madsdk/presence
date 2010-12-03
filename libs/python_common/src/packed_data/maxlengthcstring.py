import struct
from pd_exceptions import UnpackingError, ValidationError

class MaxLengthCString(object):
    def __init__(self, length, string = ""):
        super(MaxLengthCString, self).__init__()
        self._length = length
        self.string = string
        
    def string():
        doc = """Property for the string member."""
        def fget(self):
            return self._string
        def fset(self, value):
            # Check the string length.
            if len(value) > self._length:
                raise ValidationError("MaxLengthCString: string exceeds allowed length. Allowed=%i, actual=%i"%(self._length, len(value)))
            self._string = value
        return locals()
    string = property(**string())
    
    def pack(self):
        """Packs the string."""
        tokens = []
        tokens.append(struct.pack("!H", len(self.string)))
        tokens.append(self.string)
        return ''.join(tokens)

    def unpack(self, string_repr):
        try:
            string_length = struct.unpack("!H", string_repr[:2])[0]
            self.string = string_repr[2 : 2 + string_length]
            return 2 + string_length
        except Exception, excep:
            raise UnpackingError('Error unpacking string. Message was="%s"'%excep.args[0])
