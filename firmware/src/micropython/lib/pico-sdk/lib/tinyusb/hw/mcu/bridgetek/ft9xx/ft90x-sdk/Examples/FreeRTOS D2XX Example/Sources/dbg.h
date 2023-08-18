#ifndef __DBG_H__
#define __DBG_H__

#include "tinyprintf.h"

#define DEBUG

#ifdef DEBUG
#define dbg(s,...)	tfp_printf((s),##__VA_ARGS__)
#else
#define dbg(s,...)
#endif

#endif	//__DBG_H__
