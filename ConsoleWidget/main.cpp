#include "widgetconsole.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetConsole w;
    w.show();

    return a.exec();
}
