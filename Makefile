#
# Makefile for msp430
#
# 'make' builds everything
# 'make clean' deletes everything except source files and Makefile
# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced 
# $(TARGET).elf $(TARGET).hex and $(TARGET).txt nad $(TARGET).map are all generated.
# The TXT file is used for BSL loading, the ELF can be used for JTAG use
# 
TARGET     = main
MCU        = msp430g2230
# List all the source files here
# eg if you have a source file foo.c then list it here
TASKS = tasks/
SOURCES = main.c
SOURCES += $(TASKS)button.c
LIB-FOLDER = ../msp-lib/c/
OTHERS = $(LIB-FOLDER)clock.c $(LIB-FOLDER)timerA0.c $(LIB-FOLDER)opMode.c $(LIB-FOLDER)launchpad.c \
	$(LIB-FOLDER)queue.c $(LIB-FOLDER)scheduler.c $(LIB-FOLDER)task.c 
# Include are located in the Include directory
INCLUDES = -I../msp-lib/inc -Iinc/

# Add or subtract whatever MSPGCC flags you want. There are plenty more
#######################################################################################
CFLAGS   = -mmcu=$(MCU) -g -Os -Wall -Wunused -Wimplicit-function-declaration $(INCLUDES)
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS  = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map
########################################################################################
CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
MAKETXT  = srec_cat
CP       = cp -p
RM       = rm -f
MV       = mv
########################################################################################

OBJECTS = $(SOURCES:.c=.o)
OBJECTS += $(OTHERS:.c=.o)

all:  $(TARGET).elf
$(TARGET).elf: $(OBJECTS)
	echo "Linking $@"
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	echo
	echo ">>>> Size of Firmware <<<<"
	$(SIZE) $(TARGET).elf
	echo
%.o: %.c
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) -o $@ $<
flash:
	mspdebug rf2500 "prog $(TARGET).elf"
run: $(TARGET).elf flash
clean: 
	$(RM) $(TARGET).o $(TARGET).elf $(TARGET).map
	$(RM) $(LIB-FOLDER)*.o $(TASKS)*.o
