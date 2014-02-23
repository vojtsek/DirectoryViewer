#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainhandler.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  MainHandler *handler;
  int focus_idx;
  void keyPressEvent(QKeyEvent *);
  void prepareLayout();

 public slots:
  void refreshMainLayout(bool all);
  void updateFocus();
  void confirm(std::string, cmd_info_T &);
  void error(std::string &);
};

#endif // MAINWINDOW_H
