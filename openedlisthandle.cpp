#include "openedlisthandle.h"
#include "mainhandler.h"
#include "functions.h"
#include "mylineedit.h"
#include "myiconview.h"
#include "mytreeview.h"
#include "archiveviewer.h"
#include "myviewer.h"
#include "osinterface.h"
#include "stylesheets.h"
#include "types.h"

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <unistd.h>
#include <wait.h>
#include <QLineEdit>
#include <QKeyEvent>
#include <QToolBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QObject>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <sstream>

/* zvyrazni tlacitko odpovidajici zvolenemu typu rozlozeni */

void OpenedListHandle::highlightBtt(){
    for(auto &a : tool_btts){
        a.second.btt->setStyleSheet(btt_style);
        if((view_type == TREE) && (a.second.label == "Tree view"))
            a.second.btt->setStyleSheet(marked_btt_style);
        if((view_type == ICON) && (a.second.label == "Icon view"))
            a.second.btt->setStyleSheet(marked_btt_style);
        if((view_type == LIST) && (a.second.label == "List view"))
            a.second.btt->setStyleSheet(marked_btt_style);
    }
}

/* slouzi k promazani starych seznamu, ktere uz zavolaly die()
 * nemazou se hned, kvuli obsluze udalosti
 */

void OpenedListHandle::clean(){
    for(auto a : to_del){
        delete a;
    }
    to_del.clear();
}

/* znici nynejsi seznam, zaradi ho k destrukci
 * nacte o nem info, promaze predchozi seznamy
 * vytvori novy seznam a inicializuje ho - pripoji obsluhu signalu, postara se o oznaceni, focus a zkopirovani vyberu
 */

void OpenedListHandle::changeLayout(int type){
    clean();
    to_del.push_back(content);
    content->die();
    if((view_type == LIST) || (view_type == ICON))
        last_layout = view_type;
    view_type = type;
    std::set<std::string> sel = content->multi_selection;
    bool was_focused = content->is_focused;
    bool was_marked = content->marked;
    QWidgetItem *myItem;
    myItem = dynamic_cast <QWidgetItem*>(h_layout2->itemAt(0));
    h_layout2->removeWidget(myItem->widget());
    if((view_type == TREE) || (view_type == LIST))
        QObject::disconnect((MyTreeView *)content, 0, this, 0);
    switch (type) {
    case TREE:
        content = new MyTreeView(path, le2->text().toStdString(), true, os, content->getSelIdx());
        h_layout2->addWidget((MyTreeView *)content);
        break;
    case LIST:
        content = new MyTreeView(path, le2->text().toStdString(), false, os, content->getSelIdx());
        h_layout2->addWidget((MyTreeView *)content);
        break;
    case ICON:
        content = new MyIconView(path, le2->text().toStdString(), os, content->getSelIdx());
        h_layout2->addWidget((MyIconView *) content);
        break;
    case VIEW:
        content = new MyViewer(path, le2->text().toStdString(), content->getSelIdx());
        h_layout2->addWidget((MyViewer *)content);
        break;
    case ARCHIVE:
        content = new ArchiveViewer(path, content->getSelIdx());
        h_layout2->addWidget((ArchiveViewer *)content);
        break;
    }
    highlightBtt();
    if(was_marked)
        content->mark(true);
    content->multi_selection = sel;
    if(was_focused){
        content->setFocus();
        content->focus();
    }
    if((view_type == TREE) || (view_type == LIST))
        connectSignals<MyTreeView>();
    else if (view_type == ICON)
        connectSignals<MyIconView>();
    else if (view_type == VIEW)
        connectSignals<MyViewer>();
    else if (view_type == ARCHIVE)
        connectSignals<ArchiveViewer>();
    emit(updated());
    h_layout2->update();

}

/* zavolaji prislusnou zmenu seznamu */

void OpenedListHandle::toTree(){
    changeLayout(TREE);
}

void OpenedListHandle::toIcon(){
    changeLayout(ICON);
}

void OpenedListHandle::toList(){
    changeLayout(LIST);
}

/* slot, cyklicky strida seznamy (F1), pripadne pouzije posledni ulozeny (prechod z prohlizeni souboru) */

void OpenedListHandle::chlayout(){
    if(view_type == LIST)
        changeLayout(TREE);
    else if(view_type == TREE)
        changeLayout(ICON);
    else if(view_type == ICON)
        changeLayout(LIST);
    else
        changeLayout(last_layout);
}

void OpenedListHandle::refresh(){
    emit(refreshed(this));
}

/* slot, zmena vzoru */

void OpenedListHandle::patternChanged(){
    pattern = le2->text().toStdString();
    os->dirs.clear();
    os->getDirInfo(path, pattern);
    changeLayout(view_type);
}

/* slot, posun o adresar vyse
 * z cesty odsekne jeden adresar a zmeni obsah  vstupniho pole
 * to vyvola signal
 */

