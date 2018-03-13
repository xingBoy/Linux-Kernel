/*ch
 *	VKIC Ltd.
 *	By  David Tech
 *	DEMO Version :1.01 Data:2014-12-25
 *
 *
 *
 * 	1. compiler warnings all changes
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/freezer.h>
#include <linux/spi/spi.h>
#include <linux/timer.h>

#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <asm/mach/map.h>
#include <asm/irq.h>
#include <asm/io.h>
#include "wk2124A.h"

MODULE_LICENSE("Dual BSD/GPL");

#define SPI_BUFSIZ      max(32, SMP_CACHE_BYTES)
#define CONFIG_DEVFS_FS

#define WK2XXX_PAGE1        1
#define WK2XXX_PAGE0        0

#define WK2XXX_STATUS_PE    1
#define WK2XXX_STATUS_FE    2
#define WK2XXX_STATUS_BRK   4
#define WK2XXX_STATUS_OE    8

static DEFINE_MUTEX(wk2124s_lock);                /* race on probe */

static DEFINE_MUTEX(wk2124s_work_lock);                /* work on probe */

struct wk2xxx_port
{
    struct uart_port port;
    struct spi_device *spi_wk;
    spinlock_t conf_lock;
    struct workqueue_struct *workqueue;
    struct work_struct work;
    int suspending;
    void (*wk2xxx_hw_suspend) (int suspend);
    int tx_done;

    int force_end_work;
    int irq;
    int minor;
    int tx_empty;
    int tx_empty_flag;

    int start_tx_flag;
    int stop_tx_flag;
    int stop_rx_flag;
    int irq_flag;
    int conf_flag;

    int tx_empty_fail;
    int start_tx_fail;
    int stop_tx_fail;
    int stop_rx_fail;
    int irq_fail;
    int conf_fail;

    uint8_t new_lcr;
    uint8_t new_scr;
    uint8_t new_baud1;
    uint8_t new_baud0;
    uint8_t new_pres;
};

static struct wk2xxx_port wk2xxxs[NR_PORTS]; /* the chips */

static int wk2xxx_read_reg(struct spi_device *spi, uint8_t port, uint8_t reg, uint8_t *dat)
{
    struct spi_message msg;
    uint8_t buf_wdat[2];
    uint8_t buf_rdat[2];
    int status;

    struct spi_transfer index_xfer = {
        .len            = 2,
        .cs_change      = 1,
    };

    status =0;
    spi_message_init(&msg);
    buf_wdat[0] = 0x40|(((port-1)<<4)|reg);
    buf_wdat[1] = 0x00;
    buf_rdat[0] = 0x00;
    buf_rdat[1] = 0x00;
    index_xfer.tx_buf = buf_wdat;
    index_xfer.rx_buf =(void *) buf_rdat;
    spi_message_add_tail(&index_xfer, &msg);
    status = spi_sync(spi, &msg);
    udelay(5);
    if(status)
    {
        return status;
    }
    *dat = buf_rdat[1];
    return 0;
}

static int wk2xxx_write_reg(struct spi_device *spi, uint8_t port, uint8_t reg, uint8_t dat)
{
    struct spi_message msg;
    uint8_t buf_reg[2];
    int status;

    struct spi_transfer index_xfer = {
        .len            = 2,
        .cs_change      = 1,
    };
    spi_message_init(&msg);
    /* register index */
    buf_reg[0] = ((port-1)<<4)|reg;
    buf_reg[1] = dat;
    index_xfer.tx_buf = buf_reg;
    spi_message_add_tail(&index_xfer, &msg);
    status = spi_sync(spi, &msg);
    udelay(5);
    if(status)
    {
        return status;
    }

    return status;
}

static void wk2xxxirq_app(struct uart_port *port);
static void conf_wk2xxx_subport(struct uart_port *port);
static void wk2xxx_work(struct work_struct *w);
static void wk2xxx_stop_tx(struct uart_port *port);
static u_int wk2xxx_tx_empty(struct uart_port *port);

static int wk2xxx_dowork(struct wk2xxx_port *s)
{
    if (!s->force_end_work && !work_pending(&s->work) && !freezing(current) && !s->suspending)
    {
        queue_work(s->workqueue, &s->work);
        return 1;
    }
    else
    {
       return 0;
    }
}

