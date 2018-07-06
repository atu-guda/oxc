OXCDIR = /usr/share/stm32oxc

COMMONPROJDIR = $(shell basename `pwd` )
include ../../common/$(COMMONPROJDIR)/proj.mk
-include local.mk

# aux: (hal, cortex, gpio, rcc added by bsp makefile part )
# see ../common/fX_modules.mk

include ../common/board_add.mk
include $(OXCDIR)/mk/common_cortex.mk
# ../../../mk/common_cortex.mk

include $(wildcard $(DEPSDIR)/*.d)

Makefile: ../../common/$(COMMONPROJDIR)/proj.mk

#

