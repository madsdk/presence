class UnpackingError(Exception):
    """The exception type thrown when unpacking errors occur."""
    def __init__(self, *args, **kwargs):
        super(UnpackingError, self).__init__(*args, **kwargs)

class ValidationError(Exception):
    """The exception type thrown when validation errors occur."""
    def __init__(self, *args, **kwargs):
        super(ValidationError, self).__init__(*args, **kwargs)
