ROOT=$(shell pwd)
TOOL_ROOT=$(ROOT)/build

TARGET_FILES=$(filter-out $(TOOL_ROOT)/cmn.mk, $(shell ls $(TOOL_ROOT)/*.mk))

TARGETS=$(notdir $(basename $(TARGET_FILES)))

CLEAN_TARGETS=$(addsuffix .clean, $(TARGETS))
RELEASE_TARGETS=$(addsuffix .release, $(TARGETS))
AT=@

OPTS=ROOT=$(ROOT) TOOL_ROOT=$(TOOL_ROOT) 
all:$(TARGETS)

$(TARGETS): 
	$(AT)printf  "\t\t   +---------------------------+\n"
	$(AT)printf  "\t\t  /         Geesun             /|\n"
	$(AT)printf  "\t\t /      Mini-cli build        / |\n"
	$(AT)printf  "\t\t/----------------------------/  |\n"
	$(AT)printf  "\t\t|                           |   |\n"
	$(AT)printf  "\t\t|      Building...          |  /\n"
	$(AT)printf  "\t\t|     %-22s| /\n" $@
	$(AT)printf  "\t\t|                           |/\n"
	$(AT)printf  "\t\t+---------------------------+\n"
	$(AT) make -s -f $(TOOL_ROOT)/subsys.makefile $(OPTS) TARGET=$@
    
$(CLEAN_TARGETS):
	$(AT) make -s -f $(TOOL_ROOT)/subsys.makefile $(OPTS) TARGET=$(basename $@) clean

    
$(RELEASE_TARGETS):
	$(AT) make -f $(TOOL_ROOT)/subsys.makefile $(OPTS) TARGET=$(basename $@) release

clean:$(CLEAN_TARGETS)
release:$(RELEASE_TARGETS)
    