static void wk2xxx_work(struct work_struct *w)
{
    struct wk2xxx_port *s = container_of(w, struct wk2xxx_port, work);
    uint8_t rx;

    int work_tx_empty_flag = 0;
    int work_start_tx_flag;

    int work_stop_rx_flag;
    int work_stop_tx_flag = 0;

    int work_irq_flag;
    int work_conf_flag;
    do {
        mutex_lock(&wk2124s_work_lock);

        work_start_tx_flag = s->start_tx_flag;
        if(work_start_tx_flag)
            s->start_tx_flag = 0;

        work_stop_rx_flag = s->stop_rx_flag;

        if(work_stop_rx_flag)
            s->stop_rx_flag = 0;

        work_conf_flag = s->conf_flag;

        if(work_conf_flag)
            s->conf_flag = 0;

        work_irq_flag = s->irq_flag;
        if(work_irq_flag)
            s->irq_flag = 0;

        mutex_unlock(&wk2124s_work_lock);

        if(work_conf_flag)
        {
            conf_wk2xxx_subport(&s->port);
        }

        if(work_start_tx_flag)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
            rx |= WK2XXX_TFTRIG_IEN;
            wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
        }

        if(work_stop_rx_flag)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);

        rx &= ~WK2XXX_RFTRIG_IEN;
        rx &= ~WK2XXX_RXOUT_IEN;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, &rx);

        rx &= ~WK2XXX_RFTRIG_INT;
        rx &= ~WK2XXX_RXOVT_INT;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, rx);
        }

        if(work_irq_flag)
        {
            wk2xxxirq_app(&s->port);
            s->irq_fail = 1;
        }
    }while (!s->force_end_work && !freezing(current) && \
        (work_irq_flag || work_stop_rx_flag || \
         work_stop_tx_flag || work_tx_empty_flag || work_conf_flag));

    if(s->conf_fail)
    {
        conf_wk2xxx_subport(&s->port);
        s->conf_fail =0;
    }

    if(s->start_tx_fail)
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
        rx |= WK2XXX_TFTRIG_IEN;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
        s->start_tx_fail =0;
    }

    if(s->stop_rx_fail)
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
        rx &=~WK2XXX_RFTRIG_IEN;
        rx &=~WK2XXX_RXOUT_IEN;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);

        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, &rx);
        rx &= ~WK2XXX_RFTRIG_INT;
        rx &= ~WK2XXX_RXOVT_INT;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, rx);
        s->stop_rx_fail =0;
    }

    if(s->irq_fail)
    {
        s->irq_fail = 0;
        enable_irq(s->port.irq);
    }
}

static void wk2xxx_rx_chars(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    uint8_t fsr, lsr, dat[1];
    unsigned int ch, flg, ignored=0, status = 0, rx_count=0;

    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, WK2XXX_PAGE0);
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_LSR, dat);
    lsr = dat[0];

    while (fsr& WK2XXX_RDAT)
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, dat);
        ch = (int)dat[0];
        s->port.icount.rx++;
        flg = TTY_NORMAL;
        if (lsr&(WK2XXX_OE |WK2XXX_FE|WK2XXX_PE|WK2XXX_BI))
        {
            if (lsr & WK2XXX_PE)
            {
                s->port.icount.parity++;
                status |= WK2XXX_STATUS_PE;
                flg = TTY_PARITY;
            }
            if (lsr & WK2XXX_FE)
            {
                s->port.icount.frame++;
                status |= WK2XXX_STATUS_FE;
                flg = TTY_FRAME;
            }
            if (lsr & WK2XXX_OE)
            {
                s->port.icount.overrun++;
                status |= WK2XXX_STATUS_OE;
                flg = TTY_OVERRUN;
            }
            if(lsr&fsr & WK2XXX_BI)
            {
                s->port.icount.brk++;
                status |= WK2XXX_STATUS_BRK;
                flg = TTY_BREAK;
            }

            if (++ignored > 100)
                goto out;

            goto ignore_char;
        }

