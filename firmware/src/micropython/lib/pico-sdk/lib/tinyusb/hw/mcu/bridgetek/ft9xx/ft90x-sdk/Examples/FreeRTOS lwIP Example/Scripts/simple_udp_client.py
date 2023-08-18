#!/usr/bin/python
########################################################################################################################
# This is a simple UDP socket client.
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
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server = (server_address, server_port)
    print "Connecting to UDP server %s on port %s ... [%d]\n" % (server_address, server_port, i)
    i += 1

    sock.settimeout(3)
    try:
        # send data to server
        data = "Hello server\n\0"
        sock.sendto(data, server)
        print >>sys.stderr, ">> Sent " + str(len(data)) + " bytes" + " to: " + str(server)
        print >>sys.stderr, ">> %s " % data
	   
        # receive response from server
        response, server = sock.recvfrom(1024)
        print >>sys.stderr, ">> Received " + str(len(response)) + " bytes" + " from: " + str(server)
        print >>sys.stderr, ">> %s " % response
		
    except socket.timeout:
        print "Error: timed out could not connect to server. Please check if server is running!"
        sleep(1)

    except:
        print "Error: could not connect to server. Please check if server is running!"
        break
	
    finally:
        # close connection	
        sock.close()
        print 'Connection closed!\n'		
        print "============================================================================\n"
        sleep(0.5)
