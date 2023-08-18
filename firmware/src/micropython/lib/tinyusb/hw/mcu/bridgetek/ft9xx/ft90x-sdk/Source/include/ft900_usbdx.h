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

#ifndef USBD_H_6AGQHMZB
#define USBD_H_6AGQHMZB

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <ft900_memctl.h>

//#define FEATURE_ACM_LOOPBACK

//#define FEATURE_LOGD
//#define FEATURE_LOGI
//#define FEATURE_LOGE
//#define FEATURE_ASSERT_IN_DEBUG_BUILD

//#define FEATURE_USBD_DEBUG
//#define FEATURE_USBDOUT_DEBUG
//#define FEATURE_USBDIN_DEBUG

//#define FEATURE_ACM_DEBUG
//#define FEATURE_ACMTX_DEBUG
//#define FEATURE_ACMRX_DEBUG

//#define FEATURE_NCM_DEBUG
//#define FEATURE_NCMTX_DEBUG
//#define FEATURE_NCMRX_DEBUG

/*********************************************************************
 *                           Common macros                           *
 *********************************************************************/
#define CONST			const __flash__
#define STRINGIZE(x)	#x
#define INTSTR(x)		STRINGIZE(x)
#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)
#ifndef BIT
#define BIT(x)			(1U << (x))
#endif /*BIT*/
#define ARRAY_SIZE(x)	(sizeof(x)/sizeof((x)[0]))

#define LOBYTE(w) ((uint8_t)(w))
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define NOLOG(x, ...)
#define UNUSED_PARAMETER(x) (void)(x)

/*********************************************************************
 *                        Assembly functions                         *
 *********************************************************************/
void pm_memcpy(void *dest, CONST void *src, int16_t n)
__attribute__((nonnull(1, 2)));

/*********************************************************************
 *                        Debugging functions                        *
 *********************************************************************/
#ifndef NDEBUG

void hex_dump(const void *addr, int len);

#define LOGBASE(x, ...) \
		do { \
			static CONST char local[] = (x); \
			tfp_printf("%s",(CONST char *)local); \
		} while (0)


#ifndef FEATURE_LOGD
#define LOGD(x, ...)
#else /* !FEATURE_LOGD */
#define LOGD(x, ...)			LOGBASE(x, ##__VA_ARGS__)
#endif /* FEATURE_LOGD */

#ifndef FEATURE_LOGI
#define LOGI(x, ...)
#else /* !FEATURE_LOGI */
#define LOGI(x, ...)			LOGBASE(x, ##__VA_ARGS__)
#endif /* FEATURE_LOGI */

#ifndef FEATURE_LOGE
#define LOGE(x, ...)
#else /* !FEATURE_LOGE */
#define LOGE(x, ...)			LOGBASE(x, ##__VA_ARGS__)
#endif /* FEATURE_LOGE */

#ifndef FEATURE_ASSERT_IN_DEBUG_BUILD
#define assert(__e)
#else /* !FEATURE_ASSERT_IN_DEBUG_BUILD */
#define assert(__e) \
	do { \
		static CONST char local[] = "ASSERT at " __FILE__ "+" \
			INTSTR(__LINE__) ": " STRINGIZE(__e) "\r\n"; \
\
		if (__e) \
			break; \
			tfp_printf("%s",(CONST char *)local); \
		while (1); \
	} while (0)
#endif /* FEATURE_ASSERT_IN_DEBUG_BUILD */

#else /* !NDEBUG */

#define LOGD(x, ...)
#define LOGI(x, ...)
#define LOGE(x, ...)

#define assert(__e)

#endif /* NDEBUG */

/**********************************************************************
*                  Structure and constants defines                   *
**********************************************************************/

struct urb {
	uint8_t *start;

	/* reset to &buf[0] after data transfer
	 * OUT: start of buffer to be processed
	 *  IN: end of data to be transferred */
	uint8_t *ptr;

	/* OUT: end of data to be transferred
	 *  IN: Maximum packet length */
	uint8_t *end;
	bool owned_by_usbd;
	uint8_t id;
	struct urb *next;
};

struct pipe;

/* return true if any URB is submitted in callback function */
typedef bool (*usbd_callback)(struct pipe *pp);

struct pipe {
	struct urb *usbd_urb;
	struct urb *app_urb;
	uint8_t *buf_start;
	uint8_t *buf_end;
	usbd_callback on_usbd_ready;
	usbd_callback on_usbd_underrun;
	uint8_t id;
	uint8_t ep;
	bool usbd_paused;
	bool app_paused;
};

/**********************************************************************
*                          Helper functions                          *
**********************************************************************/
/* URB id, for debugging purpose */
static inline uint8_t urb_get_id(const struct urb *urb)
{
	return urb->id;
}

/* Length to be processed by application
 * IN: length of free space to fill in, OUT: length of host data */
static inline uint16_t urb_get_app_to_process(const struct urb *urb)
{
	return urb->end - urb->ptr;
}

/* Length of data already been processed by application */
static inline uint16_t urb_get_app_consumed(const struct urb *urb)
{
	return urb->ptr - urb->start;
}

/* To detect aligned IN data transfer */
static inline bool urb_in_fully_filled(const struct urb *urb)
{
	return urb->end == urb->ptr;
}

static inline bool urb_in_empty(const struct urb *urb)
{
	return urb->start == urb->ptr;
}

/* To detect if application are free to use the URB */
static inline bool urb_owned_by_app(const struct urb *urb)
{
	return !urb->owned_by_usbd;
}

/* Get next application URB pointer, but it may still been hold by USBD engine
 * at this point. To be used in conjunction with urb_owned_by_app() to know
 * the ownership */
static inline struct urb *usbd_get_app_urb(const struct pipe* pp)
{
	return pp->app_urb;
}

