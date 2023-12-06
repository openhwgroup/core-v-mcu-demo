#include <app/include/iot_task.h>
#include "hal/include/hal_udma_i2cm_reg_defs.h"
#include "drivers/include/udma_uart_driver.h"
#include "hal/include/hal_apb_soc_ctrl_regs.h"
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "libs/cli/include/cli.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/unistd.h>

static char thing_name[37] = {0};
static char location[37] = {0};
static uint8_t i2c_buffer[256] = {0};

static int thing = 0;

void set_location()
{
	if (strcmp("12d0cd37-803c-498a-983e-94f753731bd9", thing_name) == 0)
		strcpy(location, "Xian");
	else if (strcmp("9fe9bfa4-f5aa-4ecf-99e7-d8371e77f5b3", thing_name) == 0)
		strcpy(location, "Austin");
	else if (strcmp("d5cc86db-39be-4b15-bb8c-b61c5f13adc4", thing_name) == 0)
		strcpy(location, "Montreal");
	else if (strcmp("7869566b-645f-44bd-ac8f-207168e01c95", thing_name) == 0)
		strcpy(location, "Toronto");
	else if (strcmp("ed60a105-57cd-4960-ba9a-76878974ef89", thing_name) == 0)
		strcpy(location, "San Diego");
	else if (strcmp("d7d1408e-09c9-47cf-a55f-adf9292f7684", thing_name) == 0)
		strcpy(location, "Nurmberg");
	else if (strcmp("c360e34d-c356-4e24-af50-7ccd09866ead", thing_name) == 0)
		strcpy(location, "Nurmberg");
	else if (strcmp("67870924-1185-4acb-b8eb-3db3967be4ca", thing_name) == 0)
		strcpy(location, "Zurich");
	else if (strcmp("380ede96-0571-4f7e-b329-ad69ae3ef69e", thing_name) == 0)
		strcpy(location, "Paris");
	else if (strcmp("92bb486e-0a98-40a5-928a-9263ae5d26d2", thing_name) == 0)
		strcpy(location, "Rome");
	else if (strcmp("23d254d4-bc69-475f-8b7f-37fdddc8244a", thing_name) == 0)
		strcpy(location, "London");
	else if (strcmp("3e3c131d-6354-46d8-93c0-2bd687203258", thing_name) == 0)
		strcpy(location, "New York");
	else if (strcmp("3bcc439a-674e-4b49-aff6-1556558987ce", thing_name) == 0)
		strcpy(location, "Havana");
	else if (strcmp("5f14b489-95ae-455a-a8f2-a4e7a613dc8d", thing_name) == 0)
		strcpy(location, "Munich");
	else
		strcpy(location, "Unknown");
}

void set_location_dummy()
{
	if (thing == 0)
		strcpy(location, "Xian");
	else if (thing == 1)
		strcpy(location, "San Diego");
	else if (thing == 2)
		strcpy(location, "Ottawa");
	else if (thing == 3)
		strcpy(location, "Nurmberg");
	else if (thing == 4)
		strcpy(location, "Zurich");
	else if (thing == 5)
		strcpy(location, "Paris");
	else if (thing == 6)
		strcpy(location, "Rome");
	else if (thing == 7)
		strcpy(location, "London");
	else if (thing == 8)
		strcpy(location, "New York");
	else if (thing == 9)
		strcpy(location, "Havana");
	else if (thing == 10)
		strcpy(location, "Munich");
	else
		strcpy(location, "Unknown");
}

void busy_sleep(const unsigned int n)
{
	volatile unsigned int i;
	for (i = 0; i < n*1000000; i++);
}

void at_connect()
{
    char msg[32];
    char ok[] = "OK";

	strcpy(msg, "AT+CONNECT\n");
    while (1) {
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(msg), msg);
    	udma_uart_read_mod(1, sizeof(msg), msg);
    	if (strncmp(msg, ok, sizeof(ok)-1) == 0)
    		break;
    	strcpy(msg, "AT+CONNECT\n");
    	busy_sleep(10);
    }
	CLI_printf("ESP connected\r\n");
}

