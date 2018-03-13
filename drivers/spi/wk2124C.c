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
#include "wk2124C.h"

MODULE_LICENSE("Dual BSD/GPL");

#define SPI_BUFSIZ      max(32, SMP_CACHE_BYTES)
//#define _DEBUG_WK2XXX
//#define _DEBUG_WK2XXX1
//#define _DEBUG_WK2XXX2
//#define _DEBUG_WK2XXX4
#define CONFIG_DEVFS_FS

#define WK2XXX_PAGE1        1
#define WK2XXX_PAGE0        0

#define WK2XXX_STATUS_PE    1
#define WK2XXX_STATUS_FE    2
#define WK2XXX_STATUS_BRK   4
#define WK2XXX_STATUS_OE    8

static DEFINE_MUTEX(wk2xxxs_lock);                /* race on probe */

static DEFINE_MUTEX(wk2xxs_work_lock);                /* work on probe */

struct wk2xxx_port
{
    //struct timer_list mytimer;

    struct uart_port port;//[NR_PORTS];
    struct spi_device *spi_wk;
    spinlock_t conf_lock;	/* shared data */
    struct workqueue_struct *workqueue;
    struct work_struct work;
    int suspending;
    void (*wk2xxx_hw_suspend) (int suspend);
    int tx_done;

    int force_end_work;
    int irq;
    int minor;		/* minor number */
    int tx_empty;
    int tx_empty_flag;

    //int start_tx;
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
    /*
     *int work_tx_empty_flag;
     *int work_start_tx_flag;
     *int work_stop_rx_flag;
     *int work_stop_tx_flag;
     *int work_irq_flag;
     *int work_irq_fail;
     *int work_conf_flag;
     */

