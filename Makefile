SOURCES = main.c configure.c open_serversocket.c incoming.c\
          socket_reader.c process_request.c answer_resource.c\
          popen_script.c write_log.c


stepney: ${SOURCES} header.h
	gcc ${SOURCES} -o stepney

clean:
	rm stepney
