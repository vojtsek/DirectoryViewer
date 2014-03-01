#include "mydialog.h"
#include <QMessageBox>

MyDialog::MyDialog()
{
}

void MyDialog::MsgBox(std::string &str){
  QMessageBox msg_box;
  msg_box.setWindowTitle("Error!");
  msg_box.setText(QString::fromStdString(str));
  msg_box.setStandardButtons(QMessageBox::Ok);
  msg_box.setDefaultButton(QMessageBox::Ok);
  msg_box.exec();
}

std::string MyDialog::Prompt(std::string &str){
  QDialog dial;
  dial.exec();
}
