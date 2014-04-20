#include "mytreeview.h"
#include "osinterface.h"
#include "mydialog.h"
#include "mainhandler.h"
#include "functions.h"
#include "types.h"
#include "stylesheets.h"
#include <QKeyEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QPersistentModelIndex>
#include <QAbstractItemView>
#include <QBrush>
#include <QStandardItem>
#include <iostream>
#include <sstream>
#include <string>

/* vrati index prave oznaceneho prvku */

int MyTreeView::getSelIdx(){
    return currentIndex().row();
}

/* zpracuje ziskani focusu, pripadne oznaci prvni prvek */

void MyTreeView::focusInEvent(QFocusEvent *){
    emit(focused());
    if(model()){
        if(selectedIndexes().empty()){
            QPersistentModelIndex sel_idx = indexAt(QPoint(0, 0));
            setCurrentIndex(sel_idx);
        }
    }
    focus();
}

/*
 * zpracuje polozku ve stromu
 * podle informaci o souboru
 */

void MyTreeView::addItem(QTreeWidgetItem *item, dirEntryT *e){
    std::shared_ptr<Data> data_instance = Data::getInstance();

    std::stringstream ss;

    item->setText(0, QString::fromStdString(e->name));
    item->setIcon(0, base_icon);
    if(e->type == e->DIR){
        item->setIcon(0, dir_icon);
        item->setFont(0, bold_font);
    }else if(e->type == e->LINK){
        item->setFont(0, italic_font);
        item->setForeground(0, QBrush(QColor(255, 0, 0)));
    }else if(e->type == e->ARCHIVE){
        item->setIcon(0, ar_icon);
        item->setFont(0, bold_font);
        item->setForeground(0, QBrush(QColor(255, 0, 255)));
    }else item->setFont(0, base_font);

    item->setText(1, QString::fromStdString(e->type_name));
    item->setFont(1, italic_font);
    if(e->type == e->DIR) item->setFont(0, bold_font);
    ss << round((e->byte_size / pow(1024, data_instance->size_in)));
    addSizeInfo(ss);
    item->setText(2, QString::fromStdString(ss.str()));
}

/* rekurzivne vytvori strom
 * root je absolutni cesta k pripojovanemu adresari, it je ukazatel na rodicovskou polozku
 * pokud je top true, znamena to, ze se ma zanorit
 * vzdy prochazi dany adresar a pripojuje nalezene soubory, pripadne rekurzivne jejich podstromy
 * rekurzivni volani je vsak jen do hloubky 1, kvuli rychlosti, vzdy tedy predpocita jen dalsi patro.
 * rekurze take zavisi na nastaveni priznaku recursive - rozdil mezi LIST a TREE
 */

void MyTreeView::buildTree(std::string root, QTreeWidgetItem *it, bool top){
    OSInterface *os;
    if(!recursive)
        os = osi;
    else{
        os = new OSInterface();
        try{
            os->getDirInfo(root, pattern);
        }catch(OSException *e){
            std::cout << e->what() << std::endl;
            return;
        }
    }
    if(root[root.size() - 1] == OSInterface::dir_sep)
        root = root.substr(0,root.size() - 1);
    QTreeWidgetItem *item;

    for(auto &e : os->dirs){
        if(e->name.empty()) continue;
        if((e->name == ".") || (e->name == "..")) continue;
        if(it != nullptr) // ma predka
            item = new QTreeWidgetItem();
        else // vrchni uroven
            item = new QTreeWidgetItem(this);
        addItem(item, e);
        if(recursive && top){
            if(e->type == e->DIR){
                buildTree(root + OSInterface::dir_sep + e->name, item, false);
            }
        }
        if(it != nullptr){
            it->addChild(item);
            it->setExpanded(false);
        }
    }
    if(recursive) delete os;
}

/* prebuduje obsah seznamu
 * postavi novy strom, nastavi parametry
 * oznaci prvek na radku danem promennou idx - slouzi jako pamet pri zmenach
 */

void MyTreeView::rebuild(int idx){
    /*
     * if(osi == nullptr) osi = new OSInterface();
    osi->dirs.clear();
    try{
        osi->getDirInfo(path, pattern);
    }catch(OSException *e){
        std::cout << e->what() << std::endl;
        stepup();
    }
    */
    clear();
    buildTree(path, nullptr, true);
    setEditTriggers(QTreeWidget::NoEditTriggers);
    setAlternatingRowColors(false);
    setSortingEnabled(false);
    setSelectionBehavior(QTreeWidget::SelectRows);
    setHeaderHidden(true);
    setColumnWidth(0, 280);
    if(!osi->dirs.empty()){
        QPersistentModelIndex nextIndex = model()->index(idx,0);
        selectionModel()->setCurrentIndex(nextIndex, QItemSelectionModel::SelectCurrent);
        selectionModel()->select(nextIndex, QItemSelectionModel::ClearAndSelect);
    }
}

