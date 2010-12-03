from packed_data import MaxLengthCString, ValidationError

class NodeName(MaxLengthCString):
    NODE_NAME_SIZE = 32

    def __init__(self, name="unnamed"):
        super(NodeName, self).__init__(NodeName.NODE_NAME_SIZE)
        self.name = name
        
    def name():
        doc = """Property for name."""
        def fget(self):
            return self.string
        def fset(self, value):
            if len(value) == 0:
                raise ValidationError("A node name cannot be the empty string.")
            self.string = value
        return locals()
    name = property(**name())

