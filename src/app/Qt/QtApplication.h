#pragma once

#include "Application.h"

#include "mainwindow.h"

#include <QApplication>

class QtApplication : public Application
{
public:
	virtual void display() override;
	virtual int execute() override;

	QtApplication(int argc, char* argv[]);

private:
	QApplication appImpl_;
	MainWindow mainWindow_;
};
