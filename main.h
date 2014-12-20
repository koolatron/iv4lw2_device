#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Descriptors.h"

#include "include/time.h"
#include "include/shift.h"
#include "include/iv4.h"

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Buttons.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#define CMD_BUF_SIZE                32

#define FLASH_SIZE_BYTES            0x8000
#define BOOTLOADER_SEC_SIZE_BYTES   0x1000
#define BOOTLOADER_START_ADDRESS    (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)
#define MAGIC_BOOT_KEY              0xCAFEBABE

#define LEDMASK_USB_ENUMERATING     LEDS_LED1
#define LEDMASK_USB_NOTREADY        LEDS_NO_LEDS
#define LEDMASK_USB_ERROR           LEDS_NO_LEDS
#define LEDMASK_USB_READY           LEDS_LED1

#define CMD_NOTREADY                0x00
#define CMD_READY                   0x01
#define CMD_COMPLETE                0x02
#define CMD_ERROR                   0x04

typedef struct {
    uint8_t data[CMD_BUF_SIZE];
    uint8_t status;
    size_t  len;
} cbuffer_t;

/** The ATTR_INIT_SECTION(3) decoration causes gcc to place this function in the .init3 section,
 *  which runs before main but after the stack is initialized.
 */
void Bootloader_Jump_Check(void) ATTR_INIT_SECTION(3);
void Jump_To_Bootloader(void);

void ProcessCommand(void);
void ProcessInput(void);
void ProcessDisplay(void);
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
