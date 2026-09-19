# Makefile

CC := arm-none-eabi-gcc

MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFS      := -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx
COMMON_FLAGS := $(MCU_FLAGS) $(DEFS) -std=gnu11 -g3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage --specs=nano.specs

# Absolute (via $(abspath)) because CFLAGS is handed to a recursive
# 'make -C Drivers/discostick_common', which changes directory before
# these get used -- relative paths would resolve against the wrong cwd.
PROJECT_INCLUDES := \
	-I$(abspath Core/Inc) \
	-I$(abspath Drivers/STM32F4xx_HAL_Driver/Inc) \
	-I$(abspath Drivers/STM32F4xx_HAL_Driver/Inc/Legacy) \
	-I$(abspath Drivers/CMSIS/Device/ST/STM32F4xx/Include) \
	-I$(abspath Drivers/CMSIS/Include) \
	-I$(abspath USB_DEVICE/App) \
	-I$(abspath USB_DEVICE/Target) \
	-I$(abspath Middlewares/ST/STM32_USB_Device_Library/Core/Inc) \
	-I$(abspath Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)

# discostick_common's own -I flags are owned by its Makefile; a
# top-level-owned file that also needs them (as main.c does) gets them
# here instead, since this Makefile -- not the library -- is the one
# choosing to compile that file.
DISCO_DIR      := Drivers/discostick_common
DISCO_INCLUDES := -I$(DISCO_DIR)/Inc -I$(DISCO_DIR)/3rd_party/littlefs -I$(DISCO_DIR)/3rd_party/nanoprintf

CFLAGS          := $(COMMON_FLAGS) $(PROJECT_INCLUDES)
TOP_OWNED_FLAGS := $(CFLAGS) $(DISCO_INCLUDES)
ASFLAGS         := $(MCU_FLAGS) -DDEBUG -g3 -x assembler-with-cpp --specs=nano.specs

# --- Explicit, curated source lists -----------------------------------------
# Curated the same way as discostick_common's sources.mk-- add a line
# here, not a new rule, when another file joins the build.

CORE_SRCS := \
	Core/Src/adc.c \
	Core/Src/crc.c \
	Core/Src/dma.c \
	Core/Src/gpio.c \
	Core/Src/main.c \
	Core/Src/spi.c \
	Core/Src/stm32f4xx_hal_msp.c \
	Core/Src/stm32f4xx_it.c \
	Core/Src/syscalls.c \
	Core/Src/sysmem.c \
	Core/Src/system_stm32f4xx.c \
	Core/Src/tim.c \
	Core/Src/usart.c

STARTUP_SRCS := \
	Core/Startup/startup_stm32f407vetx.s

HAL_SRCS := \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c

USB_APP_SRCS := \
	USB_DEVICE/App/usb_device.c \
	USB_DEVICE/App/usbd_cdc_if.c \
	USB_DEVICE/App/usbd_desc.c

USB_TARGET_SRCS := \
	USB_DEVICE/Target/usbd_conf.c

MW_CORE_SRCS := \
	Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
	Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
	Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c

MW_CDC_SRCS := \
	Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c

# --- Build layout ------------------------------------------------------------

BUILD_DIR       := build
DISCO_BUILD_DIR := $(abspath $(BUILD_DIR)/discostick_common)

CORE_OBJS       := $(CORE_SRCS:Core/Src/%.c=$(BUILD_DIR)/core/%.o)
STARTUP_OBJS    := $(STARTUP_SRCS:Core/Startup/%.s=$(BUILD_DIR)/startup/%.o)
HAL_OBJS        := $(HAL_SRCS:Drivers/STM32F4xx_HAL_Driver/Src/%.c=$(BUILD_DIR)/hal/%.o)
USB_APP_OBJS    := $(USB_APP_SRCS:USB_DEVICE/App/%.c=$(BUILD_DIR)/usb_app/%.o)
USB_TARGET_OBJS := $(USB_TARGET_SRCS:USB_DEVICE/Target/%.c=$(BUILD_DIR)/usb_target/%.o)
MW_CORE_OBJS    := $(MW_CORE_SRCS:Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.c=$(BUILD_DIR)/mw_core/%.o)
MW_CDC_OBJS     := $(MW_CDC_SRCS:Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.c=$(BUILD_DIR)/mw_cdc/%.o)

ALL_TOP_OBJS := $(CORE_OBJS) $(STARTUP_OBJS) $(HAL_OBJS) $(USB_APP_OBJS) $(USB_TARGET_OBJS) $(MW_CORE_OBJS) $(MW_CDC_OBJS)
ALL_DEPS     := $(filter-out $(STARTUP_OBJS),$(ALL_TOP_OBJS))
ALL_DEPS     := $(ALL_DEPS:.o=.d) $(STARTUP_OBJS:.o=.d)

LDSCRIPT := STM32F407VETX_FLASH.ld
ELF      := $(BUILD_DIR)/discostick_collective.elf
MAP      := $(BUILD_DIR)/discostick_collective.map

.PHONY: all discostick_common list-objects clean
all: $(ELF)

# Always re-enters the library's own Makefile; whether anything actually
# recompiles is entirely up to its own .o/.d timestamps.
discostick_common:
	$(MAKE) -C $(DISCO_DIR) -f Makefile CC=$(CC) CFLAGS="$(CFLAGS)" BUILD_DIR=$(DISCO_BUILD_DIR)

# --- Pattern rules, one per source group ------------------------------------

$(BUILD_DIR)/core/%.o: Core/Src/%.c | $(BUILD_DIR)/core
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/startup/%.o: Core/Startup/%.s | $(BUILD_DIR)/startup
	$(CC) $(ASFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/hal/%.o: Drivers/STM32F4xx_HAL_Driver/Src/%.c | $(BUILD_DIR)/hal
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/usb_app/%.o: USB_DEVICE/App/%.c | $(BUILD_DIR)/usb_app
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/usb_target/%.o: USB_DEVICE/Target/%.c | $(BUILD_DIR)/usb_target
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/mw_core/%.o: Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.c | $(BUILD_DIR)/mw_core
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/mw_cdc/%.o: Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/%.c | $(BUILD_DIR)/mw_cdc
	$(CC) $(TOP_OWNED_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/core $(BUILD_DIR)/startup $(BUILD_DIR)/hal $(BUILD_DIR)/usb_app $(BUILD_DIR)/usb_target $(BUILD_DIR)/mw_core $(BUILD_DIR)/mw_cdc:
	mkdir -p $@

-include $(ALL_DEPS)

# --- Link --------------------------------------------------------------------
$(ELF): discostick_common $(ALL_TOP_OBJS)
	$(CC) -o $@ \
		$(ALL_TOP_OBJS) \
		$(wildcard $(DISCO_BUILD_DIR)/*.o) $(wildcard $(DISCO_BUILD_DIR)/littlefs/*.o) \
		-mcpu=cortex-m4 -T$(LDSCRIPT) --specs=nosys.specs -Wl,-Map=$(MAP) -Wl,--gc-sections \
		-static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb \
		-Wl,--start-group -lc -lm -Wl,--end-group

list-objects:
	@echo "discostick_common objects:"
	@for f in $(wildcard $(DISCO_BUILD_DIR)/*.o) $(wildcard $(DISCO_BUILD_DIR)/littlefs/*.o); do echo "  $$f"; done
	@echo "top-level objects:"
	@for f in $(ALL_TOP_OBJS); do echo "  $$f"; done

clean:
	rm -rf $(BUILD_DIR)
