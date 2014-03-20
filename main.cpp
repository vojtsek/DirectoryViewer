#include "mainwindow.h"
#include <QApplication>
#include <map>
#include <string>

int size_in, max_lists, col_count, init_count;
std::string init_dir, home_path;
std::map<std::string, std::string> extern_programmes;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
