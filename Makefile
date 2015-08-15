# Build-in-place strategy, not using recursive make 

RM = rm -f

GNUARM = /usr/local/gcc-arm-none-eabi/bin
C_COMPILER = $(GNUARM)/arm-none-eabi-gcc
CPP_COMPILER = $(GNUARM)/arm-none-eabi-g++
OBJ_COPY = $(GNUARM)/arm-none-eabi-objcopy
OBJ_DUMP = $(GNUARM)/arm-none-eabi-objdump
ELF_SIZE = $(GNUARM)/arm-none-eabi-size

OBJ = .o
DEP = .d

DISPLAY_TYPE=DISPLAY_TYPE_ITDB02_43

INC_DIRS = \
	-Isrc/config \
	-Isrc/ASF/thirdparty/CMSIS/Lib/GCC \
	-Isrc/ASF/sam/utils/cmsis/sam3s/include \
	-Isrc/ASF/common/utils \
	-Isrc \
	-Isrc/ASF/sam/utils/cmsis/sam3s/source/templates \
	-Isrc/ASF/sam/utils \
	-Isrc/ASF/sam/utils/preprocessor \
	-Isrc/ASF/common/boards \
	-Isrc/ASF/sam/utils/header_files \
	-Isrc/ASF/common/boards/user_board \
	-Isrc/ASF/thirdparty/CMSIS/Include \
	-Isrc/ASF/sam/drivers/pio \
	-Isrc/ASF/sam/drivers/pmc \
	-Isrc/ASF/common/services/clock \
	-Isrc/ASF/common/services/delay \
	-Isrc/ASF/sam/drivers/wdt \
	-Isrc/ASF/sam/drivers/pwm \
	-Isrc/ASF/sam/drivers/uart \
	-Isrc/ASF/sam/drivers/matrix \
	-Isrc/ASF/sam/drivers/efc \
	-Isrc/ASF/sam/services/flash_efc \
	-Isrc/ASF/sam/drivers/rstc 

# Add inputs and outputs from these tool invocations to the build variables
SRCS = \
	src/ASF/sam/drivers/efc/efc.c \
	src/ASF/sam/drivers/matrix/matrix.c \
	src/ASF/sam/drivers/pwm/pwm.c \
	src/ASF/sam/drivers/rstc/rstc.c \
	src/ASF/sam/services/flash_efc/flash_efc.c \
	src/Buzzer.cpp \
	src/Fields.cpp \
	src/FlashStorage.cpp \
	src/glcd19x21.cpp \
	src/glcd28x32.cpp \
	src/mem.cpp \
	src/Misc.cpp \
	src/SysTick.cpp \
	src/ASF/sam/drivers/uart/uart.c \
	src/Display.cpp \
	src/OneBitPort.cpp \
	src/PanelDue.cpp \
	src/Print.cpp \
	src/SerialIo.cpp \
	src/UTFT.cpp \
	src/UTouch.cpp \
	src/ASF/common/services/clock/sam3s/sysclk.c \
	src/ASF/common/services/delay/sam/cycle_counter.c \
	src/ASF/sam/drivers/pio/pio.c \
	src/ASF/sam/drivers/pio/pio_handler.c \
	src/ASF/sam/drivers/pmc/pmc.c \
	src/ASF/sam/drivers/pmc/sleep.c \
	src/ASF/sam/drivers/wdt/wdt.c \
	src/ASF/common/boards/user_board/init.c \
	src/ASF/common/utils/interrupt/interrupt_sam_nvic.c \
	src/ASF/sam/utils/cmsis/sam3s/source/templates/exceptions.c \
	src/ASF/sam/utils/cmsis/sam3s/source/templates/gcc/startup_sam3s.c \
	src/ASF/sam/utils/cmsis/sam3s/source/templates/system_sam3s.c \
	src/ASF/sam/utils/syscalls/gcc/syscalls.c

OBJS = $(patsubst %.cpp,%$(OBJ),$(SRCS:.c=$(OBJ)))
DEPS = $(patsubst %.cpp,%$(DEP),$(SRCS:.c=$(DEP)))
LIBS = -lm

OUTPUT_FILE_PATH = PanelDue.elf

#OUTPUT_FILE_DEP = ./makedep.mk

OPTIMIZE = -O2 --param max-inline-insns-single=500 -ffunction-sections
OPTIMIZE_CC = -fdata-sections -fno-strict-aliasing
OPTIMIZE_CPP = -fno-rtti -fno-exceptions

C_DEFS = -D__SAM3S2B__ -DNDEBUG -DBOARD=USER_BOARD -DARM_MATH_CM3=true \
	"-DDISPLAY_TYPE=$(DISPLAY_TYPE)"

C_OPTS = -x c -pipe

CPP_OPTS =
CPP_DEFS = $(C_DEFS)

MCPU = cortex-m3
ARM_OPTS = -mthumb -mlong-calls -mcpu=$(MCPU)

LINKER_SCRIPT_DEP = \
	src/ASF/sam/utils/linker_scripts/sam3s/sam3s2/gcc/flash.ld

%.o: %.c
	$(C_COMPILER) $(C_OPTS) $(ARM_OPTS) $(C_DEFS) $(INC_DIRS) $(OPTIMIZE) \
		$(OPTIMIZE_CC) -Wall -std=gnu99 \
		-MD -MP -MF $(@:%.o=%.d) -MT$(@:%.o=%.d) -MT$(@:%.o=%.o) \
		-c -o $@ $<

%.o: %.cpp
	$(CPP_COMPILER) $(CPP_OPTS) $(ARM_OPTS) $(CPP_DEFS) $(INC_DIRS) \
		$(OPTIMIZE) $(OPTIMIZE_CPP) -Wall \
		-MD -MP -MF $(@:%.o=%.d) -MT$(@:%.o=%.d) -MT$(@:%.o=%.o) \
		-c -o $@ $<

#ifneq ($(MAKECMDGOALS),clean)
#ifneq ($(strip $(DEPS)),)
#-include $(DEPS)
#endif
#endif

# Add inputs and outputs from these tool invocations to the build variables

# All Target
all: $(OUTPUT_FILE_PATH)

$(OUTPUT_FILE_PATH): $(OBJS) $(OUTPUT_FILE_DEP) $(LINKER_SCRIPT_DEP)
	$(CPP_COMPILER) -o$(OUTPUT_FILE_PATH) $(OBJS) $(LIBS) $(ARM_OPTS) \
		-static -Wl,-Map=PanelDue.map --specs=nano.specs \
		-Wl,--start-group -larm_cortexM3l_math $(LIBS) \
		-Wl,--end-group -Lsrc/ASF/thirdparty/CMSIS/Lib/GCC  \
		-Wl,--gc-sections \
		-Tsrc/ASF/sam/utils/linker_scripts/sam3s/sam3s2/gcc/flash.ld
	$(OBJ_COPY) -O binary PanelDue.elf PanelDue.bin
	$(OBJ_COPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature \
		PanelDue.elf PanelDue.hex
	$(OBJ_COPY) -j .eeprom --set-section-flags=.eeprom=alloc,load \
		--change-section-lma .eeprom=0 --no-change-warnings \
		-O binary PanelDue.elf PanelDue.eep || exit 0
	$(OBJ_DUMP) -h -S PanelDue.elf > PanelDue.lss
	$(OBJ_COPY) -O srec -R .eeprom -R .fuse -R .lock -R .signature \
		 PanelDue.elf PanelDue.srec
	$(ELF_SIZE) PanelDue.elf

# Other Targets
clean:
	-$(RM) $(OBJS)
	-$(RM) $(DEPS)
	-$(RM) PanelDue.elf PanelDue.a PanelDue.hex PanelDue.bin \
		PanelDue.lss PanelDue.eep PanelDue.map PanelDue.srec
