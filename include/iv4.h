/*
** iv4.h - header file for iv4.c containing character table defines and
**         function prototypes for the iv4lw
** Requires:    stdint.h
** Required by: iv4.c
*/

#ifndef IV4_H
#define IV4_H

/* Defining DISPLAY_REVERSED will swap the order in which our shift registers
 * get their data.  This allows us to plug the display board in rotated 180
 * degrees and keep the same grid/segment layout; the only thing that needs to
 * change are the bitfield defines.
 */
#define DISPLAY_REVERSED

/*  BIT CHAIN:
 *  Default:
 *	--op --ah   gfnm --el   --dc bkji
 *  Reversed:
 *  --el --dc   bkji --op   --ah gfnm
 */

#define blank		    0x00, 0x00, 0x00
#define char_all	  0x33, 0xf3, 0x3f

#ifdef DISPLAY_REVERSED
  #define grid_a    0x80
  #define grid_b    0x40
  #define grid_c    0x04
  #define grid_d    0x08

  #define grid_a_byte 2
  #define grid_b_byte 2
  #define grid_c_byte 0
  #define grid_d_byte 0

  #define char_0    0x23, 0xa0, 0x3e
  #define char_1		0x03, 0x00, 0x00
  #define char_2		0x21, 0xc2, 0x2c
  #define char_3		0x23, 0xc0, 0x24
  #define char_4		0x03, 0x42, 0x10
  #define char_5		0x22, 0xc2, 0x34
  #define char_6		0x22, 0xc2, 0x3c
  #define char_7		0x03, 0x80, 0x20
  #define char_8		0x23, 0xc2, 0x3c
  #define char_9		0x03, 0xc2, 0x30
  #define char_A		0x03, 0xc2, 0x38
  #define char_B		0x23, 0xd0, 0x25
  #define char_C		0x20, 0x80, 0x3c
  #define char_D		0x23, 0x90, 0x25
  #define char_E		0x20, 0x82, 0x3c
  #define char_F		0x00, 0x82, 0x38
  #define char_G		0x22, 0xc0, 0x3c
  #define char_H		0x03, 0x42, 0x18
  #define char_I		0x20, 0x90, 0x25
  #define char_J		0x23, 0x00, 0x0c
  #define char_K		0x10, 0x22, 0x18
  #define char_L		0x20, 0x00, 0x1c
  #define char_M		0x03, 0x21, 0x18
  #define char_N		0x13, 0x01, 0x18
  #define char_O		0x23, 0x80, 0x3c
  #define char_P		0x01, 0xc2, 0x38
  #define char_Q		0x33, 0x80, 0x3c
  #define char_R		0x11, 0xc2, 0x38
  #define char_S		0x22, 0xc2, 0x34
  #define char_T		0x00, 0x90, 0x21
  #define char_U		0x23, 0x00, 0x1c
  #define char_V		0x00, 0x20, 0x1a
  #define char_W		0x13, 0x00, 0x1a
  #define char_X		0x10, 0x21, 0x02
  #define char_Y		0x00, 0x21, 0x01
  #define char_Z		0x20, 0xa0, 0x26
  #define char_a		0x20, 0x02, 0x0d
  #define char_at		0x23, 0xc0, 0x39
  #define char_b		0x00, 0x02, 0x1d
  #define char_c		0x00, 0x02, 0x0c
  #define char_col	0x00, 0x02, 0x04
  #define char_d		0x00, 0x12, 0x0d
  #define char_e		0x00, 0x02, 0x0e
  #define char_eq		0x20, 0x42, 0x04
  #define char_f		0x00, 0xd2, 0x01
  #define char_g		0x00, 0x12, 0x35
  #define char_gt		0x00, 0x01, 0x02
  #define char_h		0x00, 0x02, 0x19
  #define char_i		0x00, 0x00, 0x01
  #define char_j		0x00, 0x00, 0x05
  #define char_k		0x10, 0x50, 0x01
  #define char_l		0x20, 0x10, 0x21
  #define char_lt		0x10, 0x20, 0x00
  #define char_m		0x02, 0x42, 0x09
  #define char_n		0x00, 0x02, 0x09
  #define char_o		0x00, 0x02, 0x0d
  #define char_p		0x00, 0x12, 0x38
  #define char_q		0x20, 0x12, 0x31
  #define char_qmrk	0x01, 0xc0, 0x21
  #define char_r		0x00, 0x02, 0x08
  #define char_s		0x00, 0x40, 0x05
  #define char_scol	0x20, 0x40, 0x00
  #define char_star	0x10, 0x73, 0x03
  #define char_t		0x20, 0x52, 0x01
  #define char_u		0x00, 0x00, 0x0d
  #define char_v		0x00, 0x00, 0x0a
  #define char_w		0x22, 0x00, 0x0d
  #define char_x		0x00, 0x30, 0x03
  #define char_y		0x32, 0x00, 0x00
  #define char_z		0x00, 0x02, 0x06
