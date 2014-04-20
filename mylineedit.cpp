#include "mylineedit.h"
#include "osinterface.h"
#include <QKeyEvent>
#include <iostream>
#include <string>

/* zpracovani focusu */

void MyLineEdit::focusInEvent(QFocusEvent *e){
    emit(focused(true));
    QLineEdit::focusInEvent(e);
}

void MyLineEdit::focusOutEvent(QFocusEvent *e){
    is_focused = false;
    emit(focused(false));
    QLineEdit::focusOutEvent(e);
}

/* zpracovani stisku klavesy
 * zajistuje,ze v pripade, ze uz tam je jen prefix cesty tak nepujde smazat
 * a take nejde vkladat vice oddelovacu za sebou
 */

void MyLineEdit::keyPressEvent(QKeyEvent *e){
    std::string sep;
    sep.push_back(OSInterface::dir_sep);
    if((selectedText().toStdString() == sep) && text().toStdString() == sep) return;
    std::string cont = text().toStdString();
    char key = (char) e->key();
    if((cont[cont.size() - 1] == OSInterface::dir_sep) && (key == OSInterface::dir_sep)) return;
    if(e->key() == Qt::Key_Backspace){
        if(text().toStdString() == OSInterface::getPrefix())
            setText(QString::fromStdString(OSInterface::getPrefix()));
        else
            QLineEdit::keyPressEvent(e);
        emit(focused(true));
    }else
        QLineEdit::keyPressEvent(e);
}

/* osetreni ziskavani focusu pri kliknuti mysi */

void MyLineEdit::mousePressEvent(QMouseEvent *e){
    if(is_focused)
        QLineEdit::mousePressEvent(e);
    else{
        emit(focused(true));
        is_focused = true;
    }
}
