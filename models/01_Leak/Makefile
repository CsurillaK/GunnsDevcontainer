# Paths
GUNNS_LIB_DIR := ${GUNNS_HOME}/lib/no_trick
GUNNS_LIB := $(GUNNS_LIB_DIR)/libgunns.a
GUNNS_INC := -I$(GUNNS_HOME) -I$(GUNNS_HOME)/core -I$(GUNNS_HOME)/aspects -I$(GUNNS_HOME)/ms-utils -I$(GUNNS_HOME)/gunns-ts-models
PYTHON_INC := -I/usr/include/python3.6m -DWITHOUT_NUMPY
UTILS_DIR := /workspaces/GunnsDevcontainer/utils

# Sources and target
TARGET := LeakNetwork
SRC := main.cpp gen/$(TARGET).cpp
BUILD_DIR := build
OBJ := $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
TARGET_PATH := $(BUILD_DIR)/$(TARGET)

# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -g -Dno_TRICK_ENV=1 $(GUNNS_INC) -I${UTILS_DIR} ${PYTHON_INC}
LDFLAGS := -L$(GUNNS_LIB_DIR) -lgunns -lpython3.6m

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