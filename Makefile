CC = gcc
GSL_PATH ?= /net/ens/renault/save/gsl-2.6/install
CFLAGS = -std=c99 -Wall -Wextra -fPIC -g3 -I$(GSL_PATH)/include --coverage -lgcov
LDFLAGS = -lm -lgsl -lgslcblas -ldl \
	-L$(GSL_PATH)/lib -L$(GSL_PATH)/lib64 \
	-Wl,-rpath,${GSL_PATH}/lib


SRC_DIR = src
TEST_DIR = tst
BUILD_DIR = install
MAIN = server
TEST = alltests
CLIENTS_SRC = $(SRC_DIR)/player0.c $(SRC_DIR)/player1.c $(SRC_DIR)/player_hlinked.c
CLIENT2_SO = $(BUILD_DIR)/player0.so
CLIENTS3_SO = $(BUILD_DIR)/player0.so $(BUILD_DIR)/player1.so

CLIENTS_OBJ = $(CLIENTS_SRC:%.c=%.o)
CLIENTS_SO = $(CLIENTS_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.so)
SRCS = $(shell find $(SRC_DIR) -name '*.c') #$(TEST_DIR)/$(TEST).c
OBJS = $(SRCS:%.c=%.o) 
DEPS = $(SRCS:%.c=%.d) 
TEST_SRCS = $(shell find $(TEST_DIR) -name '*.c') #$(TEST_DIR)/$(TEST).c
TEST_OBJS = $(TEST_SRCS:%.c=%.o)
TEST_DEPS = $(SRCS:%.c=%.d) 
COMMONS = $(filter-out $(SRC_DIR)/$(MAIN).o $(TEST_DIR)/$(TEST).o $(CLIENTS_OBJ), $(OBJS))

.PHONY: clean

all: build

build: server client

build2: server client2

test: alltests

install: server client test

run: build
	$(BUILD_DIR)/$(MAIN) $(CLIENTS3_SO)

run2: build2
	$(BUILD_DIR)/$(MAIN) install/player0.so install/player0.1.so

runrandom: build
	$(BUILD_DIR)/$(MAIN) $(BUILD_DIR)/player0.so $(BUILD_DIR)/player0.1.so

clean:
	@rm -f *~ src/*~ *.o *.d $(BUILD_DIR)/* $(SRC_DIR)/*.[od] $(TEST_DIR)/*.[od] */*.gcno *.gcno

server: $(SRC_DIR)/server.o $(COMMONS) | inst
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS) 

client: $(CLIENTS3_SO)

client2: $(BUILD_DIR)/player0.so
	cp $(BUILD_DIR)/player0.so $(BUILD_DIR)/player0.1.so


$(BUILD_DIR)/%.so: $(SRC_DIR)/%.o $(COMMONS) | inst
	$(CC) -shared $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c %.d
	$(CC) -c $(CFLAGS) $< -o $@

$(SRC_DIR)/%.d: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -MF $@ -c $<

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c %.d -o $@
	$(CC) --coverage -lgcov -c $(CFLAGS) $<

$(TEST_DIR)/%.d: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -MF $@ -c $<

#alltests.d: $(TEST_DIR)/$(TEST).c
#	$(CC) $(CFLAGS) -MMD -MF $@ -c $< #lol


alltests: $(TEST_OBJS) $(COMMONS) | inst
	@echo $(TEST_SRCS)
	$(CC) --coverage -lgcov $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

inst:
	mkdir -p install

-include $(DEPS) $(TEST_DEPS)


