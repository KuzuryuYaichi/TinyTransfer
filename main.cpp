#include <QCoreApplication>
#include "TinyInstance.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    TinyInstance tinyInstance;
    return a.exec();
}