error_return:
        if (uart_handle_sysrq_char(&s->port, ch))
            goto ignore_char;

        uart_insert_char(&s->port, status, WK2XXX_STATUS_OE, ch, flg);

        rx_count++;

        if ((rx_count >= 8 ) && (s->port.state->port.tty != NULL))
        {
            tty_flip_buffer_push(s->port.state->port.tty);
            rx_count = 0;
        }
ignore_char:
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
        fsr = dat[0];
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_LSR, dat);
        lsr = dat[0];
    }

out:
    if((rx_count > 0)&&(s->port.state->port.tty != NULL))
    {
        tty_flip_buffer_push(s->port.state->port.tty);
    }

    return;
#ifdef SUPPORT_SYSRQ
    s->port.state->sysrq = 0;
#endif
    goto error_return;
}

static void wk2xxx_tx_chars(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    uint8_t fsr, dat[1];
    int count;
    if (s->port.x_char)
    {
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, s->port.x_char);
        s->port.icount.tx++;
        s->port.x_char = 0;
        goto out;
    }

    if(uart_circ_empty(&s->port.state->xmit) || uart_tx_stopped(&s->port))
    {
        goto out;
    }

    /*
     * Tried using FIFO (not checking TNF) for fifo fill:
     * still had the '1 bytes repeated' problem.
     */
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];

    count = s->port.fifosize;
    do
    {
        if((fsr & WK2XXX_TFULL)|uart_circ_empty(&s->port.state->xmit))
            goto out;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, s->port.state->xmit.buf[s->port.state->xmit.tail]);
        s->port.state->xmit.tail = (s->port.state->xmit.tail + 1) & (UART_XMIT_SIZE - 1);
        s->port.icount.tx++;
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
        fsr = dat[0];
    }while(--count > 0);

out:wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];
    if((fsr&WK2XXX_TDAT)==0)
    {
        if (uart_circ_chars_pending(&s->port.state->xmit) < WAKEUP_CHARS)
            uart_write_wakeup(&s->port);

        if (uart_circ_empty(&s->port.state->xmit))
        {
            wk2xxx_stop_tx(&s->port);
        }
    }
}

static irqreturn_t wk2xxx_irq(int irq, void *dev_id)
{
    struct wk2xxx_port *s = dev_id;
    disable_irq_nosync(s->port.irq);

    if(wk2xxx_dowork(s))
    {
        s->irq_flag = 1;
    }
    else
    {
        s->irq_fail = 1;
    }
    return IRQ_HANDLED;
}

static void wk2xxxirq_app(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    unsigned int  pass_counter = 0;
    uint8_t sifr, gifr, sier, dat[1];

    uint8_t sifr0, sifr1, sifr2, sifr3, sier1, sier0, sier2, sier3;

    wk2xxx_read_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIFR, dat);
    gifr = dat[0];
    wk2xxx_write_reg(s->spi_wk, 1, WK2XXX_SPAGE, WK2XXX_PAGE0);
    wk2xxx_write_reg(s->spi_wk, 2, WK2XXX_SPAGE, WK2XXX_PAGE0);
    wk2xxx_write_reg(s->spi_wk, 3, WK2XXX_SPAGE, WK2XXX_PAGE0);
    wk2xxx_write_reg(s->spi_wk, 4, WK2XXX_SPAGE, WK2XXX_PAGE0);

    wk2xxx_read_reg(s->spi_wk, 1, WK2XXX_SIFR, &sifr0);
    wk2xxx_read_reg(s->spi_wk, 2, WK2XXX_SIFR, &sifr1);
    wk2xxx_read_reg(s->spi_wk, 3, WK2XXX_SIFR, &sifr2);
    wk2xxx_read_reg(s->spi_wk, 4, WK2XXX_SIFR, &sifr3);

    wk2xxx_read_reg(s->spi_wk, 1, WK2XXX_SIER, &sier0);
    wk2xxx_read_reg(s->spi_wk, 2, WK2XXX_SIER, &sier1);
    wk2xxx_read_reg(s->spi_wk, 3, WK2XXX_SIER, &sier2);
    wk2xxx_read_reg(s->spi_wk, 4, WK2XXX_SIER, &sier3);

    switch(s->port.iobase)
    {
        case 1 :
            if(!(gifr & WK2XXX_UT1INT))
            {
                return;
            }
            break;
        case 2 :
            if(!(gifr & WK2XXX_UT2INT))
            {
                return;
            }
            break;
        case 3 :
            if(!(gifr & WK2XXX_UT3INT))
            {
                return;
            }
            break;
        case 4 :
            if(!(gifr & WK2XXX_UT4INT))
            {
                return;
            }
            break;
        default:
            break;
    }

    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, dat);
    sifr = dat[0];
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, dat);
    sier = dat[0];
    do {
        if ((sifr&WK2XXX_RFTRIG_INT)||(sifr&WK2XXX_RXOVT_INT))
        {
            wk2xxx_rx_chars(&s->port);
        }

        if ((sifr & WK2XXX_TFTRIG_INT)&&(sier & WK2XXX_TFTRIG_IEN ))
        {
            wk2xxx_tx_chars(&s->port);
            return;
        }
        if (pass_counter++ > WK2XXX_ISR_PASS_LIMIT)
            break;
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, dat);
        sifr = dat[0];
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, dat);
        sier = dat[0];
    } while ((sifr&WK2XXX_RXOVT_INT) || (sifr & WK2XXX_RFTRIG_INT) ||
             ((sifr & WK2XXX_TFTRIG_INT)&&(sier & WK2XXX_TFTRIG_IEN)));
}

