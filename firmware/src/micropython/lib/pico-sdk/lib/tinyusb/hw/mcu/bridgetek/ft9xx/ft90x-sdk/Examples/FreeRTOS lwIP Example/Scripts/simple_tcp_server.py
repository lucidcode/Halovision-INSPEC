#!/usr/bin/python
########################################################################################################################
# This is a simple TCP socket server.
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
listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listen_socket.bind((HOST, PORT))
listen_socket.listen(2)

i = 0
while True:

    try:
        # wait for a client connection
        print "Serving TCP connection on port %s ... [%s]\n" % (PORT, i)		
        client_connection, client_address = listen_socket.accept()
        i += 1
		
        # receive data from client
        data = client_connection.recv(1024)
        if not data:
            client_connection.close()
            break
        print >>sys.stderr, ">> Received " + str(len(data)) + " bytes" + " from: " + str(client_address)
        print >>sys.stderr, ">> %s " % data
		
        # send response to client
        response = "Hello client\n\0"
        client_connection.sendall(response)
        print >>sys.stderr, ">> Sent " + str(len(response)) + " bytes" + " to: " + str(client_address)
        print >>sys.stderr, ">> %s " % response
		
    except socket.timeout:
        print "Timed out"
        break
		
    finally:
        # close connection
        client_connection.close()
        print 'Connection closed!\n'
        print "============================================================================\n"
    

        
