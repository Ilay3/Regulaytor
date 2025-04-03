/*************************************************************
*		Этот файл служит для того, чтобы обеспечить переносимость
*	типов от компилятора к компилятору.
*			VISUAL C++
**************************************************************/

/**********   ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ   *********************
*	28.06.07 - создан
*	1.08.07 - добавлены INT64S and INT64U
*	22.04.08 - удалена ";" с конца макроса BIN
*	15.09.08 - добавлены пределы
***************************************************************/

#pragma once

typedef unsigned char     	INT8U;
typedef signed char       	INT8S;
typedef unsigned short     	INT16U;
typedef signed short       	INT16S;
typedef unsigned int     	INT32U;
typedef signed int       	INT32S;
typedef unsigned long		INT64U;
typedef signed long			INT64S;
typedef float             	FP32;
typedef double				FP64;

//макрос облегчает написание двоичных чисел, например так
// BYTE b=BIN(0,0,1,0,0,1,0,1)  равно 0b00100101
#define BIN(bit7,bit6,bit5,bit4,bit3,bit2,bit1,bit0) \
    ((bit7<<7)|(bit6<<6)|(bit5<<5)|(bit4<<4)|(bit3<<3)|(bit2<<2)|(bit1<<1)|(bit0<<0))

#define INT8U_MAX	0xFF
#define INT8U_MIN	0x00
#define INT16U_MAX	0xFFFF
#define INT16U_MIN	0x0000
#define INT32U_MAX	0xFFFFFFFFU
#define INT32U_MIN	0x00000000
#define INT64U_MAX	0xFFFFFFFFFFFFFFFFUL
#define INT64U_MIN	0x0000000000000000
#define INT8S_MAX	0x7F
#define INT8S_MIN	(-INT8S_MAX -1)
#define INT16S_MAX	0x7FFF
#define INT16S_MIN	(-INT16S_MAX-1)
#define INT32S_MAX	0x7FFFFFFF
#define INT32S_MIN	(-INT32S_MAX-1)
#define INT64S_MAX	0x7FFFFFFFFFFFFFFFLL
#define INT64S_MIN	(-INT64S_MAX-1)
