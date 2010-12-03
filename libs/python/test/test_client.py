from presence import *
from time import sleep
from sys import stdout
import logging

logging.basicConfig(level=logging.DEBUG)

def service_callback(provider_name, provider_address, service):
    print 'Received announcement from %s at %s'%(provider_name, provider_address)
    print 'Service info: %s, %i, %s'%(service.name, service.port, service.data)

presence = Presence()

print 'Connecting to Presence daemon.'
presence.connect()

print 'Subscribing to "test_service" announcements and sleeping for 5 seconds.'
presence.subscribe('test_service', service_callback)
sleep(5)

print 'Unsubscribing from "test_service" and making a wildcard subscription instead for 10 seconds.'
presence.unsubscribe('test_service', service_callback)
presence.subscribe('*', service_callback)
sleep(10)

print 'Shutting down Presence.'
presence.shutdown(True)