void OpenedListHandle::stepUp(){
    unsigned int it;
    std::string cutted, cont = le1->text().toStdString();
    if(cont == OSInterface::getPrefix()) return;
    cutted = cont.substr(0, cont.size() - 1);
    if((it = cutted.find_last_of(OSInterface::dir_sep)) == std::string::npos) return;
    last_dir = cont.substr(it + 1, cont.size());
    cont = cont.substr(0, it + 1);
    if(!cont.empty())
        le1->setText(QString::fromStdString(cont));
    else
        le1->setText(QString::fromStdString(OSInterface::getPrefix()));
    os->dirs.clear();
    os->getDirInfo(path, pattern);
    content->setFocus();
}

/*
 *  slot, pri zmene cesty ve vstupnim poli
 * prestavi seznam
 */

void OpenedListHandle::pathChanged(){
    if(OSInterface::isDir(le1->text().toStdString())){
        path = le1->text().toStdString();
        content->path = path;
        int idx(0);
        os->dirs.clear();
        os->getDirInfo(path, pattern);
        content->rebuild();
        if(!last_dir.empty())
            idx = content->getIdxOf(last_dir);
        content->rebuild(idx);
    }
    if(view_type == VIEW) changeLayout(LIST);
}

/* oznaci cast vstupniho pole s cestou, reprezentujici aktualni adresar */

void OpenedListHandle::setSelection(bool in){
    if(in){
        unsigned int it;
        std::string cont = le1->text().toStdString();
        if(cont == OSInterface::getPrefix()) return;
        if((it = cont.find_last_of(OSInterface::dir_sep)) == std::string::npos) return;
        le1->setSelection(it + 1,cont.size() - it );
    }else{
        le1->setText(QString::fromStdString(path));
    }
}

/* pri zmene velikosti, kvuli aktualizaci velikosti / poctu sloupcu */

void OpenedListHandle::rebuildContent(){
    if(view_type == ICON){
        content->rebuild(content->getSelIdx());
    }
    tb2->setMaximumWidth(content->w);
}

/* pri aktivaci polozky, rozhodne co s ni */

void OpenedListHandle::processItem(std::string new_path){
    if(OSInterface::isDir(new_path)){
        if(le1->text().toStdString() == OSInterface::getPrefix()){
            repairPath(new_path);
        }
        le1->setText(QString::fromStdString(new_path)); //signal
    }else if(isKnown(new_path)){
        OSInterface::openFile(new_path);
    }else if(OSInterface::isOpenable(new_path)){
        std::string old = path;
        path = new_path;
        changeLayout(VIEW);
        path = old;
    }else if(isArch(new_path)){
        std::string old = path;
        path = new_path;
        changeLayout(ARCHIVE);
        path = old;
    }
}

/* slot zajistujici zachyceni aktivace */

void OpenedListHandle::itemActivated(QTableWidgetItem *){
    std::string new_path = content->getSelected();
    processItem(new_path);
}

/* aktualizace informaci ve spodnim radku */

void OpenedListHandle::selectionChanged(){
    updateLbl();
}

/* slot zajistujici zachyceni aktivace */

void OpenedListHandle::itemActivated(QTreeWidgetItem *item, int){
    if(view_type == TREE){
        item->setExpanded(!item->isExpanded());
    }else{
        std::string new_path = content->getSelected();
        processItem(new_path);
    }
}

/*
 * ve vystupnim parametru p vrati cestu k adresari reprezentovanemu polozkou it
 */

void OpenedListHandle::getFullPath(QTreeWidgetItem *it, std::string &p){
    std::string tmp;
    QTreeWidgetItem *par = it->parent();
    while(par){
        p = par->text(0).toStdString() + OSInterface::dir_sep + p;
        par = par->parent();
    }
    tmp = p;
    p = content->path;
    if (content->path != OSInterface::getPrefix())
        p = p + OSInterface::dir_sep;
    if(!tmp.empty())
        p = p + tmp + OSInterface::dir_sep + it->text(0).toStdString();
    else
        p = p + it->text(0).toStdString();
}

/*
 *  pri rozbaleni polozky ve stromu
 * pro jeji deti vypocita dalsi patra podstromu
 */

void OpenedListHandle::itemExpanded(QTreeWidgetItem *it){
    std::string p, path;
    getFullPath(it, p);
    for(int r = 0; r < it->childCount(); ++r)
    {
        QTreeWidgetItem *chi = it->child(r);
        if((chi == nullptr) || (chi->childCount())) continue; //spocitat dalsi patro podstromu
        path = p + OSInterface::dir_sep + chi->text(0).toStdString();
        if(OSInterface::isDir(path))
            ((MyTreeView *) content)->buildTree(path, chi, false);
    }
}

/*
 *  aktualizuje informace ve spodnim radku,
 * info cte primo ze seznamu, indexy si proto musi odpovidat
 */

