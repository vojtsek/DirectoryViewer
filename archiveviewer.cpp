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

/* vrati index oznaceneho prvku, v tomto pripade je to hodnota ulozena pri vzniku, kvuli navratu */
int ArchiveViewer::getSelIdx(){
    return idx;
}

/* zpracuje udalost predani focusu, vola focus() */
void ArchiveViewer::focusInEvent(QFocusEvent *e){
    emit(focused());
    if(model()){
        if(selectedIndexes().empty()){ // na zacatku
            QPersistentModelIndex sel_idx = indexAt(QPoint(0, 0));
            setCurrentIndex(sel_idx);
        }
    }
    focus();
}

/* vytvori obsah tohoto prvku;
 * promenna idx ukazuje do vectoru items obsahujiciho jmena souboru z archivu.
 * prislusne soubory povesi pod *it;
 */
void ArchiveViewer::buildTree(QTreeWidgetItem *it, int idx){
    QIcon dir_icon(QString::fromStdString(home_path + OSInterface::dir_sep + "icons/folder-open-green.png"));
    QIcon base_icon(QString::fromStdString(home_path + OSInterface::dir_sep + "icons/doc-plain-green.png"));
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
    //std::stringstream ss;
    if(it != nullptr)
        item = new QTreeWidgetItem();
    else // top level
        item = new QTreeWidgetItem(this);
    if(name[name.size() - 1] == '/') //odstrani posledni lomitko
        name = name.substr(0, name.size() - 1);
    pos = name.find_last_of('/');
    if(pos != std::string::npos)
        name = name.substr(pos + 1, name.size()); //vyriznuti jmena souboru
    item->setText(0, QString::fromStdString(name));
    item->setIcon(0, base_icon);
    while(1){ // prochazi soubory patrici pod tento
        if(++idx >= items.size()) break;
        try{
            name = items.at(idx);
        }catch(std::exception e) { break; }
        if(name.find(prefix) != 0) break; // narazil na soubor, ktery uz nepatri pod tuto polozku - neobsahuje prefix (na zacatku)
        last = name[name.size() - 1];
        if(name != prefix)
            name = name.substr(prefix.size(), name.size() - prefix.size() - 1);
        if(name.find('/') != std::string::npos) continue; // mezi prefixem a souborem je netrivialni cesta - tato polozka bude pripojena rekurzivne pozdeji
        name.push_back(last);
        buildTree(item, idx); // vola se pro syny
    }
    if(it != nullptr){
        it->setIcon(0, dir_icon);
        it->setFont(0, bold_font);
        it->addChild(item);
        it->setExpanded(false);
    }
}

/* nacte jmena souboru v archivu do vectoru items, vcetne cest */

void ArchiveViewer::readArch(){
    std::string ext = getExtension(path);
    if (ext == "zip"){ // pro zip archivy
        char *buf = new char[4], *fn;
        int32_t *ptr_32 = (int32_t *) buf; //4B
        int16_t *ptr_16 = (int16_t *) buf; //2B
        int32_t socd;
        int16_t fn_length, extra_length, comment_length;
        int cur_off;
        off_t offset = -4;
        std::ifstream file(path, std::ios::in | std::ios::binary);
        do{ //cte od konce a hleda znacku SOCD
            file.seekg (offset, std::ios::end);
            file.read(buf, 4);
            offset -= 1;
        }while(*ptr_32 != 0x06054b50);

        file.seekg (12, std::ios::cur);
        file.read(buf, 4);
        socd = *ptr_32; // z prislusneho offsetu precte adresu SOCD
        cur_off = socd + 28; // v promenne cur_off je aktualni offset v archivu

        do{ //z prislusnych offsetu cte informace a jmena souboru, ktera prida do vectoru items
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
        }while(*ptr_32 != 0x06054b50); //do SOCD
    }
}

/* vyvola prebudovani obsahu - od zacatku */

void ArchiveViewer::rebuild(int idx){
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

/* obsluhy udalosti */

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

/* vrati jmeno aktualne oznaceneho souboru vcetne absolutni cesty */

std::string ArchiveViewer::getSelected(){
    if(currentItem())
        return path + OSInterface::dir_sep + currentItem()->text(0).toStdString() ;
    else return "";
}

/* prestane prekazet, zrusen bude pozdeji */

void ArchiveViewer::die(){
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(0, 0);
}

/* obsluha zmacknuti klavesy */
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
    else // zpracuj normalne
        QTreeWidget::keyPressEvent(e);
}

/* zmena stylu, priznak is_focused, vyvolava focusInEvent */

void ArchiveViewer::focus(){
    is_focused = true;
    setStyleSheet(focused_list_style);
}

/* pri oznaceni jako cil operace */

void ArchiveViewer::mark(bool m){
    if(m){ //oznacit
        marked = true;
        setStyleSheet(marked_list_style);
    }else{ //odznacit
        marked = false;
        setStyleSheet(focused_list_style);
    }
}

/* kvuli zmene stylu, vyvolava se z mainwindow */

void ArchiveViewer::unFocus(){
    is_focused = false;
    if(!marked){
        setStyleSheet(unfocused_list_style);
    }else mark(true);
}
