/* 
 *  indycam.c - IndyCam digital camera driver
 *
 *  Copyright (C) 2003 Ladislav Michl <ladis@linux-mips.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/sched.h>

#include <linux/videodev.h>
#include <linux/i2c.h>

#include "indycam.h"

#define VINO_ADAPTER	(I2C_ALGO_SGI | I2C_HW_SGI_VINO)

struct indycam {
	struct i2c_client *client;
	int version;
};

static const struct { 
	unsigned char addr;
	unsigned char val;
} initseq [] = {
	{ INDYCAM_CONTROL, INDYCAM_CONTROL_AGCENA },
	{ INDYCAM_SHUTTER, INDYCAM_SHUTTER_60 },
	{ INDYCAM_GAIN,	0x80 },
	{ INDYCAM_RED_BALANCE, 0x18 },
	{ INDYCAM_BLUE_BALANCE, 0xa4 },
	{ INDYCAM_RED_SATURATION, 0x80 },
	{ INDYCAM_BLUE_SATURATION, 0xc0 },
};

static struct i2c_driver i2c_driver_indycam;

static int indycam_attach(struct i2c_adapter *adap, int addr, int kind)
{
	int i, err = 0;
	struct indycam *camera;
	struct i2c_client *client;

	client = kmalloc(sizeof(*client), GFP_KERNEL);
	if (!client) 
		return -ENOMEM;
	camera = kmalloc(sizeof(*camera), GFP_KERNEL);
	if (!camera) {
		err = -ENOMEM;
		goto out_free_client;
	}

	client->data = camera;
	client->adapter = adap;
	client->addr = addr;
	client->driver = &i2c_driver_indycam;
	strcpy(client->name, "IndyCam client");			
	camera->client = client;

	err = i2c_attach_client(client);
	if (err)
		goto out_free_camera;

	err = i2c_smbus_read_byte_data(client, INDYCAM_VERSION);
	if (err != CAMERA_VERSION_INDY && err != CAMERA_VERSION_MOOSE) {
		err = -ENODEV;
		goto out_detach_client;
	}
	printk(KERN_INFO "Indycam v%d.%d detected.\n",
	       INDYCAM_VERSION_MAJOR(err),
	       INDYCAM_VERSION_MINOR(err));

	err = 0;
	for (i = 0; i < ARRAY_SIZE(initseq); i++)
		err |= i2c_smbus_write_byte_data(client, initseq[i].addr,
						 initseq[i].val);
	if (err)
		printk(KERN_INFO "IndyCam initalization failed\n");

	MOD_INC_USE_COUNT;
	return 0;

out_detach_client:
	i2c_detach_client(client);
out_free_camera:
	kfree(camera);
out_free_client:
	kfree(client);
	return err;
}

static int indycam_probe(struct i2c_adapter *adap)
{
	/* Indy specific crap */
	if (adap->id == VINO_ADAPTER)
		return indycam_attach(adap, INDYCAM_ADDR, 0);
	/* Feel free to add probe here :-) */
	return -ENODEV;
}

static int indycam_detach(struct i2c_client *client)
{
	struct indycam *camera = client->data;

	i2c_detach_client(client);
	kfree(camera);
	kfree(client);
	MOD_DEC_USE_COUNT;
	return 0;
}

static int indycam_command(struct i2c_client *client, unsigned int cmd,
			   void *arg)
{
	struct indycam *camera = client->data;

	switch (cmd) {

	default:
		return -EINVAL;
	}

	return 0;
}

static struct i2c_driver i2c_driver_indycam = {
	.name 		= "indycam",
	.id 		= I2C_DRIVERID_INDYCAM,
	.flags 		= I2C_DF_NOTIFY,
	.attach_adapter = indycam_probe,
	.detach_client 	= indycam_detach,
	.command 	= indycam_command,
};

static int indycam_init(void)
{
	return i2c_add_driver(&i2c_driver_indycam);
}

static void indycam_exit(void)
{
	i2c_del_driver(&i2c_driver_indycam);
}

module_init(indycam_init);
module_exit(indycam_exit);

MODULE_DESCRIPTION("SGI IndyCam driver");
MODULE_AUTHOR("Ladislav Michl <ladis@linux-mips.org>");
MODULE_LICENSE("GPL");
