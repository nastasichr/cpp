TARGET   ?= test
SRC_DIRS ?= .

all: $(TARGET)
	@./$(TARGET)

CXX  := g++

SRCS := $(wildcard $(SRC_DIRS)/*.cpp)
OBJS := $(addsuffix .o,$(basename $(SRCS)))

INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -g -std=c++17 -Wall -Werror

$(TARGET): $(OBJS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
