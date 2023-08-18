/**
 * ============================================================================
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of
 * loss such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However,
 * Bridgetek has no liability in relation to those amendments.
 * ============================================================================
 */

#include <stdint.h>
#include <ft900.h>
#include <ft900_usbdx.h>
#include <ft900_memctl.h>

#ifndef FEATURE_USBD_DEBUG
#define USBDD(x, ...)
#define USBDI(x, ...)
#define USBDE(x, ...)
#else /* !FEATURE_USBD_DEBUG */
#define USBDD(x, ...)		LOGD(x, ##__VA_ARGS__)
#define USBDI(x, ...)		LOGI(x, ##__VA_ARGS__)
#define USBDE(x, ...)		LOGE(x, ##__VA_ARGS__)
#endif /* FEATURE_USBD_DEBUG */

#ifndef FEATURE_USBDOUT_DEBUG
#define USBDOUTD(x, ...)
#define USBDOUTI(x, ...)
#define USBDOUTE(x, ...)
#else /* !FEATURE_USBDOUT_DEBUG */
#define USBDOUTD(x, ...)		LOGD(x, ##__VA_ARGS__)
#define USBDOUTI(x, ...)		LOGI(x, ##__VA_ARGS__)
#define USBDOUTE(x, ...)		LOGE(x, ##__VA_ARGS__)
#endif /* FEATURE_USBDOUT_DEBUG */


#ifndef FEATURE_USBDIN_DEBUG
#define USBDIND(x, ...)
#define USBDINI(x, ...)
#define USBDINE(x, ...)
#else /* !FEATURE_USBDIN_DEBUG */
#define USBDIND(x, ...)		LOGD(x, ##__VA_ARGS__)
#define USBDINI(x, ...)		LOGI(x, ##__VA_ARGS__)
#define USBDINE(x, ...)		LOGE(x, ##__VA_ARGS__)
#endif /* FEATURE_USBDIN_DEBUG */

#ifndef NDEBUG
static bool in_usbd_isr;
#endif /* !NDEBUG */

/**
 @brief Decode of USB_ENDPOINT_SIZE definitions into actual endpoint
  Max Packet size values.
static const uint16_t usbd_ep_size_bytes[D2XX_TOTAL_NUM_SIZES] =
{ 8, 16, 32, 64, 128, 256, 512, 1024 };
 **/
#define usbd_ep_size_bytes(x)		(8 << (x))

static inline bool usbd_is_pipe_ready(const struct pipe *pp)
{
	return !!(USBD_EP_SR_REG(pp->id) &
		((pp->ep & 0x80) ? MASK_USBD_EPxSR_INPRDY : MASK_USBD_EPxSR_OPRDY));
}

static inline void usbd_in_request(uint8_t pipe,
		const uint8_t *buffer, uint16_t len)
{
	volatile uint8_t *data_reg = (uint8_t *)&(USBD->ep[pipe].epxfifo);
	uint16_t aligned = len & (~3);
	uint16_t left = len & 3;

	//assert(len <= 512);
	assert((uint32_t)buffer % 4 == 0);
	if (aligned) {
		__asm__ volatile ("streamout.l %0,%1,%2" : :
				"r"(data_reg), "r"(buffer), "r"(aligned));
	}
	if (left) {
		__asm__ volatile ("streamout.b %0,%1,%2" : :
				"r"(data_reg), "r"(buffer + aligned), "r"(left));
	}
	USBD_EP_SR_REG(pipe) = MASK_USBD_EPxSR_INPRDY;
}

static inline uint16_t usbd_out_request(uint8_t pipe, uint8_t *buffer)
{
	volatile uint8_t *data_reg = (uint8_t *)&(USBD->ep[pipe].epxfifo);
	uint16_t len = USBD_EP_CNT_REG(pipe);
	uint16_t aligned= len & (~3);
	uint16_t left = len & 3;

	//assert(len <= 512);
	assert((uint32_t)buffer % 4 == 0);
	if (aligned) {
		__asm__ volatile ("streamin.l %0,%1,%2" : :
				"r"(buffer), "r"(data_reg), "r"(aligned));
	}
	if (left) {
		__asm__ volatile ("streamin.b %0,%1,%2" : :
				"r"(buffer + aligned), "r"(data_reg), "r"(left));
	}
	USBD_EP_SR_REG(pipe) = MASK_USBD_EPxSR_OPRDY;
	return len;
}

static inline void usbd_in_transfer(const struct pipe* pp, struct urb *urb)
{
	uint16_t len = urb_get_app_consumed(urb);

	usbd_in_request(pp->id, urb->start, len);
	urb->ptr = urb->start;
	USBDIND("PP%d IN%d len:%d\r\n", pp->id, pp->usbd_urb->id, len);
}

