CLIENT = part_c_client
SERVER = part_c_server
LOGGER = part_c_logger
WRAPPER = part_c_server_wrapper

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = part_c.x

TARGETS_SVC.c = part_c_svc.c part_c_server.c part_c_xdr.c 
TARGETS_CLNT.c = part_c_clnt.c part_c_client.c part_c_xdr.c 
TARGETS = part_c.h part_c_xdr.c part_c_clnt.c part_c_svc.c part_c_client.c part_c_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)

# Compiler flags 
CFLAGS += -g 
LDLIBS += -lnsl

# Targets 

all : $(CLIENT) $(SERVER) $(LOGGER) $(WRAPPER)

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT).out $(OBJECTS_CLNT) $(LDLIBS) 

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o part_c_server_wrapped.out $(OBJECTS_SVC) $(LDLIBS)

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 


$(LOGGER) : $(LOGGER).c
	gcc $(LOGGER).c -o $(LOGGER).out

$(WRAPPER) : $(WRAPPER).c
	gcc $(WRAPPER).c -o $(SERVER).out


clean:
	@rm -rf *.txt *.log *.o *.out
	@echo "Object and output files are successfully removed."