void OpenedListHandle::updateLbl(){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    std::stringstream ss;
    std::string file = content->getSelected();
    if(file.empty()) return;
    ss << getBasename(file);
    lbl->setText(QString::fromStdString(ss.str()));
    ss.str("");
    ss.clear();
    if((view_type == LIST) || ( view_type == ICON)){
        dirEntryT *t = content->osi->dirs[content->getSelIdx()];
        if(t != nullptr)
            ss << round((t->byte_size / pow(1024, data_instance->size_in)));
        addSizeInfo(ss);
        lbl2->setText(QString::fromStdString(ss.str()));
        ss.str("");
        ss.clear();
        ss << t->perms << " ";
        ss << t->mod_time;
    }
    lbl3->setText(QString::fromStdString(ss.str()));
}

/*
 *  vytvori vsechny graficke prvky a vlozi je do layoutu
 * postara se o pripojeni obsluhy tlacitek
 */

void OpenedListHandle::initLayout(std::string p){
    v_layout = new QVBoxLayout();
    h_layout1 = new QHBoxLayout();
    h_layout2 = new QHBoxLayout();
    g_layout = new QGridLayout();
    v_layout2 = new QVBoxLayout();
    tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F2, ButtonHandle<OpenedListHandle>(Qt::Key_F2, "List view", &OpenedListHandle::toList)));
    tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F3, ButtonHandle<OpenedListHandle>(Qt::Key_F3, "Tree view", &OpenedListHandle::toTree)));
    tool_btts.insert(std::pair<Qt::Key, ButtonHandle<OpenedListHandle>>(Qt::Key_F4, ButtonHandle<OpenedListHandle>(Qt::Key_F4, "Icon view", &OpenedListHandle::toIcon)));
    up_btt = new ButtonHandle<OpenedListHandle>(Qt::Key_F10, "UP", &OpenedListHandle::stepUp);
    up_btt->btt->setMaximumWidth(30);
    up_btt->btt->setFocusPolicy(Qt::ClickFocus);
    up_btt->btt->setStyleSheet(btt_style);
    le1 = new MyLineEdit();
    le1->setFocusPolicy(Qt::ClickFocus);
    le1->setStyleSheet(lineedit_style);
    le2 = new QLineEdit();
    le2->setStyleSheet(lineedit_style);
    le2->setFocusPolicy(Qt::ClickFocus);
    le1->setText(QString::fromStdString(p));
    le2->setMaximumWidth(70);
    le2->setText("*");
    view_type = LIST;
    content = new MyTreeView(p, le2->text().toStdString(), false, os);
    tb = new QToolBar();
    lbl = new QLabel();
    lbl2 = new QLabel();
    lbl3 = new QLabel();
    tb = new QToolBar();
    tb2 = new QToolBar();
    tb2->addWidget(lbl);
    tb2->addSeparator();
    tb2->addWidget(lbl2);
    tb2->addSeparator();
    tb2->addWidget(lbl3);
    updateLbl();
    h_layout1->addWidget(le1);
    h_layout1->addWidget(le2);
    h_layout1->addWidget(up_btt->btt);
    QObject::connect(up_btt->btt, &QPushButton::clicked, this, up_btt->fnc);
    QObject::connect(le2, &QLineEdit::textChanged, this, &OpenedListHandle::patternChanged);
    QObject::connect(le1, &QLineEdit::textChanged, this, &OpenedListHandle::pathChanged);
    QObject::connect(le1, &MyLineEdit::focused, this, &OpenedListHandle::setSelection);
    if((view_type == TREE) || (view_type == LIST)){
        connectSignals<MyTreeView>();
        h_layout2->addWidget((MyTreeView *)content);
    }else if(view_type == ICON){
        connectSignals<MyIconView>();
        h_layout2->addWidget((MyIconView *)content);
    }
    for(auto &a : tool_btts){
        a.second.btt->setMaximumWidth(150);
        a.second.btt->setFocusPolicy(Qt::NoFocus);
        a.second.btt->setStyleSheet(btt_style);
        tb->addWidget(a.second.btt);
        QObject::connect(a.second.btt, &QPushButton::clicked, this, a.second.fnc);
    }
    v_layout2->addWidget(tb2);
    highlightBtt();
    v_layout2->addWidget(tb);
    v_layout->addLayout(h_layout1);
    v_layout->addLayout(h_layout2);
    v_layout->addLayout(v_layout2);
}

/* konstruktor */

OpenedListHandle::OpenedListHandle(std::string p,QWidget *parent):QWidget(parent), os(new OSInterface), in_layout(false), path(p), pattern("*"), view_type(LIST){
    last_layout = LIST;
    try{
        os->getDirInfo(path, pattern);
    }catch(OSException *e){
        std::cout << e->what() << std::endl;
    }
    initLayout(p);
}

/*zrusi graficke prvky */

void OpenedListHandle::delGraphics(){
    delete le1;
    delete le2;
    delete up_btt->btt;
    for(auto &a : tool_btts){
        delete a.second.btt;
    }
    delete tb;
    delete lbl;
    delete lbl2;
    delete lbl3;
    delete tb2;
    delete h_layout1;
    delete g_layout;
    delete h_layout2;
    delete v_layout2;
    delete v_layout;
}

OpenedListHandle::~OpenedListHandle(){
    clean();
    delete content;
    delete os;
    delGraphics();
}
