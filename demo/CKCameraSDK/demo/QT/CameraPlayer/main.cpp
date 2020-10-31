#include "cameraplayer.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMessageBox::information(nullptr, "TIP", "please run the application by root!!!");

    CameraPlayer w;
    w.show();

    return a.exec();
}
