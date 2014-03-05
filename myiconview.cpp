#include "myiconview.h"
#include "types.h"
#include "osinterface.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QHeaderView>
#include <QBrush>
#include <iostream>
#include <sstream>
#include <string>

int MyIconView::getSelIdx(){
  return currentItem()->row() * columnCount() + currentItem()->column();
}

void MyIconView::focusInEvent(QFocusEvent *e){
  emit(focused());
  focus();
}

void MyIconView::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void MyIconView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
      emit(rebuilded());
}

void MyIconView::setFocus(){ QWidget::setFocus(); }

std::string MyIconView::getSelected(){
  if(currentItem())
    return path + OSInterface::dir_sep + currentItem()->text().toStdString() ;
  else return "";
}

void MyIconView::rebuild(){
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
  setIconSize(QSize(25,25));
  for(int i = 0; i < cols; ++i){
      setColumnWidth(i, col_width - 1);
    }
  QTableWidgetItem *item;
  QIcon dir_icon("directory.png");
  QIcon arch_icon("archive.png");
  QIcon base_icon("file.png");
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
          item->setIcon(arch_icon);
        item->setFont(bold_font);
        item->setForeground(QBrush(QColor(255, 0, 255)));
      }else item->setFont(base_font);
      setItem(row, col, item);
      col = (col + 1) % cols;
      if(!col)
        ++row;
    }
  horizontalHeader()->setVisible(false);
  verticalHeader()->setVisible(false);
  setShowGrid(false);
  if(!osi->dirs.empty()){
      QPersistentModelIndex nextIndex = indexAt(QPoint(0, 0));
      selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
    }
}

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
  for(int i = 0; i < rowCount(); ++i){
      for(int j = 0; j < columnCount(); ++j){
          if(item(i, j) != nullptr){
              if(multi_selection.find(path + OSInterface::dir_sep + item(i, j)->text().toStdString()) != multi_selection.end()){
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

void MyIconView::changeSelection(){
  std::string selected = getSelected();
  if(multi_selection.find(selected) != multi_selection.end()){
    multi_selection.erase(selected);
  }else{
    multi_selection.insert(selected);
    }
  updateSelection();
}

void MyIconView::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

void MyIconView::keyPressEvent(QKeyEvent *e){
  if(e->key() == Qt::Key_Insert)
    mark(!marked);
  else if(e->key() == Qt::Key_Shift)
    changeSelection();
  else if(e->key() == Qt::Key_Backspace){
      emit(stepup());
      setFocus();
    }else if(e->key() == Qt::Key_F1)
    emit(chlayout());
  else if(e->key() == Qt::Key_Tab){
      QWidget::keyPressEvent(e);
  }else if((e->key() == Qt::Key_Down) || (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Right) || (e->key() == Qt::Key_Left)){
      QTableWidget::keyPressEvent(e);
      if(e->modifiers() & Qt::ShiftModifier)
        changeSelection();
    }else
    QTableWidget::keyPressEvent(e);
}

void MyIconView::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
  updateSelection();
}

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

void MyIconView::unFocus(){
  is_focused = false;
  if(!marked){
    setStyleSheet(unfocused_list_style);
    updateSelection();
  }else mark(true);
}

QWidget * MyIconView::getContent(){
  return (QWidget *) this;
}
