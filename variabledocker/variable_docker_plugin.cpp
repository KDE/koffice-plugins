/*
Copyright 2011 Robert Marmorstein <robert@narnia.homeunix.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "variable_docker_plugin.h"

#include "variable_docker_factory.h"

#include <kgenericfactory.h>
#include <KDockRegistry.h>

K_EXPORT_COMPONENT_FACTORY(variable_docker_plugin, KGenericFactory<VariableDockerPlugin>("VariableDockerPlugin"))

VariableDockerPlugin::VariableDockerPlugin(QObject * parent, const QStringList &) 
	: QObject(parent)
{
	KDockRegistry::instance()->add(new VariableDockerFactory(parent, "VariableDocker-plugin"));
}
#include <variable_docker_plugin.moc>