    uint8_t new_lcr;
    uint8_t new_scr;
    /*set baud 0f register*/
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
    // mutex_lock(&vk32xxs_lock);

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

static void wk2xxxirq_app(struct uart_port *port);//
static void conf_wk2xxx_subport(struct uart_port *port);//
static void wk2xxx_work(struct work_struct *w);
static void wk2xxx_stop_tx(struct uart_port *port);//
static u_int wk2xxx_tx_empty(struct uart_port *port);// or query the tx fifo is not empty?

static int wk2xxx_dowork(struct wk2xxx_port *s)
{
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_dowork---in---\n");
#endif

    if (!s->force_end_work && !work_pending(&s->work) && !freezing(current) && !s->suspending)
    {
        queue_work(s->workqueue, &s->work);//
#ifdef _DEBUG_WK2XXX
        printk("--queue_work---ok---\n");
        printk("--wk2xxx_dowork---exit---\n");
        // printk("work_pending =: %d s->force_end_work  = : %d freezing(current) = :%d s->suspending= :%d\n", work_pending(&s->work), s->force_end_work, freezing(current), s->suspending);
#endif
        return 1;
    }
    else
    {
#ifdef _DEBUG_WK2XXX
        printk("--queue_work---error---\n");
        printk("--wk2xxx_dowork---exit---\n");
#endif
        //printk("work_pending =: %d s->force_end_work  = : %d freezing(current) = :%d s->suspending= :%d\n", work_pending(&s->work), s->force_end_work, freezing(current), s->suspending);
        //	return 0;
        //	printk("work_pending() =: %d tx_empty_flag = : %d start_tx_flag = :%d stop_tx_flag = :%d conf_flag =: %d irq_flag =: %d tx_empty=:%d\n", work_pending(&s->work), s->tx_empty_flag, s->start_tx_flag, s->stop_tx_flag, s->stop_rx_flag, s->conf_flag, s->irq_flag, s->tx_empty);
        return 0;
    }
}

static void wk2xxx_work(struct work_struct *w)
{
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_work---in---\n");
#endif

    struct wk2xxx_port *s = container_of(w, struct wk2xxx_port, work);
    uint8_t rx;

    int work_tx_empty_flag = 0;
    int work_start_tx_flag;

    int work_stop_rx_flag;
    int work_stop_tx_flag = 0;

    int work_irq_flag;
    //int work_irq_fail;
    int work_conf_flag;
    do {
        mutex_lock(&wk2xxs_work_lock);
        //spin_lock(&s->conf_lock);
        /*work_tx_empty_flag = s->tx_empty_flag;
          if(work_tx_empty_flag)
          s->tx_empty_flag = 0;*/
        work_start_tx_flag = s->start_tx_flag;
        if(work_start_tx_flag)
            s->start_tx_flag = 0;
        /*work_stop_tx_flag = s->stop_tx_flag;
          if(work_stop_tx_flag)
          s->stop_tx_flag = 0;*/
        work_stop_rx_flag = s->stop_rx_flag;
        if(work_stop_rx_flag)
            s->stop_rx_flag = 0;
        work_conf_flag = s->conf_flag;
        if(work_conf_flag)
            s->conf_flag = 0;

        work_irq_flag = s->irq_flag;
        if(work_irq_flag)
            s->irq_flag = 0;

        //work_irq_fail = s->irq_fail;
        //if(work_irq_fail)
        //s->irq_fail = 0;

        //spin_unlock(&s->conf_lock);
        mutex_unlock(&wk2xxs_work_lock);

        if(work_conf_flag)
        {
            conf_wk2xxx_subport(&s->port);
        }
        /*if(work_tx_empty_flag)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, &rx);
            s->tx_empty = (rx & WK2XXX_TDAT)<=0;
#ifdef _DEBUG_WK2XXX1
            printk(KERN_ALERT "s->tx_empty_fail----FSR:%d--s->tx_empty:%d--\n", rx, s->tx_empty);
#endif
        }*/

        if(work_start_tx_flag)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
            rx |= WK2XXX_TFTRIG_IEN;
            wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
        }
        /* if(work_stop_tx_flag)
           {
           wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
           rx &=~WK2XXX_TFTRIG_IEN;
           wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
           wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, &rx);
           rx &= ~WK2XXX_TFTRIG_INT;
           wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, rx);
           }*/
        if(work_stop_rx_flag)
        {
            wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, &rx);
        #ifdef _DEBUG_WK2XXX1
            printk(KERN_ALERT "stop_rx_flag----SIER:%d--\n", rx);
        #endif

        rx &= ~WK2XXX_RFTRIG_IEN;
        rx &= ~WK2XXX_RXOUT_IEN;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, rx);
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT "stop_rx_flag----SIFR:%d--\n", rx);
#endif
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, &rx);

        rx &= ~WK2XXX_RFTRIG_INT;
        rx &= ~WK2XXX_RXOVT_INT;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIFR, rx);
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT "stop_rx_flag----SIFR:%d--\n", rx);
#endif
        }

        if(work_irq_flag)
        {
            wk2xxxirq_app(&s->port);
            s->irq_fail = 1;
        }
    }while (!s->force_end_work && !freezing(current) && \
        (work_irq_flag || work_stop_rx_flag || \
         work_stop_tx_flag || work_tx_empty_flag || work_conf_flag));
    /*
       }while (!s->force_end_work && !freezing(current) && \
       ((s->work_irq_flag != s->irq_flag) ||\
       (s->work_stop_rx_flag != s->stop_rx_flag) ||\
       (s->work_stop_tx_flag != s->stop_tx_flag) ||\
       (s->work_tx_empty_flag != s->tx_empty_flag) ||\
       (s->work_conf_flag != s->conf_flag)));

    */

    #ifdef _DEBUG_WK2XXX
    printk("-----exit------- work ------\n");
    #endif

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
        //s->irq_fail = 0;
    }
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_work---exit---\n");
#endif
}

static void wk2xxx_rx_chars(struct uart_port *port)//vk32xx_port *port)
{
#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "wk2xxx_rx_chars()---------in---\n");
#endif

    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    uint8_t fsr, lsr, dat[1];
    unsigned int ch, flg, ignored=0, status = 0, rx_count=0;

    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, WK2XXX_PAGE0);//set register in page0
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_LSR, dat);
    lsr = dat[0];
