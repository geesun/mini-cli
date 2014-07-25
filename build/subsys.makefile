include $(TOOL_ROOT)/$(TARGET).mk
OBJ_DIR=$(ROOT)/work/$(TARGET)/#$(OS)/$(ARCH)


AT=@
RELEASE_DIR=$(ROOT)/release/$(TARGET)


STATIC_OBJ_DIR=$(OBJ_DIR)/static
SHARE_OBJ_DIR=$(OBJ_DIR)/share
PROG_OBJ_DIR=$(OBJ_DIR)/prog
KERNEL_OBJ_DIR=$(OBJ_DIR)/kernel

STATIC_LIB = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_LIB))
SHARE_LIB = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_LIB))
PROG = $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG))
KERNEL_LIB = $(addsuffix .o, $(TARGET_KERNEL))

obj-m:=$(KERNEL_LIB)
KERNEL_OBJS=$(addprefix ../../../,$(TARGET_KERNEL_SRCS:.c=.o))
KERNEL_GEN_OBJS=$(addprefix $(ROOT)/,$(TARGET_KERNEL_SRCS:.c=.o))
KERNEL_GEN_CMDS=$(foreach m, $(addprefix $(ROOT)/,$(TARGET_KERNEL_SRCS:.c=.o.cmd)), $(dir $(m))/.$(notdir $(m)))


KERNEL_INC=$(addprefix "-I../../../",$(TARGET_KERNEL_INC))


STATIC_LIB_OBJS = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_SRCS:.c=.o))
STATIC_LIB_DEPS = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_SRCS:.c=.d))

SHARE_LIB_OBJS = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_SRCS:.c=.o))
SHARE_LIB_DEPS = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_SRCS:.c=.d))

PROG_OBJS = $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_SRCS:.c=.o))
PROG_DEPS = $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_SRCS:.c=.d))


TARGET_STATIC_CFLAGS += $(CFLAGS)

TARGET_SHARE_CFLAGS += $(CFLAGS)
TARGET_SHARE_LD_FLAGS += $(LD_FLAGS)

TARGET_PROG_CFLAGS += $(CFLAGS)
TARGET_PROG_LD_FLAGS += $(LD_FLAGS)

STATIC_ALL=$(STATIC_LIB) $(STATIC_LIB_OBJS) $(STATIC_LIB_DEPS)
SHARE_ALL=$(SHARE_LIB) $(SHARE_LIB_OBJS) $(SHARE_LIB_DEPS)
PROG_ALL=$(PROG) $(PROG_OBJS) $(PROG_DEPS)

define create_kernel_module
	test="\$$(src)" ; \
	inc="EXTRA_CFLAGS=$(TARGET_KERNEL_CFLAGS)" ; \
	for d in $(TARGET_KERNEL_INC) ; do \
		item="-I$$test/../../../$$d"; \
		inc="$$inc $$item" ; \
	done ; \
	mkdir $(dir $(1)) -p ;\
	echo "obj-m:=$(KERNEL_LIB)" >$(1); \
	echo "$(TARGET_KERNEL)-objs=$(KERNEL_OBJS)" >>$(1); \
	echo $$inc >>$(1) ;\
	echo "all:" >>$(1) ;\
	echo "	make -C $(KERNEL_BUILD) M=$(2) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) " >>$(1) ;\
	echo "clean:" >>$(1) ; \
	echo "	make -C $(KERNEL_BUILD) M=$(2) clean" >>$(1) ; 
endef


all:$(STATIC_ALL) $(SHARE_ALL) $(PROG_ALL) 
ifneq ($(TARGET_KERNEL),)
	$(call create_kernel_module, $(KERNEL_OBJ_DIR)/Makefile,$(KERNEL_OBJ_DIR) )
	$(AT) make -f $(KERNEL_OBJ_DIR)/Makefile
endif

$(STATIC_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_STATIC_CFLAGS) -c -o $@ $<

