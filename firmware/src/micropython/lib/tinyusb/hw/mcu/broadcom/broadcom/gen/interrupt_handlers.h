#pragma once

extern void* interrupt_handlers[];

#if BCM_VERSION != 2711
#define INTERRUPT_COUNT (72)
#else
#define INTERRUPT_COUNT (160)
#endif