#include "archiveviewer.h"
#include "osinterface.h"
#include "mydialog.h"
#include "types.h"
#include "stylesheets.h"
#include "functions.h"

#include <QKeyEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPersistentModelIndex>
#include <QAbstractItemView>
#include <QBrush>
#include <QStandardItem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

int ArchiveViewer::getSelIdx(){
  return currentIndex().row();
}

void ArchiveViewer::focusInEvent(QFocusEvent *e){
  emit(focused());
  if(model()){
      if(selectedIndexes().empty()){
          QPersistentModelIndex sel_idx = indexAt(QPoint(0, 0));
          setCurrentIndex(sel_idx);
        }
    }
  focus();
}

void ArchiveViewer::buildTree(QTreeWidgetItem *it, int idx){
    QIcon dir_icon("icons/folder-open-green.png");
    QIcon base_icon("icons/doc-plain-green.png");
  QFont base_font, bold_font, italic_font;
  italic_font.setFamily("Verdana");
  italic_font.setItalic(true);
  bold_font.setBold(true);
  bold_font.setFamily("Verdana");
  base_font.setFamily("Verdana");
  std::string name, prefix, tmp;
  char last;
  int pos;
  try{
      prefix = name = items.at(idx);
  }catch(std::exception e) { return; }
  QTreeWidgetItem *item;
      std::stringstream ss;
      if(it != nullptr)
        item = new QTreeWidgetItem();
      else
        item = new QTreeWidgetItem(this);
      if(name[name.size() - 1] == '/')
          name = name.substr(0, name.size() - 1);
      pos = name.find_last_of('/');
      if(pos != std::string::npos)
          name = name.substr(pos + 1, name.size());
      item->setText(0, QString::fromStdString(name));
      item->setIcon(0, base_icon);
      while(1){
          if(++idx >= items.size()) break;
          try{
              name = items.at(idx);
          }catch(std::exception e) { break; }
          if(name.find(prefix) != 0) break;
          last = name[name.size() - 1];
          if(name != prefix)
              name = name.substr(prefix.size(), name.size() - prefix.size() - 1);
          if(name.find('/') != std::string::npos) continue;
          name.push_back(last);
          buildTree(item, idx);
      }
      if(it != nullptr){
          it->setIcon(0, dir_icon);
          it->setFont(0, bold_font);
          it->addChild(item);
          it->setExpanded(false);
      }
}

void ArchiveViewer::readArch(){
    std::string ext = getExtension(path);
    if (ext == "zip"){
        char *buf = new char[4], *fn;
        int32_t *ptr_32 = (int32_t *) buf;
        int16_t *ptr_16 = (int16_t *) buf;
        int32_t socd;
        int16_t fn_length, extra_length, comment_length;
        int cur_off;
        off_t offset = -4;
        std::ifstream file(path, std::ios::in | std::ios::binary);
        do{
            file.seekg (offset, std::ios::end);
            file.read(buf, 4);
            offset -= 1;
        }while(*ptr_32 != 0x06054b50);

        file.seekg (12, std::ios::cur);
        file.read(buf, 4);
        socd = *ptr_32;
        cur_off = socd + 28;

        do{
            file.seekg (cur_off, std::ios::beg);
            file.read(buf, 2);
            fn_length = *ptr_16;
            file.read(buf, 2);
            extra_length = *ptr_16;
            file.read(buf, 2);
            comment_length = *ptr_16;
            cur_off += 18; //preskocit nejake dalsi info
            fn = new char[fn_length + 1];
            file.seekg(cur_off, std::ios::beg);
            file.read(fn, fn_length);
            fn[fn_length] = '\0';
            items.push_back(fn);
            delete fn;
            cur_off += fn_length + extra_length + comment_length; //do dalsiho zaznamu
            file.seekg(cur_off, std::ios::beg);
            file.read(buf, 4); //precte pro kontrolu
            cur_off += 28;
            //file.seekg(-4, std::ios::cur); //vrati zpet

        }while(*ptr_32 != 0x06054b50);
    }
}

void ArchiveViewer::rebuild(){
    readArch();
  clear();
  buildTree(nullptr, 0);
  setEditTriggers(QTreeWidget::NoEditTriggers);
  setAlternatingRowColors(false);
  setSortingEnabled(false);
  setSelectionBehavior(QTreeWidget::SelectRows);
  setHeaderHidden(true);
  setColumnWidth(0, 280);
}

void ArchiveViewer::focusOutEvent(QFocusEvent *e){
  unFocus();
}

void ArchiveViewer::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
      emit(rebuilded());
}

void ArchiveViewer::setFocus(){ QTreeWidget::setFocus(); }

std::string ArchiveViewer::getSelected(){
  if(currentItem())
  return path + OSInterface::dir_sep + currentItem()->text(0).toStdString() ;
  else return "";
}

void ArchiveViewer::die(){
  setFocusPolicy(Qt::NoFocus);
  setFixedSize(0, 0);
}

void ArchiveViewer::keyPressEvent(QKeyEvent *e){
    if(e->key() == Qt::Key_Insert)
        mark(!marked);
    else if(e->key() == Qt::Key_Shift){
        std::string msg("Selecting items is not allowed in archives");
        MyDialog::MsgBox(msg);
    }else if(e->key() == Qt::Key_Backspace){
        emit(chlayout());
        setFocus();
    }else if(e->key() == Qt::Key_Return){
        currentItem()->setExpanded(!currentItem()->isExpanded());
    }else if(e->key() == Qt::Key_F1)
        emit(chlayout());
    else
        QTreeWidget::keyPressEvent(e);
}


void ArchiveViewer::focus(){
  is_focused = true;
  setStyleSheet(focused_list_style);
}

void ArchiveViewer::mark(bool m){
  if(m){ //oznacit
      marked = true;
      setStyleSheet(marked_list_style);
    }else{ //odznacit
      marked = false;
      setStyleSheet(focused_list_style);
    }
}

QWidget *ArchiveViewer::getContent(){
  return (QWidget *) this;
}

void ArchiveViewer::unFocus(){
  is_focused = false;
  if(!marked){
      setStyleSheet(unfocused_list_style);
  }else mark(true);
}
