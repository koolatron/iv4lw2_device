#define SHR_DDR  DDRD
#define SHR_PORT PORTD
#define CLOCK    6
#define LATCH    5
#define DATA     4
#define BLANK    7

void initSHR(void);
void SHRSendByte(unsigned char byte);
void SHRSendBuffer(unsigned char* buffer, unsigned char n);
void SHRLatch(void);
void SHRReset(void);
void SHRBlank(void);
void SHRUnblank(void);