#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "wk2xxx_rx_chars()----port:%d--fsr:%d--lsr:%d--\n", s->port.iobase, fsr, lsr);
#endif

    while (fsr& WK2XXX_RDAT)/**/
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, dat);
        ch = (int)dat[0];
#ifdef _DEBUG_WK2XXX4

        printk(KERN_ALERT "wk2xxx_rx_chars()----port:%d--RXDAT:0x%x----\n", s->port.iobase, ch);
#endif

        s->port.icount.rx++;
        //rx_count++;
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT "icount.rx:%d\n", s->port.icount.rx);
#endif
        flg = TTY_NORMAL;
        if (lsr&(WK2XXX_OE |WK2XXX_FE|WK2XXX_PE|WK2XXX_BI))
        {
            printk(KERN_ALERT "2\n");
            //goto handle_error;
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
        if (uart_handle_sysrq_char(&s->port, ch))//.state, ch))
            goto ignore_char;

        uart_insert_char(&s->port, status, WK2XXX_STATUS_OE, ch, flg);
        rx_count++;

        if ((rx_count >= 8 ) && (s->port.state->port.tty != NULL))
        {
            tty_flip_buffer_push(s->port.state->port.tty);
            rx_count = 0;
        }
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT  " s->port.icount.rx = 0x%X char = 0x%X flg = 0x%X port = %d rx_count = %d\n", s->port.icount.rx, ch, flg, s->port.iobase, rx_count);
#endif
ignore_char:
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
        fsr = dat[0];
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_LSR, dat);
        lsr = dat[0];
    }

out:
    if((rx_count > 0)&&(s->port.state->port.tty != NULL))
    {
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT  "push buffer tty flip port = :%d count = :%d\n", s->port.iobase, rx_count);
#endif
        tty_flip_buffer_push(s->port.state->port.tty);
    }

#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "wk2xxx_rx_chars()---------out---\n");
#endif

    return;
#ifdef SUPPORT_SYSRQ
    s->port.state->sysrq = 0;
#endif
    goto error_return;

#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_rx_chars---exit---\n");
#endif
}

static void wk2xxx_tx_chars(struct uart_port *port)//
{
#ifdef _DEBUG_WK2XXX4
    printk("--wk2xxx_tx_chars---in---\n");
#endif

    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    uint8_t fsr, dat[1];
    int count;
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "                             in  wk2xxx_tx_chars()\n");
#endif
    if (s->port.x_char)
    {
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, s->port.x_char);
        s->port.icount.tx++;
        s->port.x_char = 0;
        goto out;
        //return;
    }

    if(uart_circ_empty(&s->port.state->xmit) || uart_tx_stopped(&s->port))
    {
        //wk2xxx_stop_tx(&s->port);
        goto out;
        //return;
    }

    /*
     * Tried using FIFO (not checking TNF) for fifo fill:
     * still had the '1 bytes repeated' problem.
     */
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];

#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "fsr:%x\n", fsr);
#endif
    //count = s->port.fifosize / 2;
    count = s->port.fifosize;
    //count = 64;
    do
    {
        if((fsr & WK2XXX_TFULL)|uart_circ_empty(&s->port.state->xmit))
            goto out;
        // break;
        wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FDAT, s->port.state->xmit.buf[s->port.state->xmit.tail]);
#ifdef _DEBUG_WK2XXX
        printk(KERN_ALERT "wk2xxx_tx_chars()----port:%d--TXDAT:0x%x----\n", s->port.iobase, s->port.state->xmit.buf[s->port.state->xmit.tail]);
#endif
        //udelay(1000);
        s->port.state->xmit.tail = (s->port.state->xmit.tail + 1) & (UART_XMIT_SIZE - 1);
        s->port.icount.tx++;
#ifdef _DEBUG_WK2XXX
        printk(KERN_ALERT "xmit.head:%d, xmit.tail:%d, char:%d, fsr:0x%X, port = %d\n", s->port.state->xmit.head, s->port.state->xmit.tail, s->port.state->xmit.buf[s->port.state->xmit.tail], fsr, s->port.iobase);
#endif
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
        fsr = dat[0];
    }while(--count > 0);