/*
 *   Return TIOCSER_TEMT when transmitter is not busy.
 */
static u_int wk2xxx_tx_empty(struct uart_port *port)
{
    uint8_t rx;
    struct wk2xxx_port *s = NULL;

    mutex_lock(&wk2124s_lock);

    s = container_of(port, struct wk2xxx_port, port);

    if(!(s->tx_empty_flag || s->tx_empty_fail))
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, &rx);
        s->tx_empty = (rx & WK2XXX_TDAT)<=0;
        if(s->tx_empty)
        {
            s->tx_empty_flag =0;
            s->tx_empty_fail=0;
        }
        else
        {
            s->tx_empty_fail=0;
            s->tx_empty_flag =0;
        }
    }

    mutex_unlock(&wk2124s_lock);
    return s->tx_empty;
}

static void wk2xxx_set_mctrl(struct uart_port *port, u_int mctrl)
{
}
static u_int wk2xxx_get_mctrl(struct uart_port *port)
{
    return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

/*
 *  interrupts disabled on entry
 */
static void wk2xxx_stop_tx(struct uart_port *port)
{
    uint8_t rx;
    struct wk2xxx_port *s = NULL;

    mutex_lock(&wk2124s_lock);

    s = container_of(port, struct wk2xxx_port, port);

    if(!(s->stop_tx_flag || s->stop_tx_fail))
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, (uint8_t *)&rx);
        s->stop_tx_fail = (rx & WK2XXX_TFTRIG_IEN) > 0;
        if(s->stop_tx_fail)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
            rx &=~WK2XXX_TFTRIG_IEN;
            wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, &rx);
            rx &= ~WK2XXX_TFTRIG_INT;
            wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, rx);
            s->stop_tx_fail =0;
            s->stop_tx_flag=0;
        }
        else
        {
            s->stop_tx_fail =0;
            s->stop_tx_flag=0;
        }
    }
    mutex_unlock(&wk2124s_lock);
}

/*
 *  * interrupts may not be disabled on entry
 */
static void wk2xxx_start_tx(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);

    if(!(s->start_tx_flag||s->start_tx_fail))
    {
        if(wk2xxx_dowork(s))
        {
            s->start_tx_flag = 1;
        }
        else
        {
            s->start_tx_fail = 1;
        }
    }
}

/*
 *  * Interrupts enabled
 */

static void wk2xxx_stop_rx(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);

    if(!(s->stop_rx_flag ||s->stop_rx_fail ))
    {
        if(wk2xxx_dowork(s))
        {
            s->stop_rx_flag = 1;
        }
        else
        {
            s->stop_rx_fail = 1;
        }
    }
}

/*
 *  * No modem control lines
 *   */
static void wk2xxx_enable_ms(struct uart_port *port)
{
}

/*
 *  * Interrupts always disabled.
 */
static void wk2xxx_break_ctl(struct uart_port *port, int break_state)
{
}