void at_getthingname()
{
    char msg[96];
    char ok[] = "OK";

	strcpy(msg, "AT+CONF? ThingName\n");
    while (strncmp(msg, ok, sizeof(ok)-1)) {
    	strcpy(msg, "AT+CONF? ThingName\n");
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(msg), msg);
    	udma_uart_read_mod(1, sizeof(msg), msg);
    }
    strncpy(thing_name, &msg[3], 36);
    thing_name[36] = '\0';
	CLI_printf("Thing name: %s\r\n", thing_name);

}

void at_check()
{
    char msg[32];
    char ok[] = "OK\n";

	strcpy(msg, "AT\n");
    while (strncmp(msg, ok, sizeof(ok)-1)) {
    	strcpy(msg, "AT\n");
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(msg), msg);
    	udma_uart_read_mod(1, sizeof(msg), msg);
    }
	CLI_printf("AT check done\r\n");
}

void rx_byte( int c )
{
    static const char _backspace[] = "\b \b";
    int x;
    static int last_was_cr;

    /* handle CR/LF
    * and CR
    * and LF
    * As a terminator
    */
    if( last_was_cr && (c =='\n') ){
        /* make CR/LF look like a single \n */
        last_was_cr = 0;
        return;
    }

    if( c == '\r' ){
        /* map to newline */
        c = '\n';
        last_was_cr = 1;
    } else {
        last_was_cr = 0;
    }

    /* we commonly need the len so get it always */
    /* todo: Future, would be nice to have up/dn arrow support cmd history */
    /* Todo: Would be nice if we had fancy editing of the command line */

    x = strlen( CLI_common.cmdline );
    switch( c ){
    case 0x03: /* Control-C */
        /* Acknowedge the key, also see the Simualted ^C in command dispatch  */
        CLI_printf(" **^C**\n");
        memset( (void *)(&(CLI_common.cmdline[0])), 0, sizeof(CLI_common.cmdline) );
        CLI_cmd_stack_clear();
        break;
    case 0x1b: /* ESCAPE key erases all bytes */
      /* note: this is different then ESC as a prefix to a CSI sequence (arrow key)
       * See arrow key decoding in CLI_getkey() code
       */
        if( x == 0 ){
            /* beep */
            CLI_beep();
            break;
        }
        /* back over */
        while( x ){
            CLI_puts_no_nl(_backspace);
            x--;
        }
        memset( CLI_common.cmdline, 0, sizeof(CLI_common.cmdline) );
        break;
    case '\b': /* backspace */
    case 0x7F: /* delete */
        if( x == 0 ){
            CLI_beep();
            break;
        }
        CLI_puts_no_nl(_backspace);
        x--;
        CLI_common.cmdline[x] = 0;
        break;
    case '\n':
        CLI_putc('\n');
        break;
    case '\t':
        /* future: add command completion */
        /* treat as space */
        c = ' ';
        /* fallthrough */
    default:
        /* if NOT in ASCII range (function keys, arrow keys, etc)*/
        if( (c < 0x20) || (c >= 0x7f) ){
            CLI_beep();
            break;
        }
        if( x >= (sizeof(CLI_common.cmdline)-1) ){
	    /* too much */
	    CLI_beep();
        } else {
   	    /* Append */
            CLI_common.cmdline[x+0] = c;
            CLI_common.cmdline[x+1] = 0;
            /* echo */
            CLI_putc(c);
        }
        break;
    }
}

void get_string()
{
    int k;

    memset( (void *)(&CLI_common.cmdline[0]), 0, sizeof(CLI_common.cmdline) );

	while(1) {
		k = CLI_getkey( 10*1000 );
		if( k == EOF ){
			continue;
		}
		rx_byte( k );

		if (strlen( CLI_common.cmdline ) == 0)
			break;

		if(k == '\r' || k == '\n')
			break;
	}
}