#ifdef _DEBUG_WK2XXX
    do
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
        fsr = dat[0];
    }while(fsr&WK2XXX_TDAT > 0);

    printk(KERN_ALERT "tx_char --fsr:0x%X, port = %d\n", fsr, s->port.iobase);
#endif
out:wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    fsr = dat[0];
    //printk(KERN_ALERT "tx_char --fsr:0x%X, port = %d\n", fsr, s->port.iobase);
    if((fsr&WK2XXX_TDAT)==0)
    {
        //printk(KERN_ALERT "tx_char1 --fsr:0x%X, port = %d\n", fsr, s->port.iobase);
        if (uart_circ_chars_pending(&s->port.state->xmit) < WAKEUP_CHARS)
            uart_write_wakeup(&s->port);

        if (uart_circ_empty(&s->port.state->xmit))
        {
            wk2xxx_stop_tx(&s->port);
        }
    }
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_tx_chars---exit---\n");
#endif
}

static irqreturn_t wk2xxx_irq(int irq, void *dev_id)//
{
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_irq---in---\n");
#endif

    struct wk2xxx_port *s = dev_id;
    //char sier;
    disable_irq_nosync(s->port.irq);
    //s->irq_fail=1;
    //s->irq_flag = 1;
    //mutex_unlock(&vk32xxs_work_lock);
    //vk32xx_dowork(s);

    if(wk2xxx_dowork(s))
    {
        //disable_irq_nosync(s->port.irq);
        //if(!s->irq_fail)
        //{
        s->irq_flag = 1;
        //s->irq_fail = 0;
        //}
    }
    else
    {
        //if(!s->irq_flag)
        s->irq_fail = 1;
    }
    //disable_irq_nosysc(s->port.irq);
#ifdef _DEBUG_WK2XXX
    printk("--wk2xxx_irq---exit---\n");
#endif

    return IRQ_HANDLED;
}

static void wk2xxxirq_app(struct uart_port *port)//
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "wk2xxxirq_app()------port:%d--------------\n", s->port.iobase);
#endif
    unsigned int  pass_counter = 0;
    uint8_t sifr, gifr, sier, dat[1];

    uint8_t sifr0, sifr1, sifr2, sifr3, sier1, sier0, sier2, sier3;

    wk2xxx_read_reg(s->spi_wk, WK2XXX_GPORT, WK2XXX_GIFR, dat);
    gifr = dat[0];
    wk2xxx_write_reg(s->spi_wk, 1, WK2XXX_SPAGE, WK2XXX_PAGE0);//set register in page0
    wk2xxx_write_reg(s->spi_wk, 2, WK2XXX_SPAGE, WK2XXX_PAGE0);//set register in page0
    wk2xxx_write_reg(s->spi_wk, 3, WK2XXX_SPAGE, WK2XXX_PAGE0);//set register in page0
    wk2xxx_write_reg(s->spi_wk, 4, WK2XXX_SPAGE, WK2XXX_PAGE0);//set register in page0

    wk2xxx_read_reg(s->spi_wk, 1, WK2XXX_SIFR, &sifr0);
    wk2xxx_read_reg(s->spi_wk, 2, WK2XXX_SIFR, &sifr1);
    wk2xxx_read_reg(s->spi_wk, 3, WK2XXX_SIFR, &sifr2);
    wk2xxx_read_reg(s->spi_wk, 4, WK2XXX_SIFR, &sifr3);

    wk2xxx_read_reg(s->spi_wk, 1, WK2XXX_SIER, &sier0);
    wk2xxx_read_reg(s->spi_wk, 2, WK2XXX_SIER, &sier1);
    wk2xxx_read_reg(s->spi_wk, 3, WK2XXX_SIER, &sier2);
    wk2xxx_read_reg(s->spi_wk, 4, WK2XXX_SIER, &sier3);