static int wk2xxx_startup(struct uart_port *port)
{
    uint8_t gena, grst, gier, sier, scr, dat[1];
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    char b[64];

    if (s->suspending)
        return 0;

    s->force_end_work = 0;
    sprintf(b, "wk2xxx-%d", (uint8_t)s->port.iobase);
    s->workqueue = create_workqueue(b);

    if (!s->workqueue)
    {
        dev_warn(&s->spi_wk->dev, "cannot create workqueue\n");
        return -EBUSY;
    }

    INIT_WORK(&s->work, wk2xxx_work);

    if (s->wk2xxx_hw_suspend)
        s->wk2xxx_hw_suspend(0);

    wk2xxx_read_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GENA, dat);
    gena=dat[0];
    switch (s->port.iobase)
    {
        case 1:
            gena |= WK2XXX_UT1EN;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GENA, gena);
            break;
        case 2:
            gena |= WK2XXX_UT2EN;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GENA, gena);
            break;
        case 3:
            gena |= WK2XXX_UT3EN;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GENA, gena);
            break;
        case 4:
            gena |= WK2XXX_UT4EN;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GENA, gena);
            break;
        default:
            printk(KERN_ALERT ":con_wk2xxx_subport bad iobase %d\n", (uint8_t)s->port.iobase);
    }

    wk2xxx_read_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GRST, dat);
    grst = dat[0];
    switch (s->port.iobase)
    {
        case 1:
            grst |= WK2XXX_UT1RST;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GRST, grst);
            break;
        case 2:
            grst |= WK2XXX_UT2RST;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GRST, grst);
            break;
        case 3:
            grst |= WK2XXX_UT3RST;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GRST, grst);
            break;
        case 4:
            grst |= WK2XXX_UT4RST;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GRST, grst);
            break;
        default:
            printk(KERN_ALERT ":con_wk2xxx_subport bad iobase %d\n", (uint8_t)s->port.iobase);
    }

    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, dat);
    sier = dat[0];
	sier &= ~WK2XXX_TFTRIG_IEN;
	sier |= WK2XXX_RFTRIG_IEN;
	sier |= WK2XXX_RXOUT_IEN;
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, sier);

    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, dat);
    scr = dat[0] | WK2XXX_TXEN|WK2XXX_RXEN;
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr);

    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FCR, 0x0f);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FCR, 0x0c);
    wk2xxx_read_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIER, dat);
    gier = dat[0];

    switch (s->port.iobase)
    {
        case 1:
            gier |= WK2XXX_UT1IE;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIER, gier);
            break;
        case 2:
            gier |= WK2XXX_UT2IE;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIER, gier);
            break;
        case 3:
            gier |= WK2XXX_UT3IE;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIER, gier);
            break;
        case 4:
            gier |= WK2XXX_UT4IE;
            wk2xxx_write_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIER, gier);
            break;
        default:
            printk(KERN_ALERT ": bad iobase %d\n", (uint8_t)s->port.iobase);
    }

    if (s->wk2xxx_hw_suspend)
        s->wk2xxx_hw_suspend(0);
	msleep(50);

	uart_circ_clear(&s->port.state->xmit);
    wk2xxx_enable_ms(&s->port);
	if(request_irq(s->port.irq, wk2xxx_irq, IRQF_SHARED|IRQF_TRIGGER_LOW, "wk2124A", s) < 0)
    {
       dev_warn(&s->spi_wk->dev, "cannot allocate irq %d\n", s->irq);
       s->port.irq = 0;
       destroy_workqueue(s->workqueue);
       s->workqueue = NULL;
       return -EBUSY;
    }
    udelay(100);
	udelay(100);

    return 0;
}

static void wk2xxx_shutdown(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
	if (s->suspending)
		return;
	s->force_end_work = 1;

	if (s->workqueue)
	{
		flush_workqueue(s->workqueue);
		destroy_workqueue(s->workqueue);
		s->workqueue = NULL;
	}

	if (s->port.irq)
	{
        //disable_irq_nosync(s->port.irq);
		free_irq(s->port.irq, s);
	}

    return;
}

