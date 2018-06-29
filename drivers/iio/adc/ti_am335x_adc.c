/*
 * TI ADC MFD driver
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/iio/iio.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/iio/machine.h>
#include <linux/iio/driver.h>

#include <linux/mfd/ti_am335x_tscadc.h>
#include <linux/iio/buffer.h>
#include <linux/iio/kfifo_buf.h>

struct mutex mutex;

ssize_t adc_calibration_show(struct kobject *kobj, struct attribute *attr, char *buffer);
ssize_t adc_calibration_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size);

struct ADC_calibration
{
    int   d1_theoretical_value;
    int   d1_measurements_value;
    int   d2_theoretical_value;
    int   d2_measurements_value;
    int   channel;
    bool  calibration_flags;
    struct kobject kobj;
};

int d1in[8] = {0};
int d2in[8] = {0};
int d1out[8] = {0};
int d2out[8] = {0};
bool cahnnel_flagss[8] = {false,false,false,false,false,false,false,false};
struct attribute d1_theoretical_val_file = 
{
    .name = "d1in",
    .mode = 0666,
};
struct attribute d1_measurements_val_file = 
{
    .name = "d1out",
    .mode = 0666,
};
struct attribute d2_theoretical_val_file = 
{
    .name = "d2in",
    .mode = 0666,
};
struct attribute d2_measurements_val_file = 
{
    .name = "d2out",
    .mode = 0666,
};

struct attribute calibration_flags_file = 
{
    .name = "calibration",
    .mode = 0666,
};
struct attribute re_calibration_flags_file = 
{
    .name = "re_calibration",
    .mode = 0666,
};
struct attribute re_calibration_all_flags_file = 
{
    .name = "re_calibration_all",
    .mode = 0666,
};
struct attribute remade_file = 
{
    .name = "REMADE",
    .mode = 0666,
};
struct attribute channel_file = 
{
    .name = "channel",
    .mode = 0666,
};
struct attribute *my_attrs[] = 
{
    &d1_theoretical_val_file,
    &d1_measurements_val_file,
    &d2_theoretical_val_file,
    &d2_measurements_val_file,
    &calibration_flags_file,
    &remade_file,
    &channel_file,
    &re_calibration_flags_file,
    &re_calibration_all_flags_file,
    NULL,
};

struct sysfs_ops my_sysfsops = 
{
    .show  =    &adc_calibration_show,
    .store =    &adc_calibration_store,
};

struct kobj_type my_type = {
    .default_attrs =  my_attrs,
    .sysfs_ops = &my_sysfsops,
};
struct ADC_calibration *adc_calibration;



struct tiadc_device {
	struct ti_tscadc_dev *mfd_tscadc;
	int channels;
	u8 channel_line[8];
	u8 channel_step[8];
	int buffer_en_ch_steps;
	u16 data[8];
	u32 open_delay[8], sample_delay[8], step_avg[8];
};




ssize_t adc_calibration_show(struct kobject *kobj, struct attribute *attr, char *buffer)
{
    struct  ADC_calibration *obj = container_of(kobj, struct ADC_calibration, kobj);
    ssize_t count = 0;
    int i = 0;
    if (strcmp(attr->name, "d1in") == 0) {
        count = sprintf(buffer, "%d\n",obj->d1_theoretical_value);
    } else if (strcmp(attr->name, "d2in") == 0) {
        count = sprintf(buffer, "%d\n", obj->d2_theoretical_value);
    }else if (strcmp(attr->name, "d1out") == 0) {
        count = sprintf(buffer, "%d\n", obj->d1_measurements_value);
    } else if (strcmp(attr->name, "d2out") == 0) {
        count = sprintf(buffer, "%d\n", obj->d2_measurements_value);
    }else if(strcmp(attr->name, "channel") == 0){
        count = sprintf(buffer, "%d\n", obj->channel);
    }else if(strcmp(attr->name, "REMADE") == 0)
    {
        count = sprintf(buffer, "%s\n", "adc calibration");
    }else if(strcmp(attr->name, "re_calibration_all") == 0)
    {
       for(i=0;i<8;i++)
       {
            if(i<3)
            {
                
                printk("d1in[%d] = %d,d2in[%d] = %d,d1out[%d] = %d,d2out[%d] = %d\n", \
                           i,d1in[i],i,d2in[i],i,d1out[i],i,d2out[i]);
            }

       }
    }else if(strcmp(attr->name, "re_calibration") == 0)
    {
        for(i = 0;i<8;i++)
        {
            if(adc_calibration->channel == i)
            {
              
                printk("d1in[%d] = %d,d2in[%d] = %d,d1out[%d] = %d,d2out[%d] = %d\n", \
                           i,d1in[i],i,d2in[i],i,d1out[i],i,d2out[i]);
                break;
            }
        }

    }else if (strcmp(attr->name, "calibration") == 0) {
        if(obj->calibration_flags)
        {
            count = sprintf(buffer, "%s\n", "true");
        }else
        {
            count = sprintf(buffer, "%s\n", "false");
        }
    }

    return count;
}

ssize_t adc_calibration_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size)
{
    struct ADC_calibration *obj = container_of(kobj, struct ADC_calibration, kobj);

    int i = 0;
    if (strcmp(attr->name, "d1in") == 0) 
    {
        sscanf(buffer, "%d",&obj->d1_theoretical_value);
    } else if (strcmp(attr->name, "d2in") == 0) {
        sscanf(buffer, "%d",&obj->d2_theoretical_value);
    }else if (strcmp(attr->name, "d1out") == 0) {
        sscanf(buffer, "%d",&obj->d1_measurements_value);
    } else if (strcmp(attr->name, "d2out") == 0) {
        sscanf(buffer, "%d",&obj->d2_measurements_value);
    }else if(strcmp(attr->name, "channel") == 0)
    {
        sscanf(buffer, "%d",&obj->channel);
    }else if(strcmp(attr->name, "re_calibration_all") == 0)
    {
        if(strncmp(buffer, "true",4) == 0)
        {
            for(i=0;i<8;i++)
            {
                d1in[i] = d2in[i] = d2out[i] = d1out[i] = 0;
                if(i<3)
                {

                    printk("d1in[%d] = %d,d2in[%d] = %d,d1out[%d] = %d,d2out[%d] = %d\n", \
                           i,d1in[i],i,d2in[i],i,d1out[i],i,d2out[i]);
                }

            }
        }
    }else if(strcmp(attr->name, "re_calibration") == 0)
    {
        if(strncmp(buffer, "true",4) == 0)
        {
            for(i = 0;i<8;i++)
            {
                if(adc_calibration->channel == i)
                {
                    d1in[i] = d2in[i] = d2out[i] = d1out[i] = 0;

                    printk("d1in[%d] = %d,d2in[%d] = %d,d1out[%d] = %d,d2out[%d] = %d\n", \
                           i,d1in[i],i,d2in[i],i,d1out[i],i,d2out[i]);
                    break;
                }
            }
        }

    }else if (strcmp(attr->name, "calibration") == 0) {
        if(strncmp(buffer, "true",4) == 0)
        {
            obj->calibration_flags = true;


            if(adc_calibration->channel < 0 || adc_calibration->channel > 7)
            {
                adc_calibration->calibration_flags = false;
                printk("The number of channels is between 0, 1, 2, 3,4,5,6,7\n");
                return -EINVAL;
            }


            for(i = 0;i<8;i++)
            {

                if(adc_calibration->d2_measurements_value == adc_calibration->d1_measurements_value)
                {
                    printk("d2out cannot be equal to d1out\n");
                    return -EINVAL;
                }
                if(adc_calibration->channel == i)
                {
                    d1in[i]  = adc_calibration->d1_theoretical_value;
                    d2in[i]  = adc_calibration->d2_theoretical_value;
                    d1out[i] = adc_calibration->d1_measurements_value;
                    d2out[i] = adc_calibration->d2_measurements_value;
                    printk("d1in[%d] = %d,d2in[%d] = %d,d1out[%d] = %d,d2out[%d] = %d\n", \
                           i,d1in[i],i,d2in[i],i,d1out[i],i,d2out[i]);
                    break;
                }
            }

            obj->calibration_flags = false;

        }else if(strncmp(buffer, "false",5) == 0)
        {
            obj->calibration_flags = false;
        }else
        {
            return -EINVAL;
        }
    }




    return size;
}

int  adc_calibration_init(struct iio_dev *indio_dev)
{
    
    adc_calibration =  kzalloc(sizeof(struct ADC_calibration), GFP_KERNEL);
    if(!adc_calibration)
    {
        return -ENOMEM;
    }

     adc_calibration->calibration_flags              = false;
     adc_calibration->channel = 0;
     
   // kobject_init_and_add(&adc_calibration.kobj, &adc_calibration.my_type,&indio_dev->dev.kobj , "adc_calibration");
    kobject_init_and_add(&adc_calibration->kobj, &my_type,&indio_dev->dev.kobj, "adc_calibration");

    return 0;
}





static unsigned int tiadc_readl(struct tiadc_device *adc, unsigned int reg)
{
	return readl(adc->mfd_tscadc->tscadc_base + reg);
}

static void tiadc_writel(struct tiadc_device *adc, unsigned int reg,
					unsigned int val)
{  
	writel(val, adc->mfd_tscadc->tscadc_base + reg);
}

static u32 get_adc_step_mask(struct tiadc_device *adc_dev)
{
	u32 step_en;

	step_en = ((1 << adc_dev->channels) - 1);
	step_en <<= TOTAL_STEPS - adc_dev->channels + 1;
	return step_en;
}

static u32 get_adc_chan_step_mask(struct tiadc_device *adc_dev,
		struct iio_chan_spec const *chan)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(adc_dev->channel_step); i++) {
		if (chan->channel == adc_dev->channel_line[i]) {
			u32 step;

			step = adc_dev->channel_step[i];
			/* +1 for the charger */
			return 1 << (step + 1);
		}
	}
	WARN_ON(1);
	return 0;
}

