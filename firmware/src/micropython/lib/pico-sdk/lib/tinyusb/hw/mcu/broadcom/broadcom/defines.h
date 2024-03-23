#pragma once

#if BCM_VERSION == 2835
#include "broadcom/gen/bcm2835_lpa.h"
#elif BCM_VERSION == 2837
#include "broadcom/gen/bcm2837_lpa.h"
#elif BCM_VERSION == 2711
#include "broadcom/gen/bcm2711_lpa.h"
#endif
