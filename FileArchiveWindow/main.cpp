#include "file_archive_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FILE_ARCHIVE_WINDOW w;
    w.show();

    return a.exec();
}