static u32 get_adc_step_bit(struct tiadc_device *adc_dev, int chan)
{
	return 1 << adc_dev->channel_step[chan];
}

static void tiadc_step_config(struct iio_dev *indio_dev)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	struct device *dev = adc_dev->mfd_tscadc->dev;
	unsigned int stepconfig;
	int i, steps = 0;

	/*
	 * There are 16 configurable steps and 8 analog input
	 * lines available which are shared between Touchscreen and ADC.
	 *
	 * Steps forwards i.e. from 0 towards 16 are used by ADC
	 * depending on number of input lines needed.
	 * Channel would represent which analog input
	 * needs to be given to ADC to digitalize data.
	 */


	for (i = 0; i < adc_dev->channels; i++) {
		int chan;

		chan = adc_dev->channel_line[i];

		if (adc_dev->step_avg[i] > STEPCONFIG_AVG_16) {
			dev_warn(dev, "chan %d step_avg truncating to %d\n",
				 chan, STEPCONFIG_AVG_16);
			adc_dev->step_avg[i] = STEPCONFIG_AVG_16;
		}

		if (adc_dev->step_avg[i])
			stepconfig =
			STEPCONFIG_AVG(ffs(adc_dev->step_avg[i]) - 1) |
			STEPCONFIG_FIFO1;
		else
			stepconfig = STEPCONFIG_FIFO1;

		if (iio_buffer_enabled(indio_dev))
			stepconfig |= STEPCONFIG_MODE_SWCNT;

		tiadc_writel(adc_dev, REG_STEPCONFIG(steps),
				stepconfig | STEPCONFIG_INP(chan));

		if (adc_dev->open_delay[i] > STEPDELAY_OPEN_MASK) {
			dev_warn(dev, "chan %d open delay truncating to 0x3FFFF\n",
				 chan);
			adc_dev->open_delay[i] = STEPDELAY_OPEN_MASK;
		}

		if (adc_dev->sample_delay[i] > 0xFF) {
			dev_warn(dev, "chan %d sample delay truncating to 0xFF\n",
				 chan);
			adc_dev->sample_delay[i] = 0xFF;
		}

		tiadc_writel(adc_dev, REG_STEPDELAY(steps),
				STEPDELAY_OPEN(adc_dev->open_delay[i]) |
				STEPDELAY_SAMPLE(adc_dev->sample_delay[i]));

		adc_dev->channel_step[i] = steps;
		steps++;
	}
}

