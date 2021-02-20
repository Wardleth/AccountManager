#include "Qt/QtApplication.h"

int main(int argc, char *argv[])
{
    QtApplication app(argc, argv);
    app.display();
    return app.execute();
}
