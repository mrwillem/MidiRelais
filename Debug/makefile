################################################################################
# Makefile to generate "elf" compiled binary for embedded targets.
# Author: Asif Iqbal (a.iqbal@ieee.org)
# Date: 7th October, 2010
################################################################################

CPU := cortex-m3
INSTRUCTION_MODE := thumb
TARGET := MidiRelais
TARGET_EXT := elf
LD_SCRIPT := ../stm32.ld

LIBS := ../../STM32F10x_StdPeriph_Driver/Release/libstm32f103.a
LIBS += /opt/stm32arm/arm-none-eabi/lib/thumb/libm.a
LIBS += ../../Embedded_GUI_HAL/Release/libstm32f103.a
LIBS += ../../Embedded_GUI_Library/Release/libstm32f103.a

INCLUDE += -I ../src
INCLUDE += -I ../inc
INCLUDE += -I ../Libraries/CMSIS/CM3/CoreSupport
INCLUDE += -I ../Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/
INCLUDE += -I ../Libraries/Embedded_GUI_HAL/inc
INCLUDE += -I ../Libraries/Embedded_GUI_Library/inc
INCLUDE += -I ../Utilities/STM32_EVAL
# INCLUDE += -I ../Libraries/STM32F10x_StdPeriph_Driver/inc
INCLUDE += -I ../Libraries/
INCLUDE += -I ../../STM32F10x_StdPeriph_Driver
INCLUDE += -I ../../STM32F10x_StdPeriph_Driver/inc
#INCLUDE += -I "C:\Program Files\CodeSourcery\Sourcery G++ Lite\lib\gcc\arm-none-eabi\4.4.1\include"

OBJ_FOLDER := objs

COMPILER_OPTIONS = -O0 -g -ggdb -Wall  -fno-strict-aliasing \
	-fmessage-length=0 -fno-builtin  -m$(INSTRUCTION_MODE) \
	-fdata-sections -ffunction-sections \
	-mcpu=$(CPU) -MMD -MP -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DUSE_STM32F103C8T6_EVAL
	
DEPEND_OPTS = -MF $(OBJ_FOLDER)/$(patsubst %.o,%.d,$(notdir $@)) \
	-MT $(OBJ_FOLDER)/$(patsubst %.o,%.d,$(notdir $@))

CC = arm-none-eabi-gcc
CFLAGS = $(COMPILER_OPTIONS) $(INCLUDE) $(DEPEND_OPTS) -c 

CXX = arm-none-eabi-g++
CXXFLAGS = $(COMPILER_OPTIONS) $(INCLUDE) $(DEPEND_OPTS) -c
AS = arm-none-eabi-gcc
ASFLAGS = $(COMPILER_OPTIONS) $(INCLUDE) $(DEPEND_OPTS)  -c

LD = arm-none-eabi-gcc
LD_OPTIONS = -lm -Wl,-Map=$(TARGET).map $(COMPILER_OPTIONS) -Wl,--gc-sections -L ../ -T $(LD_SCRIPT) $(INCLUDE) 

OBJCP = arm-none-eabi-objcopy
OBJCPFLAGS = -O ihex

AR = arm-none-eabi-ar
ARFLAGS = cr

RM := rm -rf

USER_OBJS :=
C_SRCS := 
S_SRCS := 
C_OBJS :=
S_OBJS :=

# Every subdirectory with source files must be described here
SUBDIRS := ../src
SUBDIRS += ../Libraries/CMSIS/CM3/CoreSupport
#SUBDIRS += ../Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
#SUBDIRS += ../Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/TrueSTUDIO
#SUBDIRS += ../Libraries/Embedded_GUI_HAL/src
#SUBDIRS += ../Libraries/Embedded_GUI_Library/src
#SUBDIRS += ../Libraries/STM32F10x_StdPeriph_Driver/src
SUBDIRS += ../Utilities/STM32_EVAL
# SUBDIRS += ../Utilities/STM32_EVAL/Common /included above automagically
#SUBDIRS += ../Utilities/STM32_EVAL/STM3210B_EVAL /included above automagically


C_SRCS := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
C_OBJS := $(patsubst %.c,$(OBJ_FOLDER)/%.o,$(notdir $(C_SRCS)))
S_SRCS := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.S))
S_OBJS := $(patsubst %.S,$(OBJ_FOLDER)/%.o,$(notdir $(S_SRCS)))

VPATH := $(SUBDIRS)

$(OBJ_FOLDER)/%.o : %.c
	@echo 'Building file: $(@F)'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CFLAGS) $< -o $@
	@echo 'Finished building: $(@F)'
	@echo ' '
	
$(OBJ_FOLDER)/%.o : %.S
	@echo 'Building file: $(@F)'
	@echo 'Invoking: MCU Assembler'
	$(AS) $(ASFLAGS) $< -o $@
	@echo 'Finished building: $(@F)'
	@echo ' '

# All Target
all: $(TARGET).$(TARGET_EXT)

# Tool invocations
$(TARGET).$(TARGET_EXT): $(C_OBJS) $(S_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	$(LD) $(LD_OPTIONS) $(C_OBJS) $(S_OBJS) $(USER_OBJS) $(LIBS) -o$(TARGET).$(TARGET_EXT)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(TARGET).$(TARGET_EXT) $(TARGET).bin $(TARGET).map $(OBJ_FOLDER)/*.*
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	arm-none-eabi-size --totals $(TARGET).$(TARGET_EXT) 
	arm-none-eabi-objcopy -O binary $(TARGET).$(TARGET_EXT) $(TARGET).bin 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