/* Application pause itself when no more buffer can be processed, and waiting
 * for USBD to give more datas. USBD engine will call the on_usbd_ready()
 * callback once app is paused, to resume the application's process. */
static inline void usbd_set_app_paused(struct pipe *pp)
{
	pp->app_paused = true;
}

/* To detect if USBD engine is paused */
static inline bool usbd_is_engine_paused(const struct pipe *pp)
{
	return pp->usbd_paused;
}

/**********************************************************************
*                           USBD functions                           *
**********************************************************************/
//@{
/**
    @brief     Initialise the URB pipe for data transfer.
    @details   Application provides a linear space buffer to USBD. An array of
    			URBs is initialised by USBD, dividing the linear space into
    			small chunks (512 bytes for HS, 64 bytes for FS). Pipe is
    			initialised by USBD, pointing to its own URBs. Each Endpoint
    			is represented by a pipe structure.
				Application can create multiple pipes.
    @param[in] pp Pipe structure that gets initialised by USBD upon creation of pipe.
    @param[in] id USBD_ENDPOINT_NUMBER for which a pipe has to be created.
    @param[in] ep USBD_ENDPOINT_NUMBER with MSB bit set incase of IN Endpoints.
    @param[in] urbs URB structure that gets initialised by USBD upon creation of pipe.
    @param[in] bufs A linear buffer to be passed by the application which gets divided into URBs.
    @param[in] urb_count number of URBs.
    @return    'True' if pipe is created. 'False' if the input parameters are invalid.
 **/
bool usbd_pipe_init(
		struct pipe *pp,
		uint8_t id,
		uint8_t ep,
		struct urb *urbs,
		uint8_t *bufs,
		uint8_t urb_count);

/**
    @name Optional functions for registering event callbacks.
 */
//@{
/**
    @brief     To register callback function for USBD READY event.
    @details   Application shall call this API to register their callback
    			function to get notified of when USBD is ready to
    			process the URBs.
    @param[in] pp Pipe for which the callback function is associated to.
 **/
void register_on_usbd_ready(struct pipe *pp, usbd_callback callback);

/**
    @brief     To register callback function for USDB UNDERRUN event.
    @details   Application shall call this API to register their callback
    			function to get notified of when USBD is underrunning
    			when no data from application.
    @param[in] pp Pipe for which the callback function is associated to.
 **/
void register_on_usbd_underrun(struct pipe *pp, usbd_callback callback);
//@}

/**
    @brief     Always returns a URB.
    @details   Application can choose to acquire URB
    			even if it is still held by USBD.
    @param[in] pp The pipe whose data to be transferred to/from USBD
    @return    A URB.
 **/
struct urb *usbd_force_acquire_urb_for_app(struct pipe *pp);

/**
    @brief     Submit a URB to USBD.
    @details   Application fills the urb and submits to USBD through this API call.
				When a URB is submitted without filling, this will make USBD to send a ZLP.
	@attention This function should be protected by critical section since it
	           is called from application's context.
    @param[in] pp The pipe whose data to be transferred to/from USBD.
    @param[in] urb Filled URB.
 **/
void usbd_submit_urb(struct pipe *pp, struct urb *urb);

/**
    @brief     To get more than one URBs for application.
    @details   Application to use this API to get more than one URB from the USBD.
    @attention This function should be called to obtain the length before copying
    			and submitting the URBs through usbd_sumit_urbs().
    @param[in] pp The pipe for which the data transfer is needed.
    @param[in] len The length of the data that the application wants to transfer.
    @return    The length of the URBs that is available for application usage.
 **/
uint8_t *usbd_get_app_urbs(const struct pipe *pp, uint16_t len);

/**
    @brief     To submit more than one URBs from application.
    @details   Application fills the URBs and submits to USBD through this API call.
	@attention This function should be protected by critical section since it
	           is called from application's context.
    @param[in] pp The pipe whose data to be transferred to/from USBD.
    @param[in] len The length must be equal or smaller than the length gotten from usbd_get_app_urbs().
    @return The next available URB
 **/
struct urb *usbd_submit_urbs(struct pipe *pp, uint16_t len);

/**
    @brief     The given pipe is processed to transfer the data to/from USBD hardware endpoint.
    @details   The given pipe is processed to transfer the data to/from USBD hardware endpoint.
    @attention This function has to be called from USBD_pipe_isr() implemented by the application.
    @param[in] pp The pipe whose data to be transferred to/from USBD.
 **/
void usbd_pipe_process(struct pipe *pp);

/**
    @brief     To flush the pipe.
    @details   Purge the data in the the URBs for a given pipe
    @param[in] pp Pipe for which data to be purged.
 **/
void usbd_pipe_purge(struct pipe *pp);

void usbd_buf_write(struct pipe *pp,
		void (*buf_write)(uint8_t *usbd, const uint8_t *app, uint16_t len),
		uint8_t *usbd, const uint8_t *app, uint16_t len);

void usbd_buf_read(struct pipe *pp,
		void (*buf_read)(uint8_t *app, const uint8_t *usbd, uint16_t len),
		uint8_t *app, const uint8_t *usbd, uint16_t len);

void usbd_stream_write(struct pipe *pp,
		void (*stream_write)(uint8_t *usbd, const uint8_t *app, uint16_t len),
		uint8_t *usbd, const uint8_t *app, uint16_t len);

void usbd_stream_read(struct pipe *pp,
		void (*stream_read)(uint8_t *app, const uint8_t *usbd, uint16_t len),
		uint8_t *app, const uint8_t *usbd, uint16_t len);

void usbd_pipe_isr_start(void);

void usbd_pipe_isr_stop(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* end of include guard: USBD_H_6AGQHMZB */