void at_set_wifi()
{
    char msg[256];
    char resp[32];
    char ok[] = "OK\n";
	char endpoint[] = "AT+CONF EndPoint=a25slo9f5m9kt7-ats.iot.eu-west-1.amazonaws.com\n";
    char skip_confmode = 0;
    int timeout = 10;

    vTaskDelay(100);
    CLI_common.timestamps = 0;

    /*
    resp[0] = '\0';
    while (strncmp(resp, ok, sizeof(ok)-1)) {
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(endpoint), endpoint);
    	udma_uart_read_mod(1, sizeof(resp), resp);
    }
*/
	CLI_printf("Press a key to enter configuration mode...");
    while(timeout) {
    	CLI_printf("%d...", timeout);

    	skip_confmode = CLI_getkey( 2*1000 );

		if (skip_confmode != EOF) {

			timeout = 1;
			resp[0] = '\0';

			while (strncmp(resp, ok, sizeof(ok)-1)) {
				CLI_printf("\r\nSet SSID: ");
				get_string();
				if (strlen( CLI_common.cmdline ) == 0)
					break;
				sprintf(msg, "AT+CONF SSID=%s\n", CLI_common.cmdline);

				udma_uart_flush(1);
				udma_uart_writeraw(1, strlen(msg), msg);
				udma_uart_read_mod(1, sizeof(resp), resp);
				if (strncmp(resp, ok, sizeof(ok)-1))
					CLI_printf("Error setting SSID\r\n");
				resp[0] = '\0';
			}

			while (strncmp(resp, ok, sizeof(ok)-1)) {
				CLI_printf("Set Password: ");
				get_string();
				if (strlen( CLI_common.cmdline ) == 0)
					break;
				sprintf(msg, "AT+CONF Passphrase=%s\n", CLI_common.cmdline);

				udma_uart_flush(1);
				udma_uart_writeraw(1, strlen(msg), msg);
				udma_uart_read_mod(1, sizeof(resp), resp);
				if (strncmp(resp, ok, sizeof(ok)-1))
					CLI_printf("Error setting password\r\n");
				resp[0] = '\0';
			}
		}
		timeout--;
    }
	CLI_printf("\r\n");

    CLI_printf("WiFi configured\r\n");
}

void at_set_topic()
{
    char root_topic[] = "AT+CONF Topicroot=test\n";
    char topic1[] = "AT+CONF Topic1=data\n";
    char ok[] = "OK\n";
    char resp[32];

    resp[0] = '\0';

    while (strncmp(resp, ok, sizeof(ok)-1)) {
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(root_topic), root_topic);
    	udma_uart_read_mod(1, sizeof(resp), resp);
    }

    resp[0] = '\0';

    while (strncmp(resp, ok, sizeof(ok)-1)) {
    	udma_uart_flush(1);
    	udma_uart_writeraw(1, strlen(topic1), topic1);
    	udma_uart_read_mod(1, sizeof(resp), resp);
    }

	CLI_printf("Topic configured\r\n");
}

void at_send_measure()
{
    char msg[256];
    char ok[] = "OK\n";
    char resp[32];

    int temp;
	int tempi;
	int tempd;

	udma_i2cm_read(1, 0x96, 0x00, 2, i2c_buffer, false);
	temp = (i2c_buffer[0] << 8) + i2c_buffer[1];
	tempi = (temp >> 7);
	tempd = (temp & 0x7F) * 625;
	CLI_printf("The temperature here (%s) is %d.%d C\r\n", location, tempi, tempd);

	sprintf(msg, "AT+SEND1 { \"id\" : \"%s\", \"temp\" : %d.%d }\n", location, tempi, tempd);
    resp[0] = '\0';

	udma_uart_flush(1);
	udma_uart_writeraw(1, strlen(msg), msg);
	udma_uart_read_mod(1, sizeof(resp), resp);
	if (strncmp(resp, ok, sizeof(ok)-1))
		CLI_printf("Error sending measurement\r\n");
}

void iot_app( void *pParameter )
{
	(void)pParameter;

	while (1) {
		at_check();
		at_getthingname();
		set_location();
		at_set_wifi();
		at_set_topic();
		while (1) {
			at_connect();
			at_send_measure();
			busy_sleep(10);
//			thing = (thing + 1) % 11;
//			set_location_dummy();
		}
	}
}
