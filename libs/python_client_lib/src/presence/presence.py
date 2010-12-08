# Copyright (C) 2008, Mads D. Kristensen
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
from __future__ import with_statement
from threading import Thread
from thread import allocate_lock
import select, socket, logging
from presenceservice import PresenceService
from presenceexceptions import PresenceException
from nodename import NodeName
from packed_data import UInt16

class PresenceDefines:
    """A static class containing the 'defines' of the Presense system. This is 
    things such as packet length, command structure, etc."""
    ANNOUNCEMENT_PORT = 12345

class Presence(Thread):
    """This class is the presence consumer monitor thread. It monitors the 
    local network and passes any broadcasted Presence packets on to its
    subscribers.."""
    
    TIMEOUT = 0.1
    
    def __init__(self, address=('', PresenceDefines.ANNOUNCEMENT_PORT)):
        """
        Constructur. 
        @type address: (str, int)
        @param address: The address to list for Presence announcements on. 
        Since Presence packets are broadcast the address would normally be
        '' or '<broadcast>' while the port may be any valid port number.
        """
        # Init super class.
        super(Presence, self).__init__()
        
        # Store member vars.
        self.address = address
        self._sock = None
        self._shutdown = False
        self._shutdown_signal = allocate_lock()
        self._callbacks = {}
        self._callbacks_lock = allocate_lock()
        self._error_callback = None
        
        self.setDaemon(True)
    
    def address(): #@NoSelf
        doc = 'Property (get/set/del) for the address tuple.'
        def fget(self):
            return self._address
        def fset(self, value):
            # Type checking.
            if not (type(value) == tuple and len(value) == 2 and type(value[0]) == str and type(value[1]) == int):
                raise PresenceException('Invalid address given. Type must be (str, int).')
            # Check port.
            if value[1] < 1024 or value[1] > 65535:
                raise PresenceException('Invalid address given. Port must be within the range [1024;65535].')
            self._address = value
        def fdel(self):
            del self._address
        return locals()
    address = property(**address())

    def connect(self):
        """This function starts the monitoring thread.  It is merely 
        here to make this version of Presence adhere to the same
        interface as the other version :-)"""
        self.start()
                
    def set_error_callback(self, cb_func):
        """
        Set an error handler function. This function will be called when an error
        occurs in the thread that monitors incoming commands/announcements from 
        the Presence daemon. 
        The callback function must accept two parameters: fatal (bool) and 
        error (Exception or subclass thereof). If 'fatal' is True the connection to 
        the daemon has broken and reconnection may be tried.
        @type cb_func: function
        @param cb_func: The function that will be invoked upon errors in the thread.
        """
        self._error_callback = cb_func
        
    def subscribe(self, name, callback_function):
        """
        Subscribe to all announcements of a named service. Whenever an announcement 
        arrives the given callback function will be called.
        @type name: str
        @param name: The name of the service that is being subscribed to.
        @type callback_function: function
        @param callback_function: The callback function that will be called whenever a 
        service announcement for the 'name' service arrives. This function must accept 
        three parameters: The first parameter is the name of the provider, the second 
        is the address of the provider, and the third is the service announced. The 
        types of these parameters are (str, str, PresenceService).
        """
        with self._callbacks_lock:
            if not self._callbacks.has_key(name):
                self._callbacks[name] = []
            self._callbacks[name].append(callback_function)
        
    def unsubscribe(self, name, callback_function):
        """
        Remove a callback function from the callback list of the named service.
        @type name: str
        @param name: The name pf ther service that the subscribtion is removed from.
        @type callback_function: function
        @param callback_function: The callback function that was attached to the subscribtion.
        @raise PresenceException: If the subscribtion does not exist.
        """
        # Remove the callback from _callbacks.
        if self._callbacks.has_key(name):
            if callback_function in self._callbacks[name]:
                self._callbacks[name].remove(callback_function)
                if len(self._callbacks[name]) == 0:
                    self._callbacks.pop(name)
            else:
                raise PresenceException('This function is not registered to receive callbacks.')
        else:
            raise PresenceException('Unknown service name. No callback handler exists.')
            
    def __handle_monitor_error(self, is_fatal, error):
        """
        This internally called method handles errors from the monitor thread (the thread that 
        receives incoming commands and service annoucements from the Presence daemon).
        @type is_fatal: bool
        @param is_fatal: A flag indicating whether the error was fatal, i.e., whether or 
        not the thread is still running.
        @type error: Exception (or a subclass thereof).
        @param error: The exception raised in the monitoring thread.
        """
        if self._error_callback:
            # The users wants to handle such errors.
            self._error_callback(is_fatal, error)
        else:
            # No handler has been registered. Use standard logging.
            if is_fatal:
                logging.getLogger('presence').exception('Fatal error in monitor thread.')
            else:
                logging.getLogger('presence').info('Error in monitor thread.', exc_info=True)
            
    def run(self):
        """ Main thread function. """
        logger = logging.getLogger('presence')
        logger.debug('Monitoring thread started.')
        
        # Start by opening the UDP socket.
        try:
            self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self._sock.bind(self.address)
        except Exception, e:
            self.__handle_monitor_error(True, e)
            return

        # Start monitoring the UDP socket for announcement packets.
        with self._shutdown_signal:
            while not self._shutdown:
                (rd, _, _) = select.select([self._sock], [], [self._sock], Presence.TIMEOUT)
                if rd != []:
                    # Some data is ready to be read from the socket.
                    try:
                        message, sender = self._sock.recvfrom(4096)
                    except Exception, e:
                        # Error reading from a socket object that has just told us it has data?
                        # This is a fatal error.
                        self.__handle_monitor_error(True, e)
                        break

                    # Check that the entire packet has been received.
                    packet_size = UInt16()
                    idx = packet_size.unpack(message)
                    if not len(message[idx:]) == packet_size.value:
                        continue

                    # Read in the announcement header.
                    try:
                        provider_name = NodeName()
                        idx += provider_name.unpack(message[idx:])
                        service_count = UInt16()
                        idx += service_count.unpack(message[idx:])
                    except:
                        # Just skip this packet and silently ignore this error.
                        logger.debug('Error parsing announcement header.', exc_info=True)
                        continue

                    # Run through the message sorting out the service announcements.
                    for _ in range(0, service_count.value):
                        # Read in the service.
                        try:
                            service = PresenceService()
                            idx += service.unpack(message[idx:])
                        except:
                            # Just skip this packet and silently ignore this error.
                            logger.debug('Error parsing service header.', exc_info=True)
                            break
                        
                        # Create a service object - if any body is interested in this kind 
                        # of service.
                        with self._callbacks_lock:
                            if self._callbacks.has_key(service.name) or self._callbacks.has_key('*'):
                                # Some one is interested. Send them the service object.
                                listeners = (self._callbacks['*'] if self._callbacks.has_key('*') else []) +\
                                            (self._callbacks[service.name] if self._callbacks.has_key(service.name) else [])
                                for callback in listeners:
                                    try:
                                        callback(provider_name.name, sender[0], service)
                                    except:
                                        # Ignore errors beyond our control... 
                                        pass
                    
        # Clean shutdown.
        self._shutdown = False
        logging.getLogger('presence').debug('Monitoring thread stopped.')

    def shutdown(self, block = False):
        """
        Asks Presence to shut down its monitoring thread.
        @type block: bool
        @param block: Whether or not the call should block until the thread has shut down.
        """
        self._shutdown = True
        if block:
            self._shutdown_signal.acquire()
            self._shutdown_signal.release()
        self._shutdown = False
    
