/*
 * Copyright (C) 2011 Andreas Steffen
 * HSR Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "tnc_ifmap_plugin.h"
#include "tnc_ifmap_listener.h"

#include <daemon.h>
 
typedef struct private_tnc_ifmap_plugin_t private_tnc_ifmap_plugin_t;

/**
 * private data of tnc_ifmap plugin
 */
struct private_tnc_ifmap_plugin_t {

	/**
	 * implements plugin interface
	 */
	tnc_ifmap_plugin_t public;

	/**
	 * Listener interface, listens to CHILD_SA state changes
	 */
	tnc_ifmap_listener_t *listener;
};

METHOD(plugin_t, get_name, char*,
	private_tnc_ifmap_plugin_t *this)
{
	return "tnc-ifmap";
}

METHOD(plugin_t, reload, bool,
	private_tnc_ifmap_plugin_t *this)
{
	if (this->listener)
	{
		charon->bus->remove_listener(charon->bus, &this->listener->listener);
		this->listener->destroy(this->listener);
	}

	this->listener = tnc_ifmap_listener_create();
	if (!this->listener)
	{
		return FALSE;
	}

	charon->bus->add_listener(charon->bus, &this->listener->listener);
	return TRUE;
}

METHOD(plugin_t, destroy, void,
	private_tnc_ifmap_plugin_t *this)
{
	if (this->listener)
	{
		charon->bus->remove_listener(charon->bus, &this->listener->listener);
		this->listener->destroy(this->listener);
	}
	free(this);
}

/*
 * see header file
 */
plugin_t *tnc_ifmap_plugin_create()
{
	private_tnc_ifmap_plugin_t *this;

	INIT(this,
		.public = {
			.plugin = {
				.get_name = _get_name,
				.reload = _reload,
				.destroy = _destroy,
			},
		},
		.listener = tnc_ifmap_listener_create(),
	);

	if (this->listener)
	{
		charon->bus->add_listener(charon->bus, &this->listener->listener);
	}
	return &this->public.plugin;
}

