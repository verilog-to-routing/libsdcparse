NUM_PROC=$(shell nproc)

SOURCE_DIR := $(PWD)
BUILD_DIR ?= build

.PHONY: all clean $(MAKECMDGOALS)

${BUILD_DIR}/Makefile:
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR} && cmake ${SOURCE_DIR}

all $(MAKECMDGOALS): $(BUILD_DIR)/Makefile
	$(MAKE) -k -j${NUM_PROC} -C $(BUILD_DIR) $(MAKECMDGOALS) \
		|| $(MAKE) -j1 -C $(BUILD_DIR) $(MAKECMDGOALS)
