.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment")
endif
# Nintendo DS menu banner metadata
GAME_TITLE := Gen4 Event Distributor
GAME_SUBTITLE1 := Gen IV Events Alpha
GAME_SUBTITLE2 := SooraMaru
GAME_ICON := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../icon.bmp)

include $(DEVKITARM)/ds_rules
TARGET:=gen4-event-distributor
BUILD:=build
SOURCES:=source
INCLUDES:=include
ARM7_ELF := ../../arm7/gen4-event-distributor-arm7.elf
ARCH:=-march=armv5te -mtune=arm946e-s -mthumb
CFLAGS:=-g -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS:=$(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS:=-g $(ARCH)
LDFLAGS:=-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS:=-lnds9
LIBDIRS:=$(LIBNDS) $(PORTLIBS)
_ARM7_ELF:=-7 $(ARM7_ELF)
ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT:=$(CURDIR)/$(TARGET)
export VPATH:=$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR:=$(CURDIR)/$(BUILD)
CFILES:=$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export LD:=$(CC)
export OFILES:=$(CFILES:.c=.o)
export INCLUDE:=$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) $(foreach dir,$(LIBDIRS),-I$(dir)/include) -I$(CURDIR)/$(BUILD)
export LIBPATHS:=$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
.PHONY: all $(BUILD) clean
all: $(BUILD)
$(BUILD): $(ARM7_ELF)
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
$(ARM7_ELF):
	@$(MAKE) --no-print-directory -C ../arm7
clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).map $(TARGET).lst
else
DEPENDS:=$(OFILES:.o=.d)
$(OUTPUT).nds: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
