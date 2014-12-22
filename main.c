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
static cbuffer_t cmdBuffer;
static time_t time;

uint8_t bitMap[4][3];
uint8_t charMap[4];
uint8_t timeMap[4];
uint8_t activeGrid;

uint8_t* displayPtr;

volatile uint8_t update;

int main(void) {
    SetupHardware();

    /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

    GlobalInterruptEnable();

    strncpy(charMap, "ASDF", 4);
    string_time("1259", &time);

    displayPtr = timeMap;

    if (Buttons_GetStatus() == BUTTONS_BUTTON1)
        Jump_To_Bootloader();

    for (;;) {
        /* A timer interrupt sets update to 1 every 4 milliseconds */
        if (update == 1) {

            if((Buttons_GetStatus() == BUTTONS_BUTTON1) && ((time.ticks % 25) == 0)) {
                time.minutes++;
                time.seconds = 0;
            }

            update_time(&time);
            time_string(&time, timeMap);
            ProcessCommand();
            ProcessDisplay();

            update = 0;
        }

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
    Buttons_Init();
    LEDs_Init();
    USB_Init();
    initSHR();

    /* Initialize timer0 */
    /* This sets up a timer interrupt at 250Hz to signal display service */
    OCR0A   = 249;                           // Top count; timer0 interrupt rate = 16MHz / (256 * (OCR0A + 1))
    TCCR0A |= (1 << WGM01);                  // CTC mode
    TIMSK0 |= (1 << OCIE0A);                 // Enable interrupt generation on OCR0A match
    TCCR0B |= (1 << CS02);                   // F/256 prescaler; start timer

    /* Initialize timer1 */
    /* This sets up a roughly 10kHz signal on OCR1B, PORTC5, to drive our filament */
    /* The exact frequency here doesn't matter as long as it doesn't beat against the display mux rate */
    DDRC   |= (1 << 5);                      // Enable OCR1B as output pin
    OCR1A   = 825;                           // Top count
    TCCR1A |= (1 << COM1B0);                 // Toggle OCR1B on compare match
    TCCR1B |= (1 << WGM12) | (1 << CS10);    // CTC mode, F/1 prescaler; start timer
}

/** Function to deal with incoming serial bytes on CDC interface and place them in our command buffer */
void ProcessInput(void)
{
    // Return if no bytes are waiting in the buffer
    if ( CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) == 0 ) {
        return;
    }

    // Grab the next byte from the virutal serial interface
    int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

    if (!(ReceivedByte < 0 )) {
        // Echo the received byte on the virtual serial interface
        //fputc(ReceivedByte, &USBSerialStream);

        // Jump to bootloader if the byte was 'X'
        if ((uint8_t)ReceivedByte == 'X') {
            Jump_To_Bootloader();
        }

        if (cmdBuffer.status != CMD_READY) {
            // 0x0d means process this command
            if ((uint8_t)ReceivedByte == 0x0d) {
                cmdBuffer.status = CMD_READY;
            } else {
                // Anything else, append to buffer
                if (cmdBuffer.len < (CMD_BUF_SIZE - 1)) {
                    cmdBuffer.data[cmdBuffer.len++] = (uint8_t)ReceivedByte;
                }
            }
        }
    }
}

void ProcessCommand(void) {
    if (cmdBuffer.status != CMD_READY)
        return;

    //fputs(cmdBuffer.data, &USBSerialStream);
    //fputs("\r\n", &USBSerialStream);

    switch (cmdBuffer.data[0]) {
        case 'P':
            displayPtr = charMap;
            strncpy(charMap, &cmdBuffer.data[1], 4);
            cmdBuffer.status = CMD_COMPLETE;
            break;
        case 'T':
            displayPtr = timeMap;
            strncpy(timeMap, &cmdBuffer.data[1], 4);
            string_time(timeMap, &time);
            cmdBuffer.status = CMD_COMPLETE;
            break;
        default:
            fputs_P(PSTR("Bad command!\r\n"), &USBSerialStream);
            cmdBuffer.status = CMD_ERROR;
    }

    // Zero buffer data and length
    memset(cmdBuffer.data, '\0', CMD_BUF_SIZE);
    cmdBuffer.len = 0;
}

void ProcessDisplay(void) {
    activeGrid++;
    if (activeGrid >= 4)
        activeGrid = 0;

    bufferChar(bitMap[activeGrid], (uint8_t) displayPtr[activeGrid]);
    selectGrid(bitMap[activeGrid], activeGrid);

    SHRBlank();
    SHRSendBuffer(bitMap[activeGrid]);
    SHRLatch();
    SHRUnblank();
}

ISR(TIMER0_COMPA_vect) {
    update = 1;
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
