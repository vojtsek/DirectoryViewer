#include "mainwindow.h"
#include <QApplication>
int size_in;
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