static irqreturn_t tiadc_irq_h(int irq, void *private)
{
	struct iio_dev *indio_dev = private;
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	unsigned int status, config;
	status = tiadc_readl(adc_dev, REG_IRQSTATUS);

	/*
	 * ADC and touchscreen share the IRQ line.
	 * FIFO0 interrupts are used by TSC. Handle FIFO1 IRQs here only
	 */
	if (status & IRQENB_FIFO1OVRRUN) {
		/* FIFO Overrun. Clear flag. Disable/Enable ADC to recover */
		config = tiadc_readl(adc_dev, REG_CTRL);
		config &= ~(CNTRLREG_TSCSSENB);
		tiadc_writel(adc_dev, REG_CTRL, config);
		tiadc_writel(adc_dev, REG_IRQSTATUS, IRQENB_FIFO1OVRRUN
				| IRQENB_FIFO1UNDRFLW | IRQENB_FIFO1THRES);
		tiadc_writel(adc_dev, REG_CTRL, (config | CNTRLREG_TSCSSENB));
		return IRQ_HANDLED;
	} else if (status & IRQENB_FIFO1THRES) {
		/* Disable irq and wake worker thread */
		tiadc_writel(adc_dev, REG_IRQCLR, IRQENB_FIFO1THRES);
		return IRQ_WAKE_THREAD;
	}

	return IRQ_NONE;
}

