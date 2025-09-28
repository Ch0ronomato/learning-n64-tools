V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk

all: rspl1.z64
.PHONY: all

src = main.c
asm = rsp_test.S

rspl1.z64: N64_ROM_TITLE="RSPL 2"

$(BUILD_DIR)/rspl1.elf: $(src:%.c=$(BUILD_DIR)/%.o) $(asm:%.S=$(BUILD_DIR)/%.o)

clean:
	rm -f $(BUILD_DIR)/* *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
