CLIENT = part_b_client
SERVER = part_b_server

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = part_b.x

TARGETS_SVC.c = part_b_svc.c part_b_server.c part_b_xdr.c 
TARGETS_CLNT.c = part_b_clnt.c part_b_client.c part_b_xdr.c 
TARGETS = part_b.h part_b_xdr.c part_b_clnt.c part_b_svc.c part_b_client.c part_b_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)

# Compiler flags 
CFLAGS += -g 
LDLIBS += -lnsl

# Targets 
all : $(CLIENT) $(SERVER)

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT).out $(OBJECTS_CLNT) $(LDLIBS) 

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 


$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER).out $(OBJECTS_SVC) $(LDLIBS)

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

clean:
	@rm -rf *.o *.out *.txt
	@echo "Object, executable and text files are cleaned"