#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "irq_app....gifr:%x  sier1:%x  sier2:%x sier3:%x sier4:%x   sifr1:%x sifr2:%x sifr3:%x sifr4:%x \n", gifr, sier0, sier1, sier2, sier3, sifr0, sifr1, sifr2, sifr3);
#endif
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
#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "irq_app..........sifr:%x sier:%x \n", sifr, sier);
#endif
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
#ifdef _DEBUG_WK2XXX1
        printk(KERN_ALERT "irq_app...........rx............tx  sifr:%x sier:%x port:%x\n", sifr, sier, s->port.iobase);
#endif
    } while ((sifr&WK2XXX_RXOVT_INT) || (sifr & WK2XXX_RFTRIG_INT) ||
             ((sifr & WK2XXX_TFTRIG_INT)&&(sier & WK2XXX_TFTRIG_IEN)));

#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "sifr:%d\n", sifr);
#endif
#ifdef _DEBUG_WK2XXX1
    printk(KERN_ALERT "wk2xxxirq_app()---------exit---\n");
#endif
}

/*
 *   Return TIOCSER_TEMT when transmitter is not busy.
 */
static u_int wk2xxx_tx_empty(struct uart_port *port)// or query the tx fifo is not empty?
{
    uint8_t rx;
    struct wk2xxx_port *s = NULL;

    mutex_lock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "wk2xxx_tx_empty()---------in---\n");
#endif

    s  = container_of(port, struct wk2xxx_port, port);
    // mutex_lock(&wk2xxxs_lock);
    //s->tx_empty_flag = 1;

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
#ifdef _DEBUG_WK2XXX4
    printk(KERN_ALERT "s->tx_empty_fail----FSR:%d--s->tx_empty:%d--\n", rx, s->tx_empty);
#endif

#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "wk2xxx_tx_empty----------exit---\n");
#endif
    mutex_unlock(&wk2xxxs_lock);
    return s->tx_empty;
}

static void wk2xxx_set_mctrl(struct uart_port *port, u_int mctrl)//nothing
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_set_mctrl---------exit---\n");
#endif

}
static u_int wk2xxx_get_mctrl(struct uart_port *port)// since no modem control line
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_get_mctrl---------exit---\n");
#endif
    return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

/*
 *  interrupts disabled on entry
 */
static void wk2xxx_stop_tx(struct uart_port *port)//
{
    uint8_t rx;
    struct wk2xxx_port *s = NULL;

    mutex_lock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_stop_tx------in---\n");
#endif

    //mutex_lock(&wk2xxxs_lock);
    //disable the interrupt, clear the corresponding bit in GIR
    s = container_of(port, struct wk2xxx_port, port);

    if (!(s->stop_tx_flag || s->stop_tx_fail))
    {
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, (uint8_t *)&rx);
        s->stop_tx_fail = (rx & WK2XXX_TFTRIG_IEN) > 0;

        if (s->stop_tx_fail)
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
    //mutex_unlock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX4
    printk(KERN_ALERT "-wk2xxx_stop_tx------exit---\n");
#endif
    mutex_unlock(&wk2xxxs_lock);
}

/*
 *  * interrupts may not be disabled on entry
 */
static void wk2xxx_start_tx(struct uart_port *port)
{
    //mutex_lock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_start_tx------in---\n");
#endif
    // mutex_lock(&wk2xxxs_lock);
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    //	uint8_t rx;
    //s->start_tx_flag = 1;
    //vk32xx_dowork(s);
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
    // mutex_unlock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_start_tx------exit---\n");
#endif
    // mutex_unlock(&wk2xxxs_lock);

    // vk32xx_read_reg(s->spi_vk, s->port.iobase, VK32XX_SIER, &rx);
    // rx |= VK32XX_TRIEN;
    // vk32xx_write_reg(s->spi_vk, s->port.iobase, VK32XX_SIER, rx);
}

/*
 *  * Interrupts enabled
 */

static void wk2xxx_stop_rx(struct uart_port *port)
{
    //mutex_lock(&wk2xxxs_lock);
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_stop_rx------in---\n");
#endif
    //	uint8_t rx;
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    //s->stop_rx_flag = 1;
    // mutex_lock(&wk2xxxs_lock);
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

#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_stop_rx------exit---\n");
#endif
    //mutex_unlock(&wk2xxxs_lock);
}

