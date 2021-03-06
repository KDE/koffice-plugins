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

#ifndef VARIABLE_DOCKER_WIDGET_H
#define VARIABLE_DOCKER_WIDGET_H

#include <QtGui/QDockWidget>
#include <KCanvasObserverBase.h>

class QPushButton;
class KLineEdit;
class KComboBox;
class KCanvasBase;
class KVariableManager;

class VariableDockerWidget : public QDockWidget, public KCanvasObserverBase {
	Q_OBJECT
	public:
		VariableDockerWidget(QWidget* parent = 0);
		~VariableDockerWidget();
		virtual void setCanvas(KCanvasBase* canvas);
	private:
		QPushButton* m_configure;
		QPushButton* m_addbutton;
		QPushButton* m_removebutton;
		KComboBox* m_name;
		KLineEdit* m_value;
		KCanvasBase* m_canvas;
		KVariableManager* m_vmanager;

	public slots:
		void variable_docker_submit();
		void variable_remove();
		void updateVariables();
		void updateValueField(const QString text);
};

#endif //VARIABLE_DOCKER_WIDGET_H

