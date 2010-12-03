from presence import Presence, PresenceException
from time import sleep
import logging
import struct

logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(levelname)s: %(message)s\n'\
                        '\t%(filename)s, %(funcName)s, %(lineno)s',
                    datefmt='%m/%d/%y %H:%M:%S')

p = Presence()
p.start()

def callback_function(provider_name, provider_address, service):
    print provider_name, 'at', provider_address, 'is offering the', service.name, 'service on port', service.port
    if service.name == 'scavenger':
        cpu_strength, cpu_cores, active_tasks, network_speed = struct.unpack("!fIII", service.data)
        print 'Scavenger info: strength =', cpu_strength, 'cores =', cpu_cores, 'active tasks =', active_tasks, 'network speed =', network_speed
    else:
        print 'service data has length', len(service.data)

p.subscribe('', callback_function)

sleep(5)

print 'stop it!'
p.shutdown()
print 'that is more like it...'

