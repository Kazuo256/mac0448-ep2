# dependencies
.temp/TCPconnection.o: src/TCPconnection.cxx src/TCPconnection.h \
 src/connection.h src/command.h
.temp/command.o: src/command.cxx src/command.h
.temp/connection.o: src/connection.cxx src/connection.h
.temp/prompt.o: src/prompt.cxx src/prompt.h src/command.h \
 src/connection.h
.temp/client.o: src/client.cxx src/prompt.h src/connection.h \
 src/TCPconnection.h src/UDPconnection.h src/eventmanager.h src/command.h
.temp/serverhandler.o: src/serverhandler.cxx src/serverhandler.h \
 src/commandhandler.h src/command.h src/connection.h src/serverdata.h \
 src/eventmanager.h
.temp/eventmanager.o: src/eventmanager.cxx src/eventmanager.h
.temp/serverdata.o: src/serverdata.cxx src/serverdata.h src/connection.h \
 src/eventmanager.h
.temp/server.o: src/server.cxx src/connection.h src/TCPconnection.h \
 src/UDPconnection.h src/command.h src/serverhandler.h \
 src/commandhandler.h src/eventmanager.h src/serverdata.h
.temp/UDPconnection.o: src/UDPconnection.cxx src/UDPconnection.h \
 src/connection.h src/command.h
