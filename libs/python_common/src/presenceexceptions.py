class PresenceException(Exception):
    """The only exception type thrown directly from Presence."""
    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)

