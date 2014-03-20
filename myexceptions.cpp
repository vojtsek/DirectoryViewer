#include "myexceptions.h"
#include "mydialog.h"

#include <QMessageBox>
#include <string>
#include <iostream>

/* rodicovska trida pro implementaci vyjimek */

const char *MyException::what(){
    return err.c_str();
}

void MyException::process(){
    std::cout << what() << std::endl;
    MyDialog::MsgBox(err);
}

