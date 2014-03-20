#include "myiconview.h"
#include "types.h"
#include "functions.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QHeaderView>
#include <QBrush>
#include <iostream>
#include <sstream>
#include <string>

extern std::string home_path;

/* vrati index oznaceneho prvku, prepocitany z mrizky */

int MyIconView::getSelIdx(){
    return currentItem()->row() * columnCount() + currentItem()->column();
}

/* vrati index polozky s danym textem */

int MyIconView::getIdxOf(std::string &name){
    if (name[name.size() - 1] == OSInterface::dir_sep)
        name = name.substr(0, name.size() - 1);
    auto a = findItems(QString::fromStdString(name),Qt::MatchExactly);
    if(a.size()){
        return indexFromItem(a[0]).row() * columnCount() + indexFromItem(a[0]).column();
    }
    return 0;
}

/* zpracovani focusu */

void MyIconView::focusInEvent(QFocusEvent *e){
    emit(focused());
    focus();
}

void MyIconView::focusOutEvent(QFocusEvent *e){
    unFocus();
}

/* pri zmene velikosti si ji ulozi a zasignalizuje, prepocita se pocet sloupcu */

void MyIconView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
        emit(rebuilded());
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

/* vrati validni absolutni cestu k prave oznacenemu souboru */

std::string MyIconView::getSelected(){
    repairPath(path);
    if(currentItem())
        return path + currentItem()->text().toStdString() ;
    else return "";
}

/* prebudovani obsahu
 * nacte informace o souborech v adresari
 * spocita a nastavi si parametry
 * vytvori a prida polozky
 */

void MyIconView::rebuild(int idx){
    clear();
    if(osi == nullptr) osi = new OSInterface();
    osi->dirs.clear();
    try{
        osi->getDirInfo(path, pattern);
    }catch(OSException *e){
        std::cout << e->what() << std::endl;
    }

    int cols = w / col_width;
    setRowCount(osi->dirs.size() / cols + 1);
    setColumnCount(cols);
    setIconSize(QSize(28,28));
    for(int i = 0; i < cols; ++i){
        setColumnWidth(i, col_width - 1);
    }
    QTableWidgetItem *item;
    QIcon dir_icon(QString::fromStdString(home_path + OSInterface::dir_sep + "icons/folder-open-blue.png"));
    QIcon ar_icon(QString::fromStdString(home_path + OSInterface::dir_sep +  "icons/database.png"));
    QIcon base_icon(QString::fromStdString(home_path + OSInterface::dir_sep + "icons/doc-plain-blue.png"));
    QFont base_font, bold_font, italic_font;
    italic_font.setFamily("Verdana");
    italic_font.setItalic(true);
    bold_font.setBold(true);
    bold_font.setFamily("Verdana");
    base_font.setFamily("Verdana");
    int row = 0, col = 0;
    if(path[path.size() - 1] == '/')
        path = path.substr(0,path.size() - 1);
    for(auto &e : osi->dirs){
        if((e->name == ".") || (e->name == "..")) continue;
        std::stringstream ss;
        item = new QTableWidgetItem(QString::fromStdString(e->name));
        item->setIcon(base_icon);
        if(e->type == e->DIR){
            item->setIcon(dir_icon);
            item->setFont(bold_font);
        }else if(e->type == e->LINK){
            item->setFont(italic_font);
            item->setForeground(QBrush(QColor(255, 0, 0)));
        }else if(e->type == e->ARCHIVE){
            item->setIcon(ar_icon);
            item->setFont(bold_font);
            item->setForeground(QBrush(QColor(255, 0, 255)));
        }else item->setFont(base_font);
        setItem(row, col, item);
        col = (col + 1) % cols;
        if(!col)
            ++row;
    }
    for(int c = col; c < cols; ++c){
        item = new QTableWidgetItem("");
        item->setFlags(Qt::NoItemFlags);
        setItem(row, c, item);
    }
    horizontalHeader()->setVisible(false);
    verticalHeader()->setVisible(false);
    setShowGrid(false);
    /*    if(!osi->dirs.empty()){ //oznaceni prvniho prvku pri vzniku
        QPersistentModelIndex nextIndex = indexAt(QPoint(0, 0));
        selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
    }
    */
    if(!osi->dirs.empty()){
        row = idx / cols;
        col = idx % cols;
        QPersistentModelIndex nextIndex = model()->index(row, col);
        selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
       // selectionModel()->select(nextIndex, QItemSelectionModel::ClearAndSelect);
    }
}

/* projde vsechny prvky, pokud jsou vybrane, prislusne je zvyrazni,
 * jinak do normalu
 */

void MyIconView::updateSelection(){
    QBrush brb(QColor(150, 200, 255));
    QBrush brw;
    if(marked)
        brw = QBrush(QColor(238, 255, 238));
    else
        brw = QBrush(QColor(255, 255, 255));
    QFont sel_font, base_font;
    base_font.setFamily("Verdana");
    sel_font.setFamily("Helvetica");
    sel_font.setUnderline(true);
    sel_font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    repairPath(path);
    for(int i = 0; i < rowCount(); ++i){
        for(int j = 0; j < columnCount(); ++j){
            if(item(i, j) != nullptr){
                if(multi_selection.find(path + item(i, j)->text().toStdString()) != multi_selection.end()){
                    item(i, j)->setBackground(brb);
                    item(i, j)->setFont(sel_font);
                }else{
                    if(OSInterface::isDir(path + OSInterface::dir_sep + item(i, j)->text().toStdString()))
                        base_font.setBold(true);
                    item(i, j)->setBackground(brw);
                    item(i, j)->setFont(base_font);
                    base_font.setBold(false);
                }
            }
        }
    }
}

/* zvoleny prvek prida do vyberu nebo ho naopak odebere */

void MyIconView::changeSelection(){
    std::string selected = getSelected();
    if(multi_selection.find(selected) != multi_selection.end()){
        multi_selection.erase(selected);
    }else{
        multi_selection.insert(selected);
    }
    updateSelection();
}

/* prestane prekazet, zrusen bude pozdeji */

void MyIconView::die(){
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(0, 0);
}

/* obsluha stisku klavesy */

void MyIconView::keyPressEvent(QKeyEvent *e){
    switch (e->key()) {
    case Qt::Key_Escape:
        multi_selection.clear();
        updateSelection();
        break;
    case Qt::Key_Insert:
        mark(!marked);
        break;
    case Qt::Key_Shift:
        changeSelection();
        break;
    case Qt::Key_Backspace:
        emit(stepup());
        setFocus();
        break;
    case Qt::Key_F1:
        emit(chlayout());
        break;
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
        QTableWidget::keyPressEvent(e);
        if(e->modifiers() & Qt::ShiftModifier)
            changeSelection();
        break;
    case Qt::Key_Tab:
        QWidget::keyPressEvent(e);
        break;
    default:
        QTableWidget::keyPressEvent(e);
        break;
    }
}

/* zmena vzhledu pri focusu */

void MyIconView::focus(){
    is_focused = true;
    setStyleSheet(focused_list_style);
    updateSelection();
}

/* zmena vzhledu pri oznaceni za cil */

void MyIconView::mark(bool m){
    if(m){ //oznacit
        marked = true;
        setStyleSheet(marked_list_style);
    }else{ //odznacit
        marked = false;
        setStyleSheet(focused_list_style);
    }
    updateSelection();
}

/* zmena vzhledu pri ztrate focusu */

void MyIconView::unFocus(){
    is_focused = false;
    if(!marked){
        setStyleSheet(unfocused_list_style);
        updateSelection();
    }else mark(true);
}
