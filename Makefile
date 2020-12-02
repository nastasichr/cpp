CFLAGS   ?=    # <- Something can be passed from the command line, e.g, -O1
SRC_DIR  ?= src
OBJ_DIR  := obj
TEST_DIR := tests

CXX  := g++

TEST_SRCS := $(wildcard $(TEST_DIR)/test_*.cpp)
TEST_OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))
TEST_DEPS := $(patsubst %.o, %.d, $(TEST_OBJS))
TEST_BINS := $(patsubst %.cpp, $(OBJ_DIR)/%.bin, $(wildcard $(TEST_DIR)/test_*.cpp))
TEST_RESS := $(patsubst %.bin, %.PASSED, $(TEST_BINS))

SRCS := $(wildcard src/*.cpp)
SRCS += $(filter-out $(TEST_SRCS), $(wildcard $(TEST_DIR)/*.cpp))
OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

INC_DIRS  := $(shell find $(SRC_DIR) -type d)
INC_DIRS  += $(shell find $(TEST_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(CFLAGS) $(INC_FLAGS) -g -std=c++17 -Wall -Werror -MMD

all: $(TEST_RESS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) $(LDFLAGS) $< -o $@

%.PASSED: %.bin
	./$< && touch $@

%.bin: %.o $(OBJS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $< -o $@ $(OBJS) $(LOADLIBES) $(LDLIBS)

-include $(DEPS)
-include $(TEST_DEPS)

keep-alive-intermetiates: $(OBJS) $(TEST_OBJS) $(DEPS) $(TEST_DEPS) $(TEST_BINS)

.PHONY: clean
clean:
	rm -rf $(TEST_OBJS)
	rm -rf $(TEST_DEPS)
	rm -rf $(TEST_BINS)
	rm -rf $(TEST_RESS)
	rm -rf $(OBJS)
	rm -rf $(DEPS)
