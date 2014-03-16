#include "mainwindow.h"
#include <QApplication>
int size_in, max_lists, col_count, init_count;
std::string init_dir, home_path;
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