static irqreturn_t tiadc_worker_h(int irq, void *private)
{
	struct iio_dev *indio_dev = private;
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	int i, k, fifo1count, read;
	u16 *data = adc_dev->data;

	fifo1count = tiadc_readl(adc_dev, REG_FIFO1CNT);
	for (k = 0; k < fifo1count; k = k + i) {
		for (i = 0; i < (indio_dev->scan_bytes)/2; i++) {
			read = tiadc_readl(adc_dev, REG_FIFO1);
			data[i] = read & FIFOREAD_DATA_MASK;
		}
		iio_push_to_buffers(indio_dev, (u8 *) data);
	}

	tiadc_writel(adc_dev, REG_IRQSTATUS, IRQENB_FIFO1THRES);
	tiadc_writel(adc_dev, REG_IRQENABLE, IRQENB_FIFO1THRES);

	return IRQ_HANDLED;
}

static int tiadc_buffer_preenable(struct iio_dev *indio_dev)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	int i, fifo1count, read;

	tiadc_writel(adc_dev, REG_IRQCLR, (IRQENB_FIFO1THRES |
				IRQENB_FIFO1OVRRUN |
				IRQENB_FIFO1UNDRFLW));

	/* Flush FIFO. Needed in corner cases in simultaneous tsc/adc use */
	fifo1count = tiadc_readl(adc_dev, REG_FIFO1CNT);
	for (i = 0; i < fifo1count; i++)
		read = tiadc_readl(adc_dev, REG_FIFO1);

	return 0;
}

static int tiadc_buffer_postenable(struct iio_dev *indio_dev)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	unsigned int enb = 0;
	u8 bit;

	tiadc_step_config(indio_dev);
	for_each_set_bit(bit, indio_dev->active_scan_mask, adc_dev->channels)
		enb |= (get_adc_step_bit(adc_dev, bit) << 1);
	adc_dev->buffer_en_ch_steps = enb;

	am335x_tsc_se_set_cache(adc_dev->mfd_tscadc, enb);

	tiadc_writel(adc_dev,  REG_IRQSTATUS, IRQENB_FIFO1THRES
				| IRQENB_FIFO1OVRRUN | IRQENB_FIFO1UNDRFLW);
	tiadc_writel(adc_dev,  REG_IRQENABLE, IRQENB_FIFO1THRES
				| IRQENB_FIFO1OVRRUN);
	return 0;
}

static int tiadc_buffer_predisable(struct iio_dev *indio_dev)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	int fifo1count, i, read;

	tiadc_writel(adc_dev, REG_IRQCLR, (IRQENB_FIFO1THRES |
				IRQENB_FIFO1OVRRUN | IRQENB_FIFO1UNDRFLW));
	am335x_tsc_se_clr(adc_dev->mfd_tscadc, adc_dev->buffer_en_ch_steps);
	adc_dev->buffer_en_ch_steps = 0;

	/* Flush FIFO of leftover data in the time it takes to disable adc */
	fifo1count = tiadc_readl(adc_dev, REG_FIFO1CNT);
	for (i = 0; i < fifo1count; i++)
		read = tiadc_readl(adc_dev, REG_FIFO1);

	return 0;
}

