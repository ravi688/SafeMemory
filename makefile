TARGET_STATIC_LIB = ./lib/safemem.a
TARGET_STATIC_LIB_DIR = ./lib
TARGET = main

#Dependencies
DEPENDENCY_LIBS = ./dependencies/BufferLib/lib/bufferlib.a ./shared-dependencies/CallTrace/lib/calltrace.a
DEPENDENCY_INCLUDES = ./dependencies/ ./dependencies/BufferLib/include ./dependencies/TemplateSystem/include  ./shared-dependencies/CallTrace/include

INCLUDES= -I.\include -I.\include\safe_memory $(addprefix -I, $(DEPENDENCY_INCLUDES))
SOURCES= $(wildcard source/*.c)
OBJECTS= $(addsuffix .o, $(basename $(SOURCES)))
LIBS = 

#Flags and Defines
DEBUG_DEFINES =  -DGLOBAL_DEBUG -DDEBUG -DLOG_DEBUG
RELEASE_DEFINES =  -DGLOBAL_RELEASE -DRELEASE -DLOG_RELEASE
DEFINES = 

COMPILER_FLAGS= -m64 -g
COMPILER = gcc
ARCHIVER_FLAGS = -rc
ARCHIVER = ar


.PHONY: lib-static
.PHONY: lib-static-debug
.PHONY: lib-static-release
.PHONY: release
.PHONY: debug
.PHONY: $(TARGET)	
.PHONY: clean

all: release
lib-static: lib-static-release
lib-static-debug: DEFINES += $(DEBUG_DEFINES)
lib-static-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-static-debug: $(TARGET_STATIC_LIB)
lib-static-release: DEFINES += $(RELEASE_DEFINES)
lib-static-release: __STATIC_LIB_COMMAND = lib-static-release
lib-static-release: $(TARGET_STATIC_LIB)
release: DEFINES += $(RELEASE_DEFINES)
release: __STATIC_LIB_COMMAND = lib-static-release
release: $(TARGET)
debug: DEFINES += $(DEBUG_DEFINES)
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: $(TARGET)


%.o : %.c
	$(COMPILER) $(COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(__STATIC_LIB_COMMAND)
	@echo [Log] $@ built successfully!

$(TARGET_STATIC_LIB_DIR): 
	mkdir $@

PRINT_MESSAGE: 
	@echo [Log] Building $(TARGET_STATIC_LIB) ...

$(TARGET_STATIC_LIB) : PRINT_MESSAGE $(filter-out source/main.o, $(OBJECTS)) | $(TARGET_STATIC_LIB_DIR) 
	$(ARCHIVER) $(ARCHIVER_FLAGS) $@ $(filter-out $<, $^)
	@echo [Log] $@ built successfully!


$(TARGET): $(DEPENDENCY_LIBS) $(TARGET_STATIC_LIB) source/main.o
	$(COMPILER) $(COMPILER_FLAGS) source/main.o $(LIBS) \
	$(addprefix -L, $(dir $(TARGET_STATIC_LIB) $(DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(TARGET_STATIC_LIB) $(DEPENDENCY_LIBS))) \
	-o $@

clean: 
	del $(addprefix source\, $(notdir $(OBJECTS)))
	del main.exe
	del $(subst /,\, $(TARGET_STATIC_LIB))
	rmdir $(subst /,\, $(TARGET_STATIC_LIB_DIR))
	$(MAKE) --directory=./dependencies/BufferLib clean
# 	$(MAKE) --directory=./dependencies/tgc clean