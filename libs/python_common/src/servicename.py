from packed_data import MaxLengthCString, ValidationError

class ServiceName(MaxLengthCString):
    SERVICE_NAME_SIZE = 32

    def __init__(self, name='unnamed'):
        super(ServiceName, self).__init__(ServiceName.SERVICE_NAME_SIZE)
        self.name = name
        
    def name(): #@NoSelf
        doc = """Property for name."""
        def fget(self):
            return self.string
        def fset(self, value):
            if len(value) == 0:
                raise ValidationError("A service name cannot be the empty string.")
            self.string = value
        return locals()
    name = property(**name())
    
    
