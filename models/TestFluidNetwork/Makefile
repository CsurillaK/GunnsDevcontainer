# Paths
GUNNS_LIB_DIR := ${GUNNS_HOME}/lib/no_trick
GUNNS_LIB := $(GUNNS_LIB_DIR)/libgunns.a
GUNNS_INC = -I$(GUNNS_HOME) -I$(GUNNS_HOME)/core -I$(GUNNS_HOME)/aspects -I$(GUNNS_HOME)/ms-utils -I$(GUNNS_HOME)/gunns-ts-models

# Sources and target
TARGET := TestFluidNetwork
SRC := main.cpp gen/$(TARGET).cpp
BUILD_DIR := build
OBJ := $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
TARGET_PATH := $(BUILD_DIR)/$(TARGET)

# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -g -Dno_TRICK_ENV=1 $(GUNNS_INC) -Igen
LDFLAGS := -L$(GUNNS_LIB_DIR) -lgunns

all: $(TARGET_PATH)

$(TARGET_PATH): ${OBJ} $(GUNNS_LIB)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GUNNS_LIB):
	$(MAKE) -C $(GUNNS_LIB_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean