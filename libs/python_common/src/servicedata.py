from packed_data import MaxLengthCString

class ServiceData(MaxLengthCString):
    SERVICE_DATA_SIZE = 200

    def __init__(self, data=''):
        super(ServiceData, self).__init__(ServiceData.SERVICE_DATA_SIZE, data)

    def data(): #@NoSelf
        doc = """Property for data."""
        def fget(self):
            return self.string
        def fset(self, value):
            self.string = value
        return locals()
    data = property(**data())
    
    
