#include "Qt/QtApplication.h"

void QtApplication::display() {
	mainWindow_.show();
}

int QtApplication::execute() {
	return appImpl_.exec();
}

QtApplication::QtApplication(int argc, char* argv[]) : appImpl_(argc, argv) {

}