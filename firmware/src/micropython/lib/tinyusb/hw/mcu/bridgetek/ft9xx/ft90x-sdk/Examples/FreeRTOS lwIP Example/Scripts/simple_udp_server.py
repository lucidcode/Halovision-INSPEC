#!/usr/bin/python
########################################################################################################################
# This is a simple UDP socket server.
########################################################################################################################
import sys
import socket

################################################################################
# Emulation configurations.
################################################################################
HOST = ''
PORT = 9990

################################################################################
# Emulation process.
################################################################################
i = 0
while True:

    # initialize connection
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((HOST, PORT))
    print "Serving UDP connection on port %s ... [%s]\n" % (PORT, i)
    i += 1
	
    try:
        # receive data from client
        data, client = sock.recvfrom(1024)
        print >>sys.stderr, ">> Received " + str(len(data)) + " bytes" + " from: " + str(client)
        print >>sys.stderr, ">> %s " % data

        # send response to client
        response = "Hello client\n\0"
        sock.sendto(response, client)
        print >>sys.stderr, ">> Sent " + str(len(response)) + " bytes" + " to: " + str(client)
        print >>sys.stderr, ">> %s " % response

    except:
        print 'Exception!\n'

    finally:
        # close connection
        sock.close()
        print 'Connection closed!\n'
        print "============================================================================\n"

