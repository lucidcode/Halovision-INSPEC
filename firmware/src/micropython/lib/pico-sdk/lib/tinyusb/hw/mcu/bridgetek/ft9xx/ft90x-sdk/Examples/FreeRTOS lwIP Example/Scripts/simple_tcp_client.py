#!/usr/bin/python
########################################################################################################################
# This is a simple TCP socket client.
########################################################################################################################
import sys
import socket
from time import sleep

################################################################################
# Emulation configurations.
################################################################################
if len(sys.argv) > 2:
    server_address = sys.argv[1]
    server_port = int(sys.argv[2])
elif len(sys.argv) > 1:
    server_address = sys.argv[1]
    server_port = 80
else:
    server_address = '192.168.1.190'
    server_port = 80

################################################################################
# Emulation process.
################################################################################
i = 0
while True:

    # initialize connection
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server = (server_address, server_port)
    try:
        print "Connecting to TCP server %s on port %s ... [%d]\n" % (server_address, server_port, i)
        client_socket.connect(server)
        i += 1
    except:
        print "Error: could not connect to server. Please check if server is running!"
        client_socket.close()
        continue

    # send data to server
    data = "Hello server\n\0"
    client_socket.sendall(data)
    client_socket.settimeout(None)
    print >>sys.stderr, ">> Sent " + str(len(data)) + " bytes" + " to: " + str(server)
    print >>sys.stderr, ">> %s " % data
		
    # receive response from server
    response = client_socket.recv(1024)
    print >>sys.stderr, ">> Received " + str(len(response)) + " bytes" + " from: " + str(server)
    print >>sys.stderr, ">> %s " % response

    client_socket.close()
    print 'Connection closed!\n'		
    print "============================================================================\n"
    sleep(0.5)