$(STATIC_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_STATIC_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(STATIC_LIB):$(STATIC_LIB_OBJS)
	$(AT) echo [Generate  $@]
	$(AT) $(AR)  rcs  $@ $(STATIC_LIB_OBJS)
	$(AT) echo 
	$(AT) echo 

$(SHARE_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_SHARE_CFLAGS) -c -o $@ $<

$(SHARE_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_SHARE_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(SHARE_LIB):$(SHARE_LIB_OBJS)
	$(AT) echo "Generate $@"
	$(AT) $(CC) -shared -Wl,-soname,$@ -o $@  -o $@ $(SHARE_LIB_OBJS) $(TARGET_SHARE_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 

$(PROG_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_PROG_CFLAGS) -c -o $@ $<

$(PROG_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_PROG_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(PROG):$(PROG_OBJS)
	$(AT) echo [Linking  $@]
	$(AT) $(CC)   -o $@ $(PROG_OBJS) $(TARGET_PROG_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 



-include $(STATIC_LIB_DEPS) $(SHARE_LIB_DEPS) $(PROG_DEPS)  


clean:
	@ rm  -rf $(STATIC_OBJ_DIR)  $(SHARE_OBJ_DIR) $(PROG_OBJ_DIR) $(KERNEL_GEN_OBJS) $(KERNEL_GEN_CMDS) $(RELEASE_DIR)
ifneq ($(TARGET_KERNEL),)
	$(call create_kernel_module, $(KERNEL_OBJ_DIR)/Makefile,$(KERNEL_OBJ_DIR) )
	$(AT) make -f $(KERNEL_OBJ_DIR)/Makefile clean
endif

RELEASE_SRC_DIRS=$(foreach src, $(RELEASE_SOURCES), $(RELEASE_DIR)/$(dir $(src)))

RELEASE_SRC_CMD=$(foreach src, $(RELEASE_SOURCES), "cp $(src) $(RELEASE_DIR)/$(dir $(src))" )

define release_src
	for srcs in $(RELEASE_SOURCES) ; do \
		src=`echo $$srcs|awk -F':' '{ print $$1 }' `; \
		dst=`echo $$srcs|awk -F':' '{ print $$2 }' `; \
		reg=`echo "$$src" |awk '/\*/ { print $$1 }'`; \
		if [ x$$reg == x ] &&  [ -d $$src ] ; then \
	    	mkdir -p $(RELEASE_DIR)/src/$$dst ; \
			cp -a $$src/* $(RELEASE_DIR)/src/$$dst/ ; \
		else \
			if [ x$$reg != x ] ; then   \
				mkdir -p $(RELEASE_DIR)/src/$$dst; \
			else \
	    		dst_dir=`dirname $$dst` ; \
	    		mkdir -p $(RELEASE_DIR)/src/$$dst_dir ; \
			fi; \
			cp $$src $(RELEASE_DIR)/src/$$dst; \
		fi ;\
	done ; \
	find $(RELEASE_DIR)/src/ -name CVS |xargs rm -rf ; \
	mkdir -p $(RELEASE_DIR); \
	for file in $(STATIC_LIB) $(SHARE_LIB) ; do \
		mkdir -p $(RELEASE_DIR)/libs/ ; \
		cp $$file $(RELEASE_DIR)/libs/ ; \
	done ; \
	if [ x$(PROG) != x ] ; then  \
		mkdir -p $(RELEASE_DIR)/bin ; \
		cp $(PROG) $(RELEASE_DIR)/bin ; \
	fi; \
	if [ -f $(KERNEL_OBJ_DIR)/$(addsuffix .ko, $(TARGET_KERNEL)) ] ; then  \
		mkdir -p $(RELEASE_DIR)/bin ; \
		cp $(KERNEL_OBJ_DIR)/$(addsuffix .ko, $(TARGET_KERNEL))  $(RELEASE_DIR)/bin ; \
	fi;
endef

release:all
	$(call release_src)


