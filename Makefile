PROJECT = $(notdir $(CURDIR))
COMPILER = arm-none-eabi-

CC		= $(COMPILER)gcc
CP		= $(COMPILER)objcopy
AS		= $(COMPILER)as
LD 		= $(COMPILER)ld
OD		= $(COMPILER)objdump

DEF+=USE_FULL_ASSERT
DEF+=USE_STDPERIPH_DRIVER


PERIPH_DRIVER= Libraries/STM32F4xx_StdPeriph_Driver
INC+= $(PERIPH_DRIVER)/inc
INC+= inc
INC+= spi
INC+= led
INC+= i2c
INC+= usart
INC+= Libraries/CMSIS/Include
INC+= fifo

SRC+= src/system_stm32f4xx.c
SRC+= src/main.c
SRC+= src/printf-stdarg.c
SRC+= $(PERIPH_DRIVER)/src/misc.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_gpio.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_i2c.c
SRC+= src/stm32f4xx_it.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_rcc.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_usart.c
SRC+= led/led.c
SRC+= spi/spi.c
SRC+= i2c/i2c.c
SRC+= fifo/fifo.c
SRC+= usart/usart.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_usart.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_flash.c
SRC+= $(PERIPH_DRIVER)/src/stm32f4xx_spi.c

STARTUP= Libraries/Startup/startup_stm32f40xx.s

LINKER= -TLibraries/ldscripts/stm32f4xx_flash.ld

INCLUDE= $(patsubst %,-I%,$(INC))
DEFINE= $(patsubst %,-D%,$(DEF))

$(shell mkdir -p Output)
$(shell mkdir -p Dependencies)

OUTPUT=Output
DEPDIR = Dependencies
df = $(DEPDIR)/$(*F)

OBJS+=$(patsubst %.c,%.o,$(SRC))
OBJS+=$(patsubst %.s,%.o,$(STARTUP))
OBJECTS = $(addprefix $(OUTPUT)/, $(OBJS))

CFLAGS+=$(INCLUDE)
CFLAGS+=$(DEFINE)
CFLAGS+=-g
CFLAGS+=-mcpu=cortex-m4 -mthumb
CFLAGS+=-MD

LDFLAGS+=$(LINKER)
LDFLAGS+=$(CFLAGS)
LDFLAGS+=-lc
LDFLAGS+=-lrdimon

ASFLAGS+=-g
ASFLAGS+=-mcpu=cortex-m4 -mthumb

$(OUTPUT)/%.o: %.s Makefile
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) -mthumb $< -o $@
	@echo "AS ${@}"
$(OUTPUT)/%.o: %.c Makefile
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "CC ${@}"
	@cp $(OUTPUT)/$*.d $(df).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(OUTPUT)/$*.d >> $(df).P; \
	rm -f $(OUTPUT)/$*.d

-include $(SRCS:%.c=$(DEPDIR)/%.P)

all: $(PROJECT).elf $(PROJECT).bin

$(PROJECT).elf : $(OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $^ $(LDFLAGS) $(LIBS) -o $@
	@echo "CC ${@}"
$(PROJECT).bin : $(PROJECT).elf
	@mkdir -p $(@D)
	$(CP) -O binary $(PROJECT).elf $(PROJECT).bin

	
clean: 
	@rm -f $(OBJECTS) $(PROJECT).bin $(DEPDIR)/*.P $(PROJECT).elf
flash:
	@st-flash write $(PROJECT).bin 0x8000000