/*
 *  * No modem control lines
 *   */
static void wk2xxx_enable_ms(struct uart_port *port)    //nothing
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_enable_ms------exit---\n");
#endif
}

/*
 *  * Interrupts always disabled.
 */
static void wk2xxx_break_ctl(struct uart_port *port, int break_state)
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_break_ctl------exit---\n");
#endif
}

static int wk2xxx_startup(struct uart_port *port)//i
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-wk2xxx_startup------in---\n");
#endif

    uint8_t gena, grst, gier, sier, scr, dat[1];
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
    char b[64];

    if (s->suspending)
        return 0;
    //printk("work_pending() =: %d tx_empty_flag = : %d start_tx_flag = :%d
    //    stop_tx_flag = :%d conf_flag =: %d irq_flag =: %d tx_empty=:%d\n",
    //    work_pending(&s->work), s->tx_empty_flag, s->start_tx_flag, s->stop_tx_flag,
    //    s->stop_rx_flag, s->conf_flag, s->irq_flag, s->tx_empty);

    s->force_end_work = 0;
    sprintf(b, "wk2xxx-%d", (uint8_t)s->port.iobase);
    //s->workqueue = create_singlethread_workqueue(b);
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

	//initiate the fifos
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FCR, 0x0f);//initiate the fifos
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_FCR, 0x0c);
	//enable the sub port interrupt
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
    // request irq
	if(request_irq(s->port.irq, wk2xxx_irq, IRQF_SHARED|IRQF_TRIGGER_LOW, "wk2xxxspi", s) < 0)
    {
       dev_warn(&s->spi_wk->dev, "cannot allocate irq %d\n", s->irq);
       s->port.irq = 0;
       destroy_workqueue(s->workqueue);
       s->workqueue = NULL;
       return -EBUSY;
    }
    udelay(100);
	udelay(100);
#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_startup------exit---\n");
#endif

    return 0;
}

static void wk2xxx_shutdown(struct uart_port *port)//
{

#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_shutdown------in---\n");
#endif

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
        disable_irq_nosync(s->port.irq);
		free_irq(s->port.irq, s);
	}
    //printk("work_pending() =: %d tx_empty_flag = : %d start_tx_flag = :%d
    //      stop_tx_flag = :%d conf_flag =: %d irq_flag =: %d tx_empty=:%d\n",
    //      work_pending(&s->work), s->tx_empty_flag, s->start_tx_flag,
    //      s->stop_tx_flag, s->stop_rx_flag, s->conf_flag, s->irq_flag, s->tx_empty);
#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_shutdown-----exit---\n");
#endif

    return;
}

static void conf_wk2xxx_subport(struct uart_port *port)//i
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-conf_wk2xxx_subport------in---\n");
#endif
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
    //local_irq_restore(flags);
    do{
        wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_FSR, dat);
    //ssr = dat[0];
    } while (dat[0] & WK2XXX_TBUSY);

    // then, disable everything
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, dat);
    scr = dat[0];
    scr &= 0x0f;
    scr |= scr_ss;

    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr&(~(WK2XXX_RXEN|WK2XXX_TXEN)));
    // set the parity, stop bits and data size //
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_LCR, lcr);
    // set the baud rate //
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SIER, old_sier);
    // wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr|(WK2XXX_RXEN|WK2XXX_TXEN));
    // set the baud rate //
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, 1);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD0, baud0_ss);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD1, baud1_ss);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_PRES, pres_ss);
#ifdef _DEBUG_WK2XXX2
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD0, dat);
    printk(KERN_ALERT ":WK2XXX_BAUD0=0x%X\n", dat[0]);
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_BAUD1, dat);
    printk(KERN_ALERT ":WK2XXX_BAUD1=0x%X\n", dat[0]);
    wk2xxx_read_reg(s->spi_wk, s->port.iobase, WK2XXX_PRES, dat);
    printk(KERN_ALERT ":WK2XXX_PRES=0x%X\n", dat[0]);
#endif
	wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SPAGE, 0);
    wk2xxx_write_reg(s->spi_wk, s->port.iobase, WK2XXX_SCR, scr|(WK2XXX_RXEN|WK2XXX_TXEN));