#else
  #define grid_a    0x08
  #define grid_b    0x04
  #define grid_c    0x80
  #define grid_d    0x40

  #define grid_a_byte 0
  #define grid_b_byte 0
  #define grid_c_byte 2
  #define grid_d_byte 2

  #define char_0		0x03, 0xe2, 0x3a
  #define char_1		0x00, 0x00, 0x30
  #define char_2		0x22, 0xc2, 0x1c
  #define char_3		0x02, 0x42, 0x3c
  #define char_4		0x21, 0x00, 0x34
  #define char_5		0x23, 0x42, 0x2c
  #define char_6		0x23, 0xc2, 0x2c
  #define char_7		0x02, 0x00, 0x38
  #define char_8		0x23, 0xc2, 0x3c
  #define char_9		0x23, 0x00, 0x3c
  #define char_A		0x23, 0x80, 0x3c
  #define char_B		0x02, 0x52, 0x3d
  #define char_C		0x03, 0xc2, 0x08
  #define char_D		0x02, 0x52, 0x39
  #define char_E		0x23, 0xc2, 0x08
  #define char_F		0x23, 0xc0, 0x08
  #define char_G		0x23, 0xc2, 0x2c
  #define char_H		0x21, 0x80, 0x34
  #define char_I		0x02, 0x52, 0x09
  #define char_J		0x20, 0xc2, 0x34
  #define char_K		0x21, 0x81, 0x02
  #define char_L		0x01, 0xc2, 0x00
  #define char_M		0x11, 0x80, 0x32
  #define char_N		0x11, 0x81, 0x30
  #define char_O		0x03, 0xc2, 0x38
  #define char_P		0x23, 0x80, 0x1c
  #define char_Q		0x03, 0xc3, 0x38
  #define char_R		0x23, 0x81, 0x1c
  #define char_S		0x23, 0x42, 0x2c
  #define char_T		0x02, 0x10, 0x09
  #define char_U		0x01, 0xc2, 0x30
  #define char_V		0x01, 0xa0, 0x02
  #define char_W		0x01, 0xa1, 0x30
  #define char_X		0x10, 0x21, 0x02
  #define char_Y		0x10, 0x10, 0x02
  #define char_Z		0x02, 0x62, 0x0a
  #define char_a		0x20, 0xd2, 0x00
  #define char_at		0x03, 0x92, 0x3c
  #define char_b		0x21, 0xd0, 0x00
  #define char_c		0x20, 0xc0, 0x00
  #define char_col	0x20, 0x40, 0x00
  #define char_d		0x20, 0xd0, 0x01
  #define char_e		0x20, 0xe0, 0x00
  #define char_eq		0x20, 0x42, 0x04
  #define char_f		0x20, 0x10, 0x0d
  #define char_g		0x23, 0x50, 0x01
  #define char_gt		0x10, 0x20, 0x00
  #define char_h		0x21, 0x90, 0x00
  #define char_i		0x00, 0x10, 0x00
  #define char_j		0x00, 0x50, 0x00
  #define char_k		0x00, 0x11, 0x05
  #define char_l		0x02, 0x12, 0x01
  #define char_lt		0x00, 0x01, 0x02
  #define char_m		0x20, 0x90, 0x24
  #define char_n		0x20, 0x90, 0x00
  #define char_o		0x20, 0xd0, 0x00
  #define char_p		0x23, 0x80, 0x01
  #define char_q		0x23, 0x12, 0x01
  #define char_qmrk	0x02, 0x10, 0x1c
  #define char_r		0x20, 0x80, 0x00
  #define char_s		0x00, 0x50, 0x04
  #define char_scol	0x00, 0x02, 0x04
  #define char_star	0x30, 0x31, 0x07
  #define char_t		0x20, 0x12, 0x05
  #define char_u		0x00, 0xd0, 0x00
  #define char_v		0x00, 0xa0, 0x00
  #define char_w		0x00, 0xd2, 0x20
  #define char_x		0x00, 0x30, 0x03
  #define char_y		0x00, 0x03, 0x20
  #define char_z		0x20, 0x60, 0x00
#endif

uint8_t* bufferBytes(uint8_t* buffer, uint8_t index);
uint8_t* bufferChar(uint8_t* buffer, uint8_t index);
uint8_t* selectGrid(uint8_t* buffer, uint8_t n);

#endif