static int tiadc_buffer_postdisable(struct iio_dev *indio_dev)
{
	tiadc_step_config(indio_dev);

	return 0;
}

static const struct iio_buffer_setup_ops tiadc_buffer_setup_ops = {
	.preenable = &tiadc_buffer_preenable,
	.postenable = &tiadc_buffer_postenable,
	.predisable = &tiadc_buffer_predisable,
	.postdisable = &tiadc_buffer_postdisable,
};

static int tiadc_iio_buffered_hardware_setup(struct iio_dev *indio_dev,
	irqreturn_t (*pollfunc_bh)(int irq, void *p),
	irqreturn_t (*pollfunc_th)(int irq, void *p),
	int irq,
	unsigned long flags,
	const struct iio_buffer_setup_ops *setup_ops)
{
	struct iio_buffer *buffer;
	int ret;

	buffer = iio_kfifo_allocate();
	if (!buffer)
		return -ENOMEM;

	iio_device_attach_buffer(indio_dev, buffer);

	ret = request_threaded_irq(irq,	pollfunc_th, pollfunc_bh,
				flags, indio_dev->name, indio_dev);
	if (ret)
		goto error_kfifo_free;

	indio_dev->setup_ops = setup_ops;
	indio_dev->modes |= INDIO_BUFFER_SOFTWARE;

	return 0;

error_kfifo_free:
	iio_kfifo_free(indio_dev->buffer);
	return ret;
}

static void tiadc_iio_buffered_hardware_remove(struct iio_dev *indio_dev)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);

	free_irq(adc_dev->mfd_tscadc->irq, indio_dev);
	iio_kfifo_free(indio_dev->buffer);
}


static const char * const chan_name_ain[] = {
	"AIN0",
	"AIN1",
	"AIN2",
	"AIN3",
	"AIN4",
	"AIN5",
	"AIN6",
	"AIN7",
};

static int tiadc_channel_init(struct iio_dev *indio_dev, int channels)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	struct iio_chan_spec *chan_array;
	struct iio_chan_spec *chan;
	int i;

	indio_dev->num_channels = channels;
	chan_array = kcalloc(channels,
			sizeof(struct iio_chan_spec), GFP_KERNEL);
	if (chan_array == NULL)
		return -ENOMEM;

	chan = chan_array;
	for (i = 0; i < channels; i++, chan++) {

		chan->type = IIO_VOLTAGE;
		chan->indexed = 1;
		chan->channel = adc_dev->channel_line[i];
		chan->info_mask_separate = BIT(IIO_CHAN_INFO_RAW);
		chan->datasheet_name = chan_name_ain[chan->channel];
		chan->scan_index = i;
		chan->scan_type.sign = 'u';
		chan->scan_type.realbits = 12;
		chan->scan_type.storagebits = 16;
	}

	indio_dev->channels = chan_array;

	return 0;
}

static void tiadc_channels_remove(struct iio_dev *indio_dev)
{
	kfree(indio_dev->channels);
}

