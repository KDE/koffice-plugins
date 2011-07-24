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

#ifndef VARIABLE_DOCKER_PLUGIN_H
#define VARIABLE_DOCKER_PLUGIN_H

#include <QtCore/QObject>

class VariableDockerPlugin : public QObject {
	Q_OBJECT
	public:
		VariableDockerPlugin(QObject* parent, const QStringList &list);
};
#endif //VARIABLE_DOCKER_PLUGIN_H
