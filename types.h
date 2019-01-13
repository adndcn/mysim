#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int	int32_t;


typedef unsigned int oraddr_t;

/* Basic types for openrisc */
typedef uint32_t  oraddr_t;	/*!< Address as addressed by openrisc */
typedef uint32_t  uorreg_t;	/*!< An unsigned register of openrisc */
typedef int32_t   orreg_t;	/*!< A signed register of openrisc */

typedef long long LONGEST;
typedef unsigned long long ULONGEST;

#define CONVERT_SHORT(value) (((value & 0xff) << 8) | ((value & 0xff00) >> 8))
#define CONVERT_INT(value) (((value & 0xff) << 24) | (((value >> 8) & 0xff) << 16) | (((value >> 16) & 0xff) << 8) | ((value >> 24) & 0xff))

#define false 0
#define true 1