static void conf_wk2xxx_subport(struct uart_port *port)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
	uint8_t old_sier, lcr, scr, scr_ss, dat[1], baud0_ss, baud1_ss, pres_ss;

	lcr = s->new_lcr;
	scr_ss = s->new_scr;
	baud0_ss = s->new_baud0;
	baud1_ss = s->new_baud1;
	pres_ss = s->new_pres;
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, dat);
    old_sier = dat[0];
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, old_sier&(~(WK2XXX_TFTRIG_IEN | WK2XXX_RFTRIG_IEN | WK2XXX_RXOUT_IEN)));
    do{
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    } while (dat[0] & WK2XXX_TBUSY);

    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, dat);
    scr = dat[0];
    scr &= 0x0f;
    scr |= scr_ss;

    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr&(~(WK2XXX_RXEN|WK2XXX_TXEN)));
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_LCR, lcr);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, old_sier);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, 1);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD0, baud0_ss);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD1, baud1_ss);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_PRES, pres_ss);
	wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, 0);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr|(WK2XXX_RXEN|WK2XXX_TXEN));
}

static void wk2xxx_termios( struct uart_port *port, struct ktermios *termios,
		    struct ktermios *old)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
	int baud = 0;
	uint8_t lcr, scr = 0, baud1, baud0, pres;
	unsigned short cflag;
	unsigned short lflag;
	cflag = termios->c_cflag;
	lflag = termios->c_lflag;
	baud1 = 0;
	baud0 = 0;
	pres = 0;
	baud = tty_termios_baud_rate(termios);

#if 0
	switch (baud) {
        case 600:
            baud1 = 0x47;
            baud0 = 0xf;
            pres = 0;
            break;
        case 1200:
            baud1 = 0x23;
            baud0 = 0xF;
            pres = 0;
            break;
        case 2400:
            baud1 = 0x11;
            baud0 = 0xf;
            pres = 0;
            break;
        case 4800:
            baud1 = 0x00;
            baud0 = 0x8f;
            pres = 0;
            break;
        case 9600:
            baud1 = 0x00;
            baud0 = 0x47;
            pres = 0;
            break;
        case 19200:
            baud1 = 0x00;
            baud0 = 0x23;
            pres = 0;
            break;
        case 38400:
            baud1 = 0x00;
            baud0 = 0x11;
            pres = 0;
            break;
        case 76800:
            baud1 = 0x00;
            baud0 = 0x08;
            pres = 0;
            break;
        case 1800:
            baud1 = 0x01;
            baud0 = 0x7f;
            pres = 0;
            break;
        case 3600:
            baud1 = 0x00;
            baud0 = 0xbf;
            pres = 0;
            break;
        case 7200:
            baud1 = 0x00;
            baud0 = 0x5f;
            pres = 0;
            break;
        case 14400:
            baud1 = 0x00;
            baud0 = 0x2f;
            pres = 0;
            break;
        case 28800:
            baud1 = 0x00;
            baud0 = 0x17;
            pres = 0;
            break;
        case 57600:
            baud1 = 0x00;
            baud0 = 0x0b;
            pres = 0;
            break;
        case 115200:
            baud1 = 0x00;
            baud0 = 0x05;
            pres = 0;
            break;
        case 230400:
            baud1 = 0x00;
            baud0 = 0x02;
            pres = 0;
            break;
        default:
            baud1 = 0x00;
            baud0 = 0x00;
            pres = 0;
	}
#endif
	switch (baud) {
        case 600:
            baud1 = 0x40;
            baud0 = 0x7f;
            pres = 0;
            break;
        case 1200:
            baud1 = 0x20;
            baud0 = 0x3F;
            pres = 0;
            break;
        case 2400:
            baud1 = 0x10;
            baud0 = 0x1f;
            pres = 0;
            break;
        case 4800:
            baud1 = 0x00;
            baud0 = 0x8f;
            pres = 0;
            break;
        case 9600:
            baud1 = 0x00;
            baud0 = 0x47;
            pres = 0;
            break;
        case 19200:
            baud1 = 0x00;
            baud0 = 0x23;
            pres = 0;
            break;
        case 38400:
            baud1 = 0x00;
            baud0 = 0x11;
            pres = 0;
            break;
        case 76800:
            baud1 = 0x00;
            baud0 = 0x05;
            pres = 0;
            break;
        case 1800:
            baud1 = 0x01;
            baud0 = 0x7f;
            pres = 0;
            break;
        case 3600:
            baud1 = 0x00;
            baud0 = 0xbf;
            pres = 0;
            break;
        case 7200:
            baud1 = 0x00;
            baud0 = 0x5f;
            pres = 0;
            break;
        case 14400:
            baud1 = 0x00;
            baud0 = 0x2f;
            pres = 0;
            break;
        case 28800:
            baud1 = 0x00;
            baud0 = 0x17;
            pres = 0;
            break;
        case 57600:
            baud1 = 0x00;
            baud0 = 0x0b;
            pres = 0;
            break;
        case 115200:
            baud1 = 0x00;
            baud0 = 0x05;
            pres = 0;
            break;
        case 230400:
            baud1 = 0x00;
            baud0 = 0x02;
            pres = 0;
            break;
        default:
            baud1 = 0x00;
            baud0 = 0x00;
            pres = 0;
	}

	tty_termios_encode_baud_rate(termios, baud, baud);

	lcr = 0;
    if (cflag & CSTOPB)
        lcr|=WK2XXX_STPL;
    else
        lcr&=~WK2XXX_STPL;

    if (cflag & PARENB)
    {
        lcr|=WK2XXX_PAEN;
        if (!(cflag & PARODD))
        {
            lcr |= WK2XXX_PAM0;
            lcr &= ~WK2XXX_PAM0;
        }
        else
        {
            lcr |= WK2XXX_PAM1;
            lcr &= ~WK2XXX_PAM0;
        }
    }
    else
    {
        lcr&=~WK2XXX_PAEN;
    }

	s->new_baud1=baud1;
	s->new_baud0=baud0;
	s->new_pres=pres;

	s->new_lcr = lcr;
	s->new_scr = scr;

	if(!(s->conf_flag|| s->conf_fail))
	{
        if(wk2xxx_dowork(s))
        {
            s->conf_flag =1;
        }
        else
        {
            s->conf_fail =1;
        }
	}
	return ;
}

static const char *wk2xxx_type(struct uart_port *port)
{
    return port->type == PORT_WK2XXX ? "wk2xxx" : NULL;
}

/*
* Release the memory region(s) being used by 'port'.
*/
static void wk2xxx_release_port(struct uart_port *port)
{
    printk(KERN_ALERT "wk2xxx_release_port\n");
}

/*
* Request the memory region(s) being used by 'port'.
*/
static int wk2xxx_request_port(struct uart_port *port)
{
    printk(KERN_ALERT "wk2xxx_request_port\n");
    return 0;
}

/*
* Configure/autoconfigure the port*/
static void wk2xxx_config_port(struct uart_port *port, int flags)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);

    if (flags & UART_CONFIG_TYPE && wk2xxx_request_port(port) == 0)
        s->port.type = PORT_WK2XXX;
}

/*
* Verify the new serial_struct (for TIOCSSERIAL).
* The only change we allow are to the flags and type, and
* even then only between PORT_vk32xx and PORT_UNKNOWN
*/
static int wk2xxx_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    int ret = 0;
    if (ser->type != PORT_UNKNOWN && ser->type != PORT_WK2XXX)
        ret = -EINVAL;
    if (port->irq != ser->irq)
        ret = -EINVAL;
    if (ser->io_type != SERIAL_IO_PORT)
        ret = -EINVAL;
    if (port->iobase != ser->port)
        ret = -EINVAL;
    if (ser->hub6 != 0)
        ret = -EINVAL;

    return ret;
}

static struct uart_ops wk2xxx_pops = {
    tx_empty:       wk2xxx_tx_empty,
    set_mctrl:      wk2xxx_set_mctrl,
    get_mctrl:      wk2xxx_get_mctrl,
    stop_tx:        wk2xxx_stop_tx,
    start_tx:       wk2xxx_start_tx,
    stop_rx:        wk2xxx_stop_rx,
    enable_ms:      wk2xxx_enable_ms,
    break_ctl:      wk2xxx_break_ctl,
    startup:        wk2xxx_startup,
    shutdown:       wk2xxx_shutdown,
    set_termios:    wk2xxx_termios,
    type:           wk2xxx_type,
    release_port:   wk2xxx_release_port,
    request_port:   wk2xxx_request_port,
    config_port:    wk2xxx_config_port,
    verify_port:    wk2xxx_verify_port,
};

