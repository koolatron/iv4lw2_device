#include "main.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
    {
        .Config =
            {
                .ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
                .DataINEndpoint           =
                    {
                        .Address          = CDC_TX_EPADDR,
                        .Size             = CDC_TXRX_EPSIZE,
                        .Banks            = 1,
                    },
                .DataOUTEndpoint =
                    {
                        .Address          = CDC_RX_EPADDR,
                        .Size             = CDC_TXRX_EPSIZE,
                        .Banks            = 1,
                    },
                .NotificationEndpoint =
                    {
                        .Address          = CDC_NOTIFICATION_EPADDR,
                        .Size             = CDC_NOTIFICATION_EPSIZE,
                        .Banks            = 1,
                    },
            },
    };

/** Jump to bootloader check
 *  Using the ATTR_INIT_SECTION(3) decoration causes gcc to place this function call in the init section,
 *  which runs before main
 */
void Bootloader_Jump_Check(void) ATTR_INIT_SECTION(3);

int main(void) {
    SetupHardware();

    GlobalInterruptEnable();
    
    for (;;) {
        CDC_Task();
        USB_USBTask();
    };
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    LEDs_Init();
    USB_Init();
}

/** Function to manage CDC data transmission and reception to and from the host for the CDC interface, which echoes back
 *  all data sent to it from the host.
 */
void CDC_Task(void)
{
    /* Device must be connected and configured for the task to run */
    if (USB_DeviceState != DEVICE_STATE_Configured)
      return;

    /* Select the Serial Rx Endpoint */
    Endpoint_SelectEndpoint(CDC_RX_EPADDR);

    /* Check to see if any data has been received */
    if (Endpoint_IsOUTReceived())
    {
        /* Create a temp buffer big enough to hold the incoming endpoint packet */
        uint8_t  Buffer[Endpoint_BytesInEndpoint()];

        /* Remember how large the incoming packet is */
        uint16_t DataLength = Endpoint_BytesInEndpoint();

        /* Read in the incoming packet into the buffer */
        Endpoint_Read_Stream_LE(&Buffer, DataLength, NULL);

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();

        if ( Buffer[0] == 'a' ) {
            Buffer[0] = 'z';
        }

        /* Select the Serial Tx Endpoint */
        Endpoint_SelectEndpoint(CDC_TX_EPADDR);

        /* Write the received data to the endpoint */
        Endpoint_Write_Stream_LE(&Buffer, DataLength, NULL);

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearIN();

        /* Wait until the endpoint is ready for the next packet */
        Endpoint_WaitUntilReady();

        /* Send an empty packet to prevent host buffering */
        Endpoint_ClearIN();
    }
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

    LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}


/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

void Bootloader_Jump_Check(void)
{
    // If the reset source was the bootloader and the key is correct, clear it and jump to the bootloader
    if ((MCUSR & (1 << WDRF)) && (Boot_Key == MAGIC_BOOT_KEY))
    {
        Boot_Key = 0;
        ((void (*)(void))BOOTLOADER_START_ADDRESS)();
    }
}

void Jump_To_Bootloader(void)
{
    // If USB is used, detach from the bus and reset it
    USB_Disable();
    
    // Disable all interrupts
    cli();
    
    // Wait two seconds for the USB detachment to register on the host
    Delay_MS(2000);
    
    // Set the bootloader key to the magic value and force a reset
    Boot_Key = MAGIC_BOOT_KEY;
    wdt_enable(WDTO_250MS);
    for (;;);
}
