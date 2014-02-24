#include "myexceptions.h"

#include <QMessageBox>
#include <string>
#include <iostream>

const char *MyException::what(){
  return err.c_str();
}

void MyException::process(){
  std::cout << what() << std::endl;
  QMessageBox msg_box;
  msg_box.setWindowTitle("Error!");
  msg_box.setText(QString::fromStdString(err));
  msg_box.setStandardButtons(QMessageBox::Ok);
  msg_box.setDefaultButton(QMessageBox::Ok);
 // msg_box.exec();
}

