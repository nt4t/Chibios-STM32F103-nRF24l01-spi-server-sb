/*
Chibios-STM32F103-nRF24l01-spi-server-sb
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "nrf24l01.h"

/*
 * Low speed SPI configuration (140.625kHz, CPHA=0, CPOL=0, MSb first).
 */
static const SPIConfig ls_spicfg = {
  NULL,
  GPIOB,
  GPIOB_SPI2NSS,
  SPI_CR1_BR_2 | SPI_CR1_BR_1
};

/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static WORKING_AREA(blinker_wa, 128);
static msg_t blinker(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
//    palSetPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(500);
//    palClearPad(GPIOC, GPIOC_LED3);
    chThdSleepMilliseconds(500);
  }
}

void nrf_dump_regs(nrf_regs *r) {

    int i;
    int j;

//	cio_print("\n\r** START nRF2401 Register DUMP **\n\r");
    chprintf((BaseSequentialStream *) &SD1, "\n\r** START nRF2401 Register DUMP **\n\r");

    nrf_reg_buf buf;

    for(i = 0; i < r->count; i++) {

	    nrf_read_reg(i, &buf);

	    if(r->data[i].size == 0) continue;

//		cio_printf("%s: ", r->data[i].name);
	    chprintf((BaseSequentialStream *) &SD1, "%s: ", r->data[i].name);

	    for(j = 0; j < buf.size; j++) {
//			cio_printb(buf.data[j], 8);
//			cio_printf(" (%u) ", buf.data[j]);
		    chprintf((BaseSequentialStream *) &SD1, " (%u) ", buf.data[j]);
	    }

	    chprintf((BaseSequentialStream *) &SD1,"\n\r - ");

	    for(j = 0; j < r->data[i].fields->count; j++) {
		    chprintf((BaseSequentialStream *) &SD1, "%u[%u]:%s=%u ", j,
			    r->data[i].fields->data[j].size,
			    r->data[i].fields->data[j].name,
			    nrf_get_reg_field(i, j, &buf));
	    }

	    chprintf((BaseSequentialStream *) &SD1,"\n\r - ");
    }
    chprintf((BaseSequentialStream *) &SD1,"** END **\n\r");
}

/**
 * Configure the NRF into ShockBurst without autoretry. Set device as PTX.
 */
void nrf_configure_sb_tx(void) {

    // Set address for TX and receive on P0
    nrf_reg_buf addr;

    addr.data[0] = 1;
    addr.data[1] = 2;
    addr.data[2] = 3;
    addr.data[3] = 4;
    addr.data[4] = 5;

    nrf_preset_sb(NRF_MODE_PTX, 40, 1, &addr);
    
    // Wait for radio to power up
    chThdSleepMilliseconds(300);
}

/*
 * Application entry point.
 */
int main(void) {
  unsigned i;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * SPI2 I/O pins setup.
   */
  palSetPadMode(GPIOB, 13, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* New SCK.     */
  palSetPadMode(GPIOB, 14, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* New MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_STM32_ALTERNATE_PUSHPULL);     /* New MOSI.    */
  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL);              /* New CS.      */

  palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);      /* USART1 TX.       */
  palSetPadMode(GPIOA, 10, PAL_MODE_INPUT); 

  sdStart(&SD1, NULL);

  spiAcquireBus(&SPID2);              /* Acquire ownership of the bus.    */
  spiStart(&SPID2, &ls_spicfg);       /* Setup transfer parameters.       */

//  nrf_init();
  nrf_configure_sb_tx();
  nrf_dump_regs(&nrf_reg_def);

// chThdCreateStatic(blinker_wa, sizeof(blinker_wa),
//                    NORMALPRIO-1, blinker, NULL);
  static nrf_payload   p;

  int s;

  // set payload size according to the payload size configured for the RX channel
  p.size = 1;
  p.data[0] = 0;

  while (TRUE) {
    chprintf((BaseSequentialStream *) &SD1, "Sending payload: %x ", p.data[0]);
    s = nrf_send_blocking(&p);
    chprintf((BaseSequentialStream *) &SD1, " - done; bytes send: %u\n\r", s);

    chThdSleepMilliseconds(100);

    p.data[0]++;
  }

  return 0;
}
