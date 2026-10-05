.PHONY: all arm7 arm9 clean
all: arm7 arm9
arm7:
	@$(MAKE) --no-print-directory -C arm7
arm9: arm7
	@$(MAKE) --no-print-directory -C arm9
	@cp arm9/gen4-event-distributor.nds ./gen4-event-distributor.nds
clean:
	@$(MAKE) --no-print-directory -C arm9 clean || true
	@$(MAKE) --no-print-directory -C arm7 clean || true
	@rm -f gen4-event-distributor.nds
