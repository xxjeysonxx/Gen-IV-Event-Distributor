.SUFFIXES:
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment")
endif
include $(DEVKITARM)/ds_rules
TARGET:=gen4-event-distributor-arm7
BUILD:=build
SOURCES:=source
INCLUDES:=include
ARCH:=-march=armv4t -mtune=arm7tdmi -mthumb
CFLAGS:=-g -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -DARM7
ASFLAGS:=-g $(ARCH)
LDFLAGS:=-specs=ds_arm7.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS:=-lnds7
LIBDIRS:=$(LIBNDS)
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
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).elf
clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).map $(TARGET).lst
else
DEPENDS:=$(OFILES:.o=.d)
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
