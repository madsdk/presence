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
import struct, select, socket, Queue, logging
from presenceservice import PresenceService
from presencepeer import PresencePeer
from presenceexceptions import PresenceException
from packed_data import UnpackingError, ValidationError, UInt16
from nodename import NodeName
from servicename import ServiceName

class PresenceDefines:
    """A static class containing the 'defines' of the Presense system. This is 
    things such as packet length, command structure, etc."""
    LOCAL_PORT = 2000
    QUEUE_TIMEOUT = 2.0

    MAX_COMMAND_SIZE = 256

    # Command identifiers.
    REGISTER_CALLBACK = 1 
    REMOVE_CALLBACK = 2
    REGISTER_SERVICE = 3
    REMOVE_SERVICE = 4
    ERROR = 5
    ACK = 6
    ANNOUNCEMENT = 7
    NODE_NAME = 8
        
    # Error numbers.
    ERR_INVALID_MESSAGE = 0
    ERR_INVALID_COMMAND = 1
    ERR_INVALID_CONNECTION = 2
    ERR_SERVICE_EXISTS = 3
    ERR_NO_SUCH_CALLBACK = 4
            
class Presence(Thread):
    """This class represents a connection to the local Presence daemon."""
    
    TIMEOUT = 0.1
    
    def __init__(self, address=('localhost', PresenceDefines.LOCAL_PORT)):
        """
        Constructur. 
        @type address: (str, int)
        @param address: The address of the Presence daemon. Default value is ('localhost', 2000).
        @raise PresenceException: If the given address is invalid. This only means that simple
        type checking is done; it does not guarantee that it is possible to connect to the address.
        """
        # Init super class.
        super(Presence, self).__init__()
                
        # Store member vars.
        self.__address = address
        self.__sock = None
        self.__shutdown = False
        self.__shutdown_signal = allocate_lock()
        self.__lock = allocate_lock()
        self.__callbacks = {}
        self.__command_queue = Queue.Queue()
        self.__error_callback = None
        self.setDaemon(True)
    
    def address(): #@NoSelf
        doc = 'Property (get/set) for the address tuple.'
        def fget(self):
            return self.__address
        def fset(self, value):
            # Type checking.
            if not (type(value) == tuple and len(value) == 2 and type(value[0]) == str and type(value[1]) == int):
                raise PresenceException('Invalid address given. Type must be (str, int).')
            # Check port.
            if value[1] < 1024 or value[1] > 65535:
                raise PresenceException('Invalid address given. Port must be within the range [1024;65535].')
            # Check address.
            try:
                socket.inet_aton(value[0])
            except:
                raise PresenceException('Invalid address given. An IP address in dotted-quad notation is needed.')
            self.__address = value
        return locals()
    address = property(**address())

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
        self.__error_callback = cb_func
        
    def __close_connection(self):
        """This function shuts down the daemon connection when an error has occurred."""
        # Shut down the monitor thread.
        self.shutdown(True)
        # Close and reset the socket object.
        try:
            self.__sock.close()
        except:
            logging.getLogger('presence').info('Error closing conneciton. Exception was:', exc_info=True)
        self.__sock = None
        # Remove any registered callback objects.
        self.__callbacks = {}
        # Flush the command queue.
        self.__command_queue = Queue.Queue()
    
    def connect(self):
        """
        Connect to the Presence daemon.
        @raise PresenceException: Raised if an error occurs connecting to the
        local Presence daemon.
        """
        # Warn the user that she is already connected.
        if self.__sock != None:
            raise PresenceException('Attempting to connect while already connected.')
        # Create a socket object for the connection.
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Connect to the daemon.
        try:
            self.__sock.connect(self.__address)
        except Exception, e:
            logging.getLogger('presence').info('Error connecting to daemon. Exception was:', exc_info=True)
            self.__sock = None
            raise PresenceException('Error connecting to the Presence daemon. Is it running?', e)
        # Start the monitoring thread.
        self.start()
    
    def disconnect(self):
        """Disconnects from the local Presence daemon."""
        self.__close_connection()

    def get_node_name(self):
        """
        Fetches the name of the local Presence daemon.
        @raise PresenceException: If this function is called on an disconnected Presence 
        instance, if the daemon did not respond for some reason, or if a socket error occurs.
        """
        logger = logging.getLogger('presence')

        if self.__sock == None:
            raise PresenceException('Attempting to send command to Presence daemon while disconnected.')
        try:
            # Send the request to the daemon.
            self.__sock.send(struct.pack("!B", PresenceDefines.NODE_NAME))
        except Exception, e:
            # Error sending request to daemon. Close the connection.
            logger.info('Error sending command to daemon. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('Error sending request. Connection has been closed.', e)
        # Wait for the daemon to respond.
        try:
            _, msg = self.__command_queue.get(True, PresenceDefines.QUEUE_TIMEOUT)
            return msg
        except:
            # The daemon did not respond. Close the connection.
            logging.getLogger('presence').info('Daemon failed to respond to request. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('The daemon did not respond the command.')
            
    def register_service(self, service):
        """
        Registers a service with the daemon. This means that the daemon will start announcing
        that the local device is providing this service.
        @type service: PresenceService
        @param service: The service that is to be registered with Presence for announcement.
        @raise PresenceException: If this function is called on an disconnected Presence 
        instance, if the daemon does not respond, if the daemon responds with an 
        error message, or if a socket error occurs.
        """
        if self.__sock == None:
            raise PresenceException('Attempting to send command to daemon while disconnected.')
        try:
            # Send the request to the daemon.
            tokens = []
            tokens.append(struct.pack("!B", PresenceDefines.REGISTER_SERVICE))
            tokens.append(service.pack())
            self.__sock.send(''.join(tokens))
        except Exception, e:
            # Error sending request to daemon. Close the connection.
            logging.getLogger('presence').info('Error sending command to daemon. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('Error sending request. Connection has been closed.', e)
        # Wait for the daemon to ack the command.
        try:
            (response_type, msg) = self.__command_queue.get(True, PresenceDefines.QUEUE_TIMEOUT)
        except:
            # The daemon did not respond. Close the connection.
            logging.getLogger('presence').info('Daemon failed to respond to request. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('The daemon did not ack the receipt of the command.')
        # Check what was returned by the daemon. There are only two possibilities, ERROR or ACK.
        if response_type == PresenceDefines.ERROR:
            error_no, msg_text = msg
            raise PresenceException('Error registering service: %s (errno=%i)'%(msg_text, error_no))
    
    def update_service(self, service):
        """
        Updates an already registered service. This call is in effect just a renaming of register_service.
        @see: register_service
        """
        self.register_service(service)
        
    def remove_service(self, name):
        """
        Remove a service announcement from the Presence daemon. This means that the
        named service will no longer be announced.
        @type name: str
        @param name: The name of the service that is to be removed.
        @raise PresenceException: If this function is called on an disconnected Presence 
        instance, if the daemon does not respond, if the daemon responds with an 
        error message, or if a socket error occurs.
        """
        if self.__sock == None:
            raise PresenceException('Attempting to send command to daemon while disconnected.')

        # Send the request to the daemon.
        try:
            tokens = []
            tokens.append(struct.pack("!B", PresenceDefines.REMOVE_SERVICE))
            service_name = ServiceName(name)
            tokens.append(service_name.pack())
            self.__sock.send(''.join(tokens))
        except ValidationError, e:
            raise PresenceException("Invalid service name given.")
        except Exception, e:
            # Error sending request to daemon. Close the connection.
            logging.getLogger('presence').info('Error sending command to daemon. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('Error sending request. Connection has been closed.', e)

        # Wait for the daemon to ack the command.
        try:
            (response_type, msg) = self.__command_queue.get(True, PresenceDefines.QUEUE_TIMEOUT)
        except:
            # The daemon did not respond. Close the connection.
            logging.getLogger('presence').info('Daemon failed to respond to request. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('The daemon did not ack the receipt of the command.')
        
        # Check the returncode from the daemon.
        if response_type == PresenceDefines.ERROR:
            error_no, msg_text = msg
            raise PresenceException('Error unregistering service: %s (errno=%i)'%(msg_text, error_no))
        
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
        @raise PresenceException: If this function is called on an disconnected Presence 
        instance, if the daemon does not respond, if the daemon responds with an 
        error message, or if a socket error occurs.
        """
        if self.__sock == None:
            raise PresenceException('Attempting to send command to daemon while disconnected.')
        try:
            # Send the request to the daemon.
            tokens = []
            tokens.append(struct.pack("!B", PresenceDefines.REGISTER_CALLBACK))
            service_name = ServiceName(name)
            tokens.append(service_name.pack())
            self.__sock.send(''.join(tokens))
        except ValidationError, e:
            raise PresenceException("Invalid service name given.")
        except Exception, e:
            # Error sending request to daemon. Close the connection.
            logging.getLogger('presence').info('Error sending command to daemon. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('Error sending request. Connection has been closed.', e)
        # Wait for the daemon to ack the command.
        try:
            # We do not need to check exactly _what_ has been returned by the daemon.
            # It will always return ACK - if it returns at all...
            (_, _) = self.__command_queue.get(True, PresenceDefines.QUEUE_TIMEOUT)
        except:
            # The daemon did not respond. Close the connection.
            logging.getLogger('presence').info('Daemon failed to respond to request. Exception was:', exc_info=True)
            self.__close_connection()
            raise PresenceException('The daemon did not ack the subscription request.')
        # Store the callback function in __callbacks.
        if self.__callbacks.has_key(name):
            self.__callbacks[name].append(callback_function)
        else:
            self.__callbacks[name] = [callback_function]
        
    def unsubscribe(self, name, callback_function):
        """
        Remove a callback function from the callback list of the named service.
        @type name: str
        @param name: The name pf ther service that the subscribtion is removed from.
        @type callback_function: function
        @param callback_function: The callback function that was attached to the subscribtion.
        @raise PresenceException: If this function is called on an disconnected Presence 
        instance, if the daemon does not respond, and if the daemon responds with an 
        error message. This exception is also raised if the subscribtion does not exist and
        when a socket error occurs.
        """
        if self.__sock == None:
            raise PresenceException('Attempting to send command to daemon while disconnected.')
        
        # Remove the callback from __callbacks.
        if self.__callbacks.has_key(name):
            if callback_function in self.__callbacks[name]:
                self.__callbacks[name].remove(callback_function)
            else:
                raise PresenceException('This function is not registered to receive callbacks.')
        else:
            raise PresenceException('Unknown service name. No callback handler exists.')
            
        # If no more callback handlers exists for the given service we unsubscribe at 
        # the daemon now.
        if len(self.__callbacks[name]) == 0:
            self.__callbacks.pop(name)
            try:
                # Send the request to the daemon.
                tokens = []
                tokens.append(struct.pack("!B", PresenceDefines.REMOVE_CALLBACK))
                service_name = ServiceName(name)
                tokens.append(service_name.pack())
                self.__sock.send(''.join(tokens))
            except ValidationError, e:
                raise PresenceException("Invalid service name given.")
            except Exception, e:
                # Error sending request to daemon. Close the connection.
                logging.getLogger('presence').info('Error sending command to daemon. Exception was:', exc_info=True)
                self.__close_connection()
                raise PresenceException('Error sending request. Connection has been closed.', e)
            # Wait for the daemon to ack the command.
            try:
                (response_type, msg) = self.__command_queue.get(True, PresenceDefines.QUEUE_TIMEOUT)
            except:
                # The daemon did not respond. Close the connection.
                logging.getLogger('presence').info('Daemon failed to respond to request. Exception was:', exc_info=True)
                self.__close_connection()
                raise PresenceException('The daemon did not ack the receipt of the command.')
            if response_type == PresenceDefines.ERROR:
                error_no, msg_text = msg
                raise PresenceException('Error unsubscribing: %s (errno=%i)'%(msg_text, error_no))
        
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
        if self.__error_callback:
            # The users wants to handle such errors.
            self.__error_callback(is_fatal, error)
        else:
            # No handler has been registered. Use standard logging.
            if is_fatal:
                logging.getLogger('presence').exception('Fatal error in monitor thread. Exception was:')
            else:
                logging.getLogger('presence').info('Error from monitor thread. Exception was:', exc_info=True)
            
    def run(self):
        """
        Main thread function.
        In this thread the socket connection to the daemon is monitored. If service
        announcements arrive they are forwarded to their respective callback functions, 
        and if ACKs of ERRORs are received they are placed in the command queue.
        """
        logger = logging.getLogger('presence')
        logger.info('Monitoring thread started.')
        
        with self.__shutdown_signal:
            try:
                while not self.__shutdown:
                    (rd, _, _) = select.select([self.__sock], [], [self.__sock], Presence.TIMEOUT)
                    if rd != []:
                        # Data has arrived from the daemon.
                        message = self.__sock.recv(4096)
                        
                        length = UInt16()
                        idx = 0
                        packets = []
                        while idx < len(message):
                            try:
                                idx += length.unpack(message[idx:])
                                if len(message) < idx + length.value:
                                    raise ValidationError('Insufficient amount of data received')
                            except (UnpackingError, ValidationError), e:
                                self.__handle_monitor_error(False, e)
                                break
                            packets.append(message[idx:idx+length.value])
                            idx += length.value

                        for packet in packets:
                            # Find out which kind of packet we are dealing with here.
                            idx = 0
                            (packet_type,) = struct.unpack('!B', packet[0])
                            idx += 1

                            if packet_type == PresenceDefines.ANNOUNCEMENT:
                                #print "announcement received"
                                peer = PresencePeer()
                                service = PresenceService()
                                try:
                                    idx += peer.unpack(packet[idx:])
                                    idx += service.unpack(packet[idx:])
                                except (UnpackingError, ValidationError), e:
                                    self.__handle_monitor_error(False, e)
                                    continue

                                with self.__lock:
                                    try:
                                        # Check for "promiscuous mode" listeners.
                                        if self.__callbacks.has_key('*'):
                                            callback_functions = self.__callbacks['*']
                                            for callback_function in callback_functions:
                                                callback_function(peer._name.name, socket.inet_ntoa(peer.address_str), service)

                                        # Check for named listeners.
                                        if self.__callbacks.has_key(service.name):
                                            callback_functions = self.__callbacks[service.name]
                                            for callback_function in callback_functions:
                                                callback_function(peer._name.name, socket.inet_ntoa(peer.address_str), service)
                                    except Exception, e:
                                        self.__handle_monitor_error(False, e)

                            elif packet_type == PresenceDefines.ACK:
                                self.__command_queue.put((packet_type, ''))

                            elif packet_type == PresenceDefines.ERROR:
                                try:
                                    (error_number,) = struct.unpack("!B", packet[idx:idx+1])
                                    idx += 1
                                    error_message = MaxLengthCString(PresenceDefines.MAX_COMMAND_SIZE - 4)
                                    idx += error_message.unpack(packet[idx:])
                                    self.__command_queue.put((packet_type, (error_number, error_message)))
                                except Exception, e:
                                    self.__handle_monitor_error(False, e)
                                    break

                            elif packet_type == PresenceDefines.NODE_NAME:
                                try:
                                    node_name = NodeName()
                                    idx += node_name.unpack(packet[idx:])
                                    self.__command_queue.put((packet_type, node_name.name))
                                except Exception, e:
                                    self.__handle_monitor_error(False, e)
                            else:
                                logger.info('Received invalid message. Type was = %i'%packet_type)
                                break
            except Exception, e:
                # A fatal error has occurred.
                self.__sock.close()
                self.__sock = None
                self.__callbacks = {}
                self.__command_queue = Queue.Queue()
                self.__shutdown = False
                self.__handle_monitor_error(True, e)
                return
            
        # Clean shutdown.
        self.__shutdown = False
        logging.getLogger('presence').info('Monitoring thread stopped.')

    def shutdown(self, block = False):
        """
        Asks the Presence API to shut down its monitoring thread.
        @type block: bool
        @param block: Whether or not the call should block until the thread has shut down.
        """
        self.__shutdown = True
        if block:
            self.__shutdown_signal.acquire()
            self.__shutdown_signal.release()
        self.__shutdown = False
    
