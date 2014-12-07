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

uint32_t Boot_Key ATTR_NO_INIT;
static FILE USBSerialStream;
static buffer cmdBuffer;
uint8_t bitMap[4][3];
uint8_t charMap[4];
uint8_t timeMap[4];

int main(void) {
    SetupHardware();

    /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

    GlobalInterruptEnable();

    /* Put a test pattern up */
    SHRBlank();
    bufferChar(bitMap[0], 'A');
    selectGrid(bitMap[0], 2);
    SHRSendBuffer(bitMap[0]);
    SHRLatch();
    SHRUnblank();

    for (;;) {
        ProcessInput();
        CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
        USB_USBTask();
    };
}

/** Configures the board hardware and chip peripherals. */
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
    initSHR();

    /* Initialize timer0 */
    /* This sets up a timer interrupt at 250Hz to signal display service */
    OCR0A   = 249;                           // Top count
    TCCR0A |= (1 << WGM01);                  // CTC mode
    TIMSK0 |= (1 << OCIE0A);                 // Enable interrupt generation on OCR0A match
    TCCR0B |= (1 << CS02);                   // F/256 prescaler; start timer

    /* Initialize timer1 */
    /* This sets up a 10kHz signal on OCR1B, PORTC5, to drive our filament */
    DDRC   |= (1 << 5);                      // Enable OCR1B as output pin
    OCR1A   = 799;                           // Top count
    TCCR1A |= (1 << COM1B0);                 // Toggle OCR1B on compare match
    TCCR1B |= (1 << WGM12) | (1 << CS10);    // CTC mode, F/1 prescaler; start timer
}

/** Function to deal with incoming serial bytes on CDC interface and place them in our command buffer */
void ProcessInput(void)
{
    // Grab the next byte from the virutal serial interface
    int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

    if (!(ReceivedByte < 0 )) {
        // Echo the received byte on the virtual serial interface
        fputc(ReceivedByte, &USBSerialStream);

        // Jump to bootloader if the byte was 'X'
        if ((uint8_t)ReceivedByte == 'X') {
            Jump_To_Bootloader();
        }

        // Do some basic parsing for the command buffer
        // XXX: this needs some more thought.
        if ((uint8_t)ReceivedByte == '%') {
            // Command start token
            cmdBuffer.data[0] = '%';
            cmdBuffer.data[1] = '\0';
            cmdBuffer.len = 1;
        } else {
            if ((cmdBuffer.len > 0) && (cmdBuffer.status == CMD_NOTREADY)) {
                if ((uint8_t)ReceivedByte == 0x0d) {
                    // Command end token
                    cmdBuffer.status = CMD_READY;
                } else {
                    if (cmdBuffer.len < (CMD_BUF_SIZE - 1)) {
                        // Append bytes
                        cmdBuffer.data[cmdBuffer.len++] = (uint8_t)ReceivedByte;
                        cmdBuffer.data[cmdBuffer.len+1] = '\0';
                    }
                }
            }
        }

        if (cmdBuffer.status == CMD_READY) {
            fputs(cmdBuffer.data, &USBSerialStream);
            fputs("\r\n", &USBSerialStream);
            cmdBuffer.len = 0;
            cmdBuffer.data[0] = '\0';
            cmdBuffer.status = CMD_NOTREADY;
        }
    }
}

ISR(TIMER0_COMPA_vect) {
    // Tick the clock, tell the display to service
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
