from servicename import ServiceName
from servicedata import ServiceData
from packed_data import UInt16, UInt32, MaxLengthCString, UnpackingError, ValidationError

class PresenceService(object):
    SERVICE_NAME_SIZE = 32
    SERVICE_DATA_SIZE = 200

    """A Service that may be announced via the Presence daemon."""
    def __init__(self, name = 'unnamed', port  = 0, data = ''):
        """Constructor. Creates a presence service object that can be registered with the
        Presence daemon.
        @type name: string
        @param name: The name of the service. This must be between 1 and 32 characters.
        @type port: int
        @param port: The port that the service is listening on. This must be a number 
        between 0 and 65535.
        @type data: string
        @param data: The (optional) service data. Currently this can at most be 184 bytes.
        @raise PresenceException: Raised if the input parameters are invalid.
        """
        super(PresenceService, self).__init__()
        self._name = ServiceName(name)
        self._port = UInt16(port)
        self._data = ServiceData(data)

    def name(): #@NoSelf
        def fget(self):
            return self._name.name
        def fset(self, value):
            self._name.name = value
        return locals()
    name = property(**name())
                
    def port(): #@NoSelf
        def fget(self):
            return self._port.value
        def fset(self, value):
            self._port.value = value
        return locals()
    port = property(**port())

    def data(): #@NoSelf
        def fget(self):
            return self._data.data
        def fset(self, value):
            self._data.data = value
        return locals()
    data = property(**data())
    
    def pack(self):
        """Packs the service into a string for transfer through a socket."""
        return ''.join([self._name.pack(), self._port.pack(), self._data.pack()])

    def unpack(self, string_repr):
        try:
            idx = self._name.unpack(string_repr)
            idx += self._port.unpack(string_repr[idx:])
            idx += self._data.unpack(string_repr[idx:])
            return idx
        except (UnpackingError, ValidationError), excep:
            raise UnpackingError('Error unpacking service description. Message was="%s"'%excep.args[0])
