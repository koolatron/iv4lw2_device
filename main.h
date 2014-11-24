#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Descriptors.h"

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
//#include <LUFA/Common/Common.h>

#define FLASH_SIZE_BYTES            0x8000
#define BOOTLOADER_SEC_SIZE_BYTES   0x1000  
#define BOOTLOADER_START_ADDRESS    (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)
#define MAGIC_BOOT_KEY              0xCAFEBABE

#define LEDMASK_USB_ENUMERATING     LEDS_LED1
#define LEDMASK_USB_NOTREADY        LEDS_NO_LEDS
#define LEDMASK_USB_ERROR           LEDS_NO_LEDS
#define LEDMASK_USB_READY           LEDS_LED1

uint32_t Boot_Key ATTR_NO_INIT;

void Bootloader_Jump_Check(void);
void Jump_To_Bootloader(void);

void ProcessInput(void);
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
