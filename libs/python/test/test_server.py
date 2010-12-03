from presence import *
from time import sleep
from sys import stdout
import logging

logging.basicConfig(level=logging.DEBUG)

presence = Presence()

print 'Connecting to Presence daemon.'
presence.connect()

print 'Fetching daemon name.'
print presence.get_node_name()

print 'Registering the "test_service" service and announcing it for 5 seconds'
service = PresenceService('test_service', 4242, 'some service specific data')
presence.register_service(service)
for x in range(1, 6):
    sleep(1)
    print x,
    stdout.flush()
print
    
print 'Modifying service data and announcing it for 5 more seconds.'
service.data = 'some _other_ service specific data'
presence.update_service(service)
for x in range(1, 6):
    sleep(1)
    print x, 
    stdout.flush()
print

print 'Removing service announcement and sleeping for 5 seconds.'
presence.remove_service('test_service')
for x in range(1, 6):
    sleep(1)
    print x, 
    stdout.flush()
print

print 'Shutting down Presence.'
presence.shutdown(True)