#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-conf_wk2xxx_subport------exit---\n");
#endif
}

// change speed
static void wk2xxx_termios( struct uart_port *port, struct ktermios *termios,
		    struct ktermios *old)
{
#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-vk32xx_termios------in---\n");
#endif

    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);
	int baud = 0;
	uint8_t lcr, scr = 0, baud1, baud0, pres;
	unsigned short cflag;
	unsigned short lflag;
	//u32 param_new, param_mask;
    //mutex_lock(&wk2xxxs_lock);
	cflag = termios->c_cflag;
	lflag = termios->c_lflag;
#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "cflag := 0x%X  lflag : = 0x%X\n", cflag, lflag);
#endif
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

	/* we are sending char from a workqueue so enable */

	//spin_lock(&s->conf_lock);

	//s->port.state->port.tty->low_latency = 1;
	//termios->c_lflag &= ~ECHO;

	lcr = 0;
    if (cflag & CSTOPB)
        lcr|=WK2XXX_STPL;//two  stop_bits
    else
        lcr&=~WK2XXX_STPL;//one  stop_bits

    if (cflag & PARENB)
    {
        lcr|=WK2XXX_PAEN;//enbale spa
        if (!(cflag & PARODD))
        {
            lcr |= WK2XXX_PAM0;//PAM0=1
            lcr &= ~WK2XXX_PAM0;//PAM1=0
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

	//scr = 0;
	//scr &= 0x0f;
	//scr |= param_new<<4;
	s->new_baud1=baud1;
	s->new_baud0=baud0;
	s->new_pres=pres;

	//spin_lock(&s->conf_lock);
	//s->conf_flag =1;
	s->new_lcr = lcr;
	s->new_scr = scr;
	//spin_unlock(&s->conf_lock);
	//vk32xx_dowork(s);

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
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "-vk32xx_termios------exit---\n");
#endif
	// mutex_unlock(&wk2xxxs_lock);
	return ;
}

static const char *wk2xxx_type(struct uart_port *port)
{
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "wk2xxx_type-------------out-------- \n");
#endif
    return port->type == PORT_WK2XXX ? "wk2xxx" : NULL;//this is defined in serial_core.h
}

/*
* Release the memory region(s) being used by 'port'.
*/
static void wk2xxx_release_port(struct uart_port *port)
{
    printk(KERN_ALERT "wk2xxx_release_port\n");
//	struct vk32xx_port *s = container_of(port, struct vk32xx_port, port);
//	dev_dbg(&s->spi_vk->dev, "%s\n", __func__);
        //no such memory region for vk32
}

/*
* Request the memory region(s) being used by 'port'.
*/
static int wk2xxx_request_port(struct uart_port *port)//no such memory region needed for vk32
{
    printk(KERN_ALERT "wk2xxx_request_port\n");
    //struct vk32xx_port *s = container_of(port, struct vk32xx_port, port);
    return 0;
}