static inline void usbd_out_transfer(const struct pipe* pp, struct urb *urb)
{
	urb->end = urb->start + usbd_out_request(pp->id, urb->start);
	urb->ptr = urb->start;
	USBDOUTD("PP%d OUT%d len:%d\r\n",
			pp->id, pp->usbd_urb->id, urb_get_app_to_process(urb));
}

static inline void usbd_release_urb(struct pipe *pp, struct urb *urb)
{
	urb->owned_by_usbd = false;
	pp->usbd_urb = pp->usbd_urb->next;
}

static inline void usbd_transfer(struct pipe* pp, struct urb *urb)
{
	if (pp->ep & 0x80) {
		usbd_in_transfer(pp, urb);
	} else
		usbd_out_transfer(pp, urb);
	usbd_release_urb(pp, urb);
}

static inline struct urb *usbd_get_usbd_urb(struct pipe* pp)
{
	return pp->usbd_urb;
}

static inline bool pipe_underrun(struct pipe *pp, struct urb *urb)
{
	if (urb->owned_by_usbd)
		return false;
	if (pp->on_usbd_underrun) {
		pp->usbd_paused = !pp->on_usbd_underrun(pp);
		#ifndef NDEBUG
		if (pp->usbd_paused) {
			USBDD("PP%d paused at URB%d\r\n", pp->id, urb->id);
		}
		#endif /* !NDEBUG */
		return pp->usbd_paused;
	} else {
		pp->usbd_paused = true;
		USBDD("PP%d paused at URB%d\r\n", pp->id, urb->id);
		return true;
	}
}

void usbd_submit_urb(struct pipe *pp, struct urb *urb)
{
#ifndef NDEBUG
	if (!in_usbd_isr) {
		/* Global interrupt must be masked when calling usbd_submit_urb() */
		assert(INTERRUPT->global_mask & 0x80);
	}
#endif /* !NDEBUG */
	if (pp->usbd_paused) {
		USBDD("PP%d resume from app\r\n", pp->id);
		/* start transfer again and clear paused flag */
		pp->usbd_paused = false;
		usbd_transfer(pp, urb);
	} else {
		/* queue back to usbd */
		urb->owned_by_usbd = true;
	}
	pp->app_urb = pp->app_urb->next;
}

struct urb *usbd_submit_urbs(struct pipe *pp, uint16_t len)
{
	struct urb *urb = usbd_get_app_urb(pp);

	do {
		uint16_t app = urb_get_app_to_process(urb);

		assert(urb->end >= urb->ptr);
		if (len < app) {
			urb->ptr += len;
			break;
		}
		urb->ptr += app;
		len -= app;
		usbd_submit_urb(pp, urb);
		urb = urb->next;
	} while (len);
	return urb;
}

struct urb *usbd_force_acquire_urb_for_app(struct pipe *pp)
{
	struct urb *urb = usbd_get_app_urb(pp);

#ifndef NDEBUG
	if (!in_usbd_isr) {
		/* Global interrupt must be masked when calling usbd_submit_urb() */
		assert(INTERRUPT->global_mask & 0x80);
	}
#endif /* !NDEBUG */
	if (urb->owned_by_usbd) {
		urb->ptr = urb->start;
		usbd_release_urb(pp, urb);
	}
	return urb;
}

uint8_t *usbd_get_app_urbs(const struct pipe *pp, uint16_t len)
{
	struct urb *urb = usbd_get_app_urb(pp);
	uint8_t *ptr = urb->ptr;
	uint16_t total = 0;

	while (!urb->owned_by_usbd) {
		total += urb_get_app_to_process(urb);
		if (total >= len) {
			return ptr;
		}
		urb = urb->next;
	}
	return NULL;
}

#ifndef NDEBUG
void usbd_pipe_isr_start(void)
{
	in_usbd_isr = true;
}

void usbd_pipe_isr_stop(void)
{
	in_usbd_isr = false;
}
#endif /* !NDEBUG */

void usbd_pipe_process(struct pipe *pp)
{
	struct urb *urb = usbd_get_usbd_urb(pp);

	assert (!pp->usbd_paused);
	/* No buffer belongs to ISR */
	if (pipe_underrun(pp, urb))
		return;

	usbd_transfer(pp, urb);

	if (pp->app_paused && pp->on_usbd_ready)
		pp->app_paused = !pp->on_usbd_ready(pp);
}