/* zpracovani udalosti */

void MyTreeView::focusOutEvent(QFocusEvent *){
    unFocus();
}

void MyTreeView::resizeEvent(QResizeEvent *e)
{
    w = e->size().width();
    if(w)
        emit(rebuilded());
}

void MyTreeView::setFocus(){ QTreeWidget::setFocus(); }

/* vrati absolutni cesty k prave oznacenemu souboru */

std::string MyTreeView::getSelected(){
    repairPath(path);
    if(currentItem())
        return path + currentItem()->text(0).toStdString() ;
    else return "";
}

/* vrati index polozky s danym textem */

int MyTreeView::getIdxOf(std::string &name){
    if (name[name.size() - 1] == OSInterface::dir_sep)
        name = name.substr(0, name.size() - 1);
    auto a = findItems(QString::fromStdString(name),Qt::MatchExactly);
    if(a.size()){
        return indexFromItem(a[0]).row();
    }
    return 0;
}

/* prida (odebere) oznaceny soubor do (z) vyberu */

void MyTreeView::changeSelection(){
    std::string selected = getSelected();
    if(multi_selection.find(selected) != multi_selection.end()){
        multi_selection.erase(selected);
    }else{
        multi_selection.insert(selected);
    }
    updateSelection();
}

/* de facto prestane byt viditelny, zrusen bude pozdeji */

void MyTreeView::die(){
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(0, 0);
}

/* zpracovani stisku klavesy */

void MyTreeView::keyPressEvent(QKeyEvent *e){
    size_t size;
    switch (e->key()) {
    case Qt::Key_Escape:
        multi_selection.clear();
        updateSelection();
        break;
    case Qt::Key_Insert:
        mark(!marked);
        break;
    case Qt::Key_Shift:
        if(!recursive)
            changeSelection();
        else{
            std::string msg("Selecting items is not allowed in tree mode.");
            MyDialog::MsgBox(msg);
        }
        break;
    case Qt::Key_Backspace:
        emit(stepup());
        setFocus();
        break;
    case Qt::Key_F1:
        emit(chlayout());
        break;
    case Qt::Key_F11:
        emit(refresh());
        break;
    case Qt::Key_Down:
    case Qt::Key_Up:
        QTreeWidget::keyPressEvent(e);
        if(e->modifiers() & Qt::ShiftModifier)
            changeSelection();
        break;
    case Qt::Key_Space:
        try{
        size = OSInterface::computeDirSize(getSelected());
        std::stringstream ss;
        std::shared_ptr<Data> data_instance = Data::getInstance();
        ss << round((size / pow(1024, data_instance->size_in)));
        addSizeInfo(ss);

        currentItem()->setText(2, QString::fromStdString(ss.str()));
    }catch(OSException *e) { std::cout << e->what() << std::endl; }
        break;
    default:
        QTreeWidget::keyPressEvent(e);
        break;
    }
}

/* projde vsechny prvky, pokud jsou vybrane, prislusne je zvyrazni,
 * jinak do normalu
 */

void MyTreeView::updateSelection(){
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
    for(int i = 0; i < topLevelItemCount(); ++i){
        if(topLevelItem(i) != nullptr){
            if(multi_selection.find(path + topLevelItem(i)->text(0).toStdString()) != multi_selection.end()){
                topLevelItem(i)->setBackground(0,brb);
                topLevelItem(i)->setFont(0, sel_font);
                topLevelItem(i)->setBackground(1,brb);
                topLevelItem(i)->setBackground(2,brb);
            }
            else{
                std::string file = path + topLevelItem(i)->text(0).toStdString();
                if((OSInterface::isDir(file)) || (isArch(file)))
                    base_font.setBold(true);
                topLevelItem(i)->setBackground(0,brw);
                topLevelItem(i)->setBackground(1,brw);
                topLevelItem(i)->setBackground(2,brw);
                topLevelItem(i)->setFont(0, base_font);
                base_font.setBold(false);
            }
        }
    }
}



/* zmena vzhledu pri focusu */

void MyTreeView::focus(){
    is_focused = true;
    setStyleSheet(focused_list_style);
    updateSelection();
}

/* zmena vzhledu pri oznaceni */

void MyTreeView::mark(bool m){
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

void MyTreeView::unFocus(){
    is_focused = false;
    if(!marked){
        setStyleSheet(unfocused_list_style);
        updateSelection();
    }else mark(true);
}
