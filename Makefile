
PROG_NAME = ep2
CLIENT_NAME = $(PROG_NAME)-client
SERVER_NAME = $(PROG_NAME)-server

SRC_DIR = src
OBJ_DIR = .temp
DOWNLOAD_DIR = downloads

CXX = g++
CXXFLAGS = -D_GNU_SOURCE -ansi -Wall -O0 $(FLAGS)

include objs.makefile

.PHONY: all
all: $(CLIENT_NAME) $(SERVER_NAME)

$(CLIENT_NAME): $(OBJ_DIR) $(CLIENT_OBJS) $(COMMON_OBJS) $(DOWNLOAD_DIR)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) $(COMMON_OBJS) -o $(CLIENT_NAME)

$(SERVER_NAME): $(OBJ_DIR) $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) $(COMMON_OBJS) -o $(SERVER_NAME)

$(OBJ_DIR):
	mkdir $@

$(DOWNLOAD_DIR):
	mkdir $@

.temp/%.o: src/%.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@

.temp/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

.temp/%.o: src/%.cxx
	$(CXX) -c $(CXXFLAGS) $< -o $@

include deps.makefile

.PHONY: debug
debug: CXXFLAGS += -g -DEP2_DEBUG
debug: all

.PHONY: clean
clean:
	rm -rf $(CLIENT_NAME) $(SERVER_NAME) $(DOWNLOAD_DIR)
	rm -rf $(OBJ_DIR)