static int tiadc_read_raw(struct iio_dev *indio_dev,
		struct iio_chan_spec const *chan,
		int *val, int *val2, long mask)
{
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	int i, map_val;
	unsigned int fifo1count, read, stepid;
	bool found = false;
	u32 step_en;
    int value_average = 0;
	unsigned long timeout;
    unsigned int data = 0;
    int data_count = 10;
    //printk("-----ADC---channel--%d--channel2--%d\n",chan->channel,chan->channel2);
	if (iio_buffer_enabled(indio_dev))
		return -EBUSY;

	step_en = get_adc_chan_step_mask(adc_dev, chan);
	if (!step_en)
		return -EINVAL;

    mutex_lock(&mutex);

    while(data_count--)
    {

        fifo1count = tiadc_readl(adc_dev, REG_FIFO1CNT);
        while (fifo1count--)
            tiadc_readl(adc_dev, REG_FIFO1);

        am335x_tsc_se_set_once(adc_dev->mfd_tscadc, step_en);

        timeout = jiffies + usecs_to_jiffies
                    (IDLE_TIMEOUT * adc_dev->channels);
        /* Wait for Fifo threshold interrupt */
        while (1) {
            fifo1count = tiadc_readl(adc_dev, REG_FIFO1CNT);
            if (fifo1count)
                break;

            if (time_after(jiffies, timeout)) {
                am335x_tsc_se_adc_done(adc_dev->mfd_tscadc);
                mutex_unlock(&mutex);
                return -EAGAIN;
                
            }
        }
        map_val = adc_dev->channel_step[chan->scan_index];

        /*
         * We check the complete FIFO. We programmed just one entry but in case
         * something went wrong we left empty handed (-EAGAIN previously) and
         * then the value apeared somehow in the FIFO we would have two entries.
         * Therefore we read every item and keep only the latest version of the
         * requested channel.
         */
        for (i = 0; i < fifo1count; i++) {
            read = tiadc_readl(adc_dev, REG_FIFO1);
            stepid = read & FIFOREAD_CHNLID_MASK;
            stepid = stepid >> 0x10;

            if (stepid == map_val) {
                read = read & FIFOREAD_DATA_MASK;
                found = true;
                *val = (u16) read;
               // printk("-----adc_test----:fifo1count = %d --read = %d \n",fifo1count,read);
            }
        }
        data += read;
        am335x_tsc_se_adc_done(adc_dev->mfd_tscadc);

        if (found == false)
        {
            mutex_unlock(&mutex);
            return -EBUSY;
            
        }
    }


    value_average = (u16)data/10; 


    // x = (y-b)/k  .....    ->x = (y-y1)(x2-x1)/(y2-y1) + yx;
    for(i =0;i<8;i++)
    {
        if(chan->channel == i)
        {
            if((d2out[i] - d1out[i] ==0) || (d2out[i] ==0) ||( d1out[i] ==0) ||(d2in[i] == 0) || (d1in[i] ==0))
            {
                *val = value_average;
            }else
            {
                *val = (value_average - d1out[i]) * (d2in[i]-d1in[i]) / (d2out[i] - d1out[i]) + d1in[i];
            }
            if(*val< 50)
            {
                *val = 0;
            }
            break;
        }
    }
        mutex_unlock(&mutex);
	return IIO_VAL_INT;
}

static const struct iio_info tiadc_info = {
	.read_raw = &tiadc_read_raw,
	.driver_module = THIS_MODULE,
};

static int tiadc_parse_dt(struct platform_device *pdev,
			  struct tiadc_device *adc_dev)
{
	struct device_node *node = pdev->dev.of_node;
	struct property *prop;
	const __be32 *cur;
	int channels = 0;
	u32 val;

	of_property_for_each_u32(node, "ti,adc-channels", prop, cur, val) {
		adc_dev->channel_line[channels] = val;

		/* Set Default values for optional DT parameters */
		adc_dev->open_delay[channels] = STEPCONFIG_OPENDLY;
		adc_dev->sample_delay[channels] = STEPCONFIG_SAMPLEDLY;
		adc_dev->step_avg[channels] = 16;

		channels++;
	}

	of_property_read_u32_array(node, "ti,chan-step-avg",
				   adc_dev->step_avg, channels);
	of_property_read_u32_array(node, "ti,chan-step-opendelay",
				   adc_dev->open_delay, channels);
	of_property_read_u32_array(node, "ti,chan-step-sampledelay",
				   adc_dev->sample_delay, channels);

	adc_dev->channels = channels;
	return 0;
}