static struct uart_driver wk2124_uart_driver = {
    owner:          THIS_MODULE,
    major:        	SERIAL_WK2124A_MAJOR,
#ifdef CONFIG_DEVFS_FS
    driver_name:    "ttySWZA",
    dev_name:       "ttysWZA",
#else
    driver_name:    "ttySWZA",
    dev_name:       "ttysWZA",
#endif
    minor:          MINOR_START,
    nr:             NR_PORTS,
    cons:           NULL
};

static int uart_driver_registered;

static int __devinit wk2124A_probe(struct spi_device *spi)
{
	uint8_t i;
    int status;
	static volatile unsigned long *gpbcon_addr;
	#define GPBCONWWWWW 0xE020004C

	i = 0;

	gpbcon_addr = ioremap(GPBCONWWWWW, 4);
    status = *gpbcon_addr;
    printk(KERN_ERR "GPB_DRV...........= 0x%X\n", status);
	*gpbcon_addr |= 0xaa00;
    status = *gpbcon_addr;

    printk(KERN_ERR "GPB_DRV...........= 0x%X\n", status);

    printk(KERN_ALERT "wk2124_probe wk2124\n");

	mutex_lock(&wk2124s_lock);

	if(!uart_driver_registered)
	{
		uart_driver_registered = 1;
        status = uart_register_driver(&wk2124_uart_driver);
        if (status)
       	{
        	printk(KERN_ERR "Couldn't register wk2124 uart driver\n");
        	mutex_unlock(&wk2124s_lock);
        	return status;
		}
    }
    printk(KERN_ALERT "wk2xxx_serial_init()\n");

	for (i = 0; i < NR_PORTS; i++)
	{
		struct wk2xxx_port *s = &wk2xxxs[i];
		s->tx_done = 0;
		s->spi_wk    = spi;
		s->port.line = i;
		s->port.ops = &wk2xxx_pops;
		s->port.uartclk = WK_CRASTAL_CLK;
		s->port.fifosize = 64;
		s->port.iobase = i+1;
		s->port.irq    = IRQ_WK2124A ;
        printk("wk2124 --------irq: %d\n", s->port.irq);
		s->port.iotype = SERIAL_IO_PORT;
		s->port.flags  = ASYNC_BOOT_AUTOCONF;
        status = uart_add_one_port(&wk2124_uart_driver, &s->port);
        if(status<0)
        {
            printk(KERN_ALERT "uart_add_one_port failed for line i:= %d with error %d\n", i, status);
		}
	}
	printk(KERN_ALERT "uart_add_one_port = 0x%d\n", status);

	mutex_unlock(&wk2124s_lock);

    return 0;
}

static int __devexit wk2124_remove(struct spi_device *spi)
{
	int i;

	mutex_lock(&wk2124s_lock);
    for(i =0; i < NR_PORTS; i++)
    {
        struct wk2xxx_port *s = &wk2xxxs[i];
	    uart_remove_one_port(&wk2124_uart_driver, &s->port);
	}
	printk(KERN_ALERT "removing wk2xxx driver\n");

	uart_unregister_driver(&wk2124_uart_driver);

	mutex_unlock(&wk2124s_lock);

	return 0;
}

static int wk2124_resume(struct spi_device *spi)
{
	printk(KERN_ALERT "resume wk2xxx");
	return 0;
}

static struct spi_driver wk2124A_driver = {
    .driver = {
        .name  = "wk2124A",
        .bus   = &spi_bus_type,
        .owner = THIS_MODULE,
    },

    .probe  = wk2124A_probe,
    .remove = __devexit_p(wk2124_remove),
    .resume = wk2124_resume,
};

static int __init wk2124A_init(void)
{
    int retval;

    retval = spi_register_driver(&wk2124A_driver);
    printk(KERN_ALERT "rgister spi return v = :%d\n", retval);

	return retval;
}

static void __exit wk2124A_exit(void)
{
    spi_unregister_driver(&wk2124A_driver);
    printk("TEST_REG:quit ");
}

module_init(wk2124A_init);
module_exit(wk2124A_exit);

MODULE_AUTHOR("WKIC Ltd");
MODULE_DESCRIPTION("wk2xxx generic serial port driver");
MODULE_LICENSE("GPL");