/*
* Configure/autoconfigure the port*/
static void wk2xxx_config_port(struct uart_port *port, int flags)
{
    struct wk2xxx_port *s = container_of(port, struct wk2xxx_port, port);

#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "wk2xxx_config_port \n");
#endif

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
#ifdef _DEBUG_WK2XXX
    printk(KERN_ALERT "wk2xxx_verify_port \n");
#endif
    int ret = 0;
    if (ser->type != PORT_UNKNOWN && ser->type != PORT_WK2XXX)
        ret = -EINVAL;
    if (port->irq != ser->irq)
        ret = -EINVAL;
    if (ser->io_type != SERIAL_IO_PORT)
        ret = -EINVAL;
    //  if (port->uartclk / 16 != ser->baud_base)//?a??2?è·?¨
    //  ret = -EINVAL;
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

static struct uart_driver wk2xxx_uart_driver = {
    owner:          THIS_MODULE,
    major:        	SERIAL_WK2124C_MAJOR,
#ifdef CONFIG_DEVFS_FS
    driver_name:    "ttySWZC",
    dev_name:       "ttysWZC",
#else
    driver_name:    "ttySWZC",
    dev_name:       "ttysWZC",
#endif
    minor:          MINOR_START,
    nr:             NR_PORTS,
    cons:           NULL//WK2Xxx_CONSOLE,
};

static int uart_driver_registered;

static int __devinit wk2124C_probe(struct spi_device *spi)
{
#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2124C_probe()------in---\n");
#endif
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

    printk(KERN_ALERT "wk2xxx_probe wk2xxx\n");

	mutex_lock(&wk2xxxs_lock);

	if(!uart_driver_registered)
	{
		uart_driver_registered = 1;
        status = uart_register_driver(&wk2xxx_uart_driver);
        if (status)
       	{
        	printk(KERN_ERR "Couldn't register wk2xxx uart driver\n");
        	mutex_unlock(&wk2xxxs_lock);
        	return status;
		}
    }
    printk(KERN_ALERT "wk2xxx_serial_init()\n");

	for (i = 0; i < NR_PORTS; i++)
	{
		struct wk2xxx_port *s = &wk2xxxs[i];//container_of(port, struct vk32xx_port, port);
		s->tx_done = 0;
		s->spi_wk    = spi;
		s->port.line = i;
		s->port.ops = &wk2xxx_pops;
		s->port.uartclk = WK_CRASTAL_CLK;
		s->port.fifosize = 64;
		s->port.iobase = i+1;
		s->port.irq    = IRQ_WK2124C ;
		s->port.iotype = SERIAL_IO_PORT;
		s->port.flags  = ASYNC_BOOT_AUTOCONF;
		//s->minor       = i;
        status = uart_add_one_port(&wk2xxx_uart_driver, &s->port);
        if(status<0)
        {
            //dev_warn(&spi->dev, "uart_add_one_port failed for line i:= %d with error %d\n", i, status);
            printk(KERN_ALERT "uart_add_one_port failed for line i:= %d with error %d\n", i, status);
		}
	}
	printk(KERN_ALERT "uart_add_one_port = 0x%d\n", status);

	mutex_unlock(&wk2xxxs_lock);

#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_probe()------exit---\n");
#endif
    return 0;
}

static int __devexit wk2124C_remove(struct spi_device *spi)
{
	//struct wk2xxx_port *s = dev_get_drvdata(&spi->dev);
	int i;

#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_remove()------in---\n");
#endif
	mutex_lock(&wk2xxxs_lock);
    for(i =0; i < NR_PORTS; i++)
    {
        struct wk2xxx_port *s = &wk2xxxs[i];
	    uart_remove_one_port(&wk2xxx_uart_driver, &s->port);
	}
	printk(KERN_ALERT "removing wk2xxx driver\n");

	uart_unregister_driver(&wk2xxx_uart_driver);

	mutex_unlock(&wk2xxxs_lock);

#ifdef _DEBUG_WK2XXX
	printk(KERN_ALERT "-wk2xxx_remove()------exit---\n");
#endif

	return 0;
}

static int wk2124C_resume(struct spi_device *spi)
{
	printk(KERN_ALERT "resume wk2124C");
	return 0;
}

static struct spi_driver wk2124C_driver = {
    .driver = {
        .name  = "wk2124C",
        .bus   = &spi_bus_type,
        .owner = THIS_MODULE,
    },

    .probe  = wk2124C_probe,
    .remove = __devexit_p(wk2124C_remove),
    .resume = wk2124C_resume,
};

static int __init wk2124C_init(void)
{
    int retval;

    retval = spi_register_driver(&wk2124C_driver);
    printk(KERN_ALERT "rgister spi return v = :%d\n", retval);

	return retval;
}

static void __exit wk2124C_exit(void)
{
    spi_unregister_driver(&wk2124C_driver);
    printk("TEST_REG:quit ");
}

module_init(wk2124C_init);
module_exit(wk2124C_exit);

MODULE_AUTHOR("WKIC Ltd");
MODULE_DESCRIPTION("wk2124C generic serial port driver");
MODULE_LICENSE("GPL");