static int tiadc_probe(struct platform_device *pdev)
{
	struct iio_dev		*indio_dev;
	struct tiadc_device	*adc_dev;
	struct device_node	*node = pdev->dev.of_node;
	int			err;

	if (!node) {
		dev_err(&pdev->dev, "Could not find valid DT data.\n");
		return -EINVAL;
	}

	indio_dev = devm_iio_device_alloc(&pdev->dev,
					  sizeof(struct tiadc_device));
	if (indio_dev == NULL) {
		dev_err(&pdev->dev, "failed to allocate iio device\n");
		return -ENOMEM;
	}
	adc_dev = iio_priv(indio_dev);

	adc_dev->mfd_tscadc = ti_tscadc_dev_get(pdev);
	tiadc_parse_dt(pdev, adc_dev);

	indio_dev->dev.parent = &pdev->dev;
	indio_dev->name = dev_name(&pdev->dev);
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &tiadc_info;

	tiadc_step_config(indio_dev);
	tiadc_writel(adc_dev, REG_FIFO1THR, FIFO1_THRESHOLD);

	err = tiadc_channel_init(indio_dev, adc_dev->channels);
	if (err < 0)
		return err;

	err = tiadc_iio_buffered_hardware_setup(indio_dev,
		&tiadc_worker_h,
		&tiadc_irq_h,
		adc_dev->mfd_tscadc->irq,
		IRQF_SHARED,
		&tiadc_buffer_setup_ops);

	if (err)
		goto err_free_channels;

	err = iio_device_register(indio_dev);
	if (err)
		goto err_buffer_unregister;

	platform_set_drvdata(pdev, indio_dev);
    adc_calibration_init(indio_dev);
    mutex_init(&mutex);

	return 0;

err_buffer_unregister:
	tiadc_iio_buffered_hardware_remove(indio_dev);
err_free_channels:
	tiadc_channels_remove(indio_dev);
	return err;
}

static int tiadc_remove(struct platform_device *pdev)
{
	struct iio_dev *indio_dev = platform_get_drvdata(pdev);
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	u32 step_en;

    kobject_del(&adc_calibration->kobj);
    kobject_put(&adc_calibration->kobj);
	iio_device_unregister(indio_dev);
	tiadc_iio_buffered_hardware_remove(indio_dev);
	tiadc_channels_remove(indio_dev);

	step_en = get_adc_step_mask(adc_dev);
	am335x_tsc_se_clr(adc_dev->mfd_tscadc, step_en);

	return 0;
}

#ifdef CONFIG_PM
static int tiadc_suspend(struct device *dev)
{
	struct iio_dev *indio_dev = dev_get_drvdata(dev);
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	struct ti_tscadc_dev *tscadc_dev;
	unsigned int idle;

	tscadc_dev = ti_tscadc_dev_get(to_platform_device(dev));
	if (!device_may_wakeup(tscadc_dev->dev)) {
		idle = tiadc_readl(adc_dev, REG_CTRL);
		idle &= ~(CNTRLREG_TSCSSENB);
		tiadc_writel(adc_dev, REG_CTRL, (idle |
				CNTRLREG_POWERDOWN));
	}

	return 0;
}

static int tiadc_resume(struct device *dev)
{
	struct iio_dev *indio_dev = dev_get_drvdata(dev);
	struct tiadc_device *adc_dev = iio_priv(indio_dev);
	unsigned int restore;

	/* Make sure ADC is powered up */
	restore = tiadc_readl(adc_dev, REG_CTRL);
	restore &= ~(CNTRLREG_POWERDOWN);
	tiadc_writel(adc_dev, REG_CTRL, restore);

	tiadc_step_config(indio_dev);
	am335x_tsc_se_set_cache(adc_dev->mfd_tscadc,
			adc_dev->buffer_en_ch_steps);
	return 0;
}

static const struct dev_pm_ops tiadc_pm_ops = {
	.suspend = tiadc_suspend,
	.resume = tiadc_resume,
};
#define TIADC_PM_OPS (&tiadc_pm_ops)
#else
#define TIADC_PM_OPS NULL
#endif

static const struct of_device_id ti_adc_dt_ids[] = {
	{ .compatible = "ti,am3359-adc", },
	{ }
};
MODULE_DEVICE_TABLE(of, ti_adc_dt_ids);

static struct platform_driver tiadc_driver = {
	.driver = {
		.name   = "TI-am335x-adc",
		.pm	= TIADC_PM_OPS,
		.of_match_table = ti_adc_dt_ids,
	},
	.probe	= tiadc_probe,
	.remove	= tiadc_remove,
};
module_platform_driver(tiadc_driver);

MODULE_DESCRIPTION("TI ADC controller driver");
MODULE_AUTHOR("Rachna Patil <rachna@ti.com>");
MODULE_LICENSE("GPL");