void usbd_pipe_purge(struct pipe *pp)
{
	struct urb *urb = usbd_get_usbd_urb(pp);
	uint8_t pipe_id = pp->id;

	/* No buffer belongs to ISR */
	if (pipe_underrun(pp, urb))
		return;

	if (pp->ep & 0x80)
	{
		/* purging the IN data available for endpoint */
		uint16_t len = urb_get_app_consumed(urb);

		if (len <= USBD_ep_max_size(pipe_id))
		{
			/* purge the data in the IN urb buffer */
			urb->ptr = urb->start;
		}
	}
	else
	{
		urb->end = urb->start;
		urb->ptr = urb->start;
	}
	usbd_release_urb(pp, urb);

	if (pp->app_paused && pp->on_usbd_ready)
		pp->app_paused = !pp->on_usbd_ready(pp);
}

void register_on_usbd_ready(struct pipe *pp,
		usbd_callback callback)
{
	pp->on_usbd_ready = callback;
}

void register_on_usbd_underrun(struct pipe *pp,
		usbd_callback callback)
{
	pp->on_usbd_underrun = callback;
}

bool usbd_pipe_init(
		struct pipe *pp,
		uint8_t id,
		uint8_t ep,
		struct urb *urbs,
		uint8_t *bufs,
		uint8_t urb_count)
{
	bool out_direction = !(ep & 0x80);
	uint16_t max_pkt_size = USBD_ep_max_size(id);

	pp->buf_start = bufs;
	pp->buf_end = bufs + max_pkt_size * urb_count;

	if (!urb_count)
		return false;
	if ((unsigned long)bufs % sizeof(uint32_t) != 0)
		return false;
	for (int i = 0; i < urb_count; i++) {
		urbs[i].start = bufs + i * max_pkt_size;
		urbs[i].ptr = urbs[i].start;
		urbs[i].end = urbs[i].start;
		if (!out_direction) {
			urbs[i].end += max_pkt_size;
			urbs[i].owned_by_usbd = false;
		} else
			urbs[i].owned_by_usbd = true;
		urbs[i].next = &urbs[(i + 1) % urb_count];
		urbs[i].id = i;
	}
	assert(pp->buf_end == urbs[urb_count - 1].start + max_pkt_size);

	pp->id = id;
	pp->ep = ep;
	pp->usbd_urb = urbs;
	pp->app_urb = urbs;

	/* For USB IN, usbd is paused by default because no app data */
	pp->usbd_paused = !out_direction;
	/* For USB OUT, app is paused by default because no USB data */
	pp->app_paused = out_direction;
	/* on_data_ready() will be called by USBD for paused app */
	return true;
}

void usbd_buf_write(struct pipe *pp,
		void (*buf_write)(uint8_t *usbd, const uint8_t *app, uint16_t len),
		uint8_t *usbd, const uint8_t *app, uint16_t len)
{
	uint16_t linear_len = pp->buf_end - usbd;

	if (unlikely(linear_len < len)) {
		buf_write(usbd, app, linear_len);
		buf_write(pp->buf_start, app + linear_len, len - linear_len);
	} else
		buf_write(usbd, app, len);
}

void usbd_buf_read(struct pipe *pp,
		void (*buf_read)(uint8_t *app, const uint8_t *usbd, uint16_t len),
		uint8_t *app, const uint8_t *usbd, uint16_t len)
{
	uint16_t linear_len = pp->buf_end - usbd;

	if (unlikely(linear_len < len)) {
		buf_read(app, usbd, linear_len);
		buf_read(app + linear_len, pp->buf_start, len - linear_len);
	} else
		buf_read(app, usbd, len);
}

void usbd_stream_write(struct pipe *pp,
		void (*stream_write)(uint8_t *usbd, const uint8_t *app, uint16_t len),
		uint8_t *usbd, const uint8_t *app, uint16_t len)
{
	uint16_t linear_len = pp->buf_end - usbd;

	if (unlikely(linear_len < len)) {
		stream_write(usbd, app, linear_len);
		stream_write(pp->buf_start, app, len - linear_len);
	} else
		stream_write(usbd, app, len);
}

void usbd_stream_read(struct pipe *pp,
		void (*stream_read)(uint8_t *app, const uint8_t *usbd, uint16_t len),
		uint8_t *app, const uint8_t *usbd, uint16_t len)
{
	uint16_t linear_len = pp->buf_end - usbd;

	if (unlikely(linear_len < len)) {
		stream_read(app, usbd, linear_len);
		stream_read(app, pp->buf_start, len - linear_len);
	} else
		stream_read(app, usbd, len);
}
