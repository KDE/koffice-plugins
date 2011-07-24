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

#ifndef VARIABLE_DOCKER_FACTORY_H
#define VARIABLE_DOCKER_FACTORY_H 

#include <KDockFactoryBase.h>
#include <variable_docker_widget.h>

class VariableDockerFactory : public KDockFactoryBase{
	public:
		VariableDockerFactory(QObject* parent, const QString& dockerId) : KDockFactoryBase(parent, dockerId) {}
		QString id() const{
			return QString("VariableDockerFactory");
		}

		KDockFactoryBase::DockPosition defaultDockPosition() const{
			return KDockFactoryBase::DockTop;
		}

		bool isCollapsible() const{
			return false;
		}

		bool defaultCollapsed() const{
			return false;
		}

		VariableDockerWidget* createDockWidget();
};

#endif //VARIABLE_DOCKER_FACTORY_H
