#include "mainwindow.h"
#include "mainhandler.h"
#include "myiconview.h"
#include "ui_mainwindow.h"
#include "osinterface.h"
#include "optiondialog.h"
#include "openedlisthandle.h"
#include "stylesheets.h"
#include "types.h"

#include <QPushButton>
#include <QShortcut>
#include <QComboBox>
#include <typeinfo>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QListView>
#include <QScrollBar>
#include <QToolBar>
#include <QKeyEvent>
#include <QTreeView>
#include <QHeaderView>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <string>

// konstruktor; inicializace
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    handler(new MainHandler), focus_idx(0)
{
    ui->setupUi(this);
    // pripojeni signalu z menu
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    QObject::connect(ui->actionOption, SIGNAL(triggered()), this, SLOT(opt()));
    QShortcut *shortcut = new QShortcut(QKeySequence("F10"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(exit()));
    prepareLayout();
}

MainWindow::~MainWindow()
{
    for (auto a : handler->opened_lists)
        delete a;
    delete handler;
    delete ui;
}

/* slot,
 * overi, ze chce uzivatel provest akci a zavola prislusnou metodu tridy OSInterface;
 * pouze pro kopii a presun
 */

void MainWindow::confirm1(std::string action, cmd_info_T &info){
    QMessageBox msg_box;
    char buf[10];
    sprintf(buf, "%d", info.source_files.size());
    QString text = QString::fromStdString(action);
    text.append(" selected ");
    text.append(buf);
    text.append(" file(s) \n");
    text.append("From:\n");
    text.append(QString::fromStdString(info.src_path));
    text.append("\nTo:\n");
    for(auto &a : info.paths){
        text.append(QString::fromStdString(a));
        text.append("\n");
    }
    text.append("?");
    msg_box.setWindowTitle("Confirm");
    msg_box.setText(text);
    msg_box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msg_box.setDefaultButton(QMessageBox::Yes);
    if(msg_box.exec() == QMessageBox::Yes){
        if(info.cmd == info.Cmd::COPY)
            OSInterface::copy(info);
        else if(info.cmd == info.Cmd::MOVE)
            OSInterface::move(info);
    }
}

/* slot,
 * overi, ze chce uzivatel provest akci a zavola prislusnou metodu OSInterface;
 * pro mazani a prejmenovani
 */

void MainWindow::confirm2(std::string action, cmd_info_T &info){
    QMessageBox msg_box;
    char buf[10];
    sprintf(buf, "%d", info.source_files.size());
    QString text = QString::fromStdString(action);
    text.append(" selected ");
    text.append(buf);
    text.append(" file(s)\n");
    text.append("From:\n");
    text.append(QString::fromStdString(info.src_path));
    text.append("?");
    msg_box.setWindowTitle("Confirm");
    msg_box.setText(text);
    msg_box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msg_box.setDefaultButton(QMessageBox::Yes);
    if(msg_box.exec() == QMessageBox::Yes){
        if(info.cmd == info.Cmd::REMOVE)
            OSInterface::remove(info);
        else if(info.cmd == info.Cmd::RENAME)
            OSInterface::rename(info);
    }
}

/* oznami chybu */

void MainWindow::error(std::string &err){
    QMessageBox msg_box;
    msg_box.setWindowTitle("Error!");
    msg_box.setText(QString::fromStdString(err));
    msg_box.setStandardButtons(QMessageBox::Ok);
    msg_box.setDefaultButton(QMessageBox::Ok);
    msg_box.exec();
}

/* volano pri vytvoreni okna
 * nastavi  parametry layoutu, prida ovladaci tlacitka do spodniho toolbaru a zaregistruje obsluhu
 * inicializuje dany pocet otevrenych seznamu
 */

void MainWindow::prepareLayout(){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    ui->centralGridLayout->setHorizontalSpacing(20);
    QObject::connect(handler, &MainHandler::ch_list, this, &MainWindow::refreshMainLayout);
    QObject::connect(handler, &MainHandler::confirm1, this, &MainWindow::confirm1);
    QObject::connect(handler, &MainHandler::confirm2, this, &MainWindow::confirm2);
    QObject::connect(handler, &MainHandler::error, this, &MainWindow::error);

    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F2, ButtonHandle<MainHandler>(Qt::Key_F2, "F2 Add List", &MainHandler::list_added)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F3, ButtonHandle<MainHandler>(Qt::Key_F3, "F3 Remove List", &MainHandler::list_removed)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F4, ButtonHandle<MainHandler>(Qt::Key_F4, "F4 Create Folder", &MainHandler::create)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F5, ButtonHandle<MainHandler>(Qt::Key_F5, "F5 Copy", &MainHandler::copy)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F6, ButtonHandle<MainHandler>(Qt::Key_F6, "F6 Move", &MainHandler::move)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F7, ButtonHandle<MainHandler>(Qt::Key_F7, "F7 Rename", &MainHandler::rename)));
    handler->tool_btts.insert(std::pair<Qt::Key, ButtonHandle<MainHandler>>(Qt::Key_F8, ButtonHandle<MainHandler>(Qt::Key_F8, "F8 Remove", &MainHandler::remove)));


    for(auto &a : handler->tool_btts){
        ui->mainToolBar->addWidget(a.second.btt);
        a.second.btt->setFocusPolicy(Qt::ClickFocus);
        a.second.btt->setStyleSheet(btt_style);
        QObject::connect(a.second.btt, &QPushButton::clicked, handler, a.second.fnc);
    }

    for(unsigned int i = 0; i < data_instance->init_count; ++i){
        handler->list_added();
    }

    //emit(handler->ch_list(false));

    handler->opened_lists[0]->content->setFocus();
}

/* slot volany pri zmene focusu
 * odoznaci vsechny otevrene seznamy,
 * ten co byl zameren se postara sam o sebe
 */

void MainWindow::updateFocus(){
    for(auto &a : handler->opened_lists){
        a->content->unFocus();
    }
}

/* slot, po potvrzeni ukonci program */

void MainWindow::exit(){
    if(QMessageBox::question(nullptr, "Exit?", "Do you wanna quit?", QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::No)
        return;
    QApplication::quit();
}

/* slot, vytvori dialog s nastavenim
 * pote aktualizuje layout a seznamy aby se projevily zmeny
 */

void MainWindow::opt(){
    OptionDialog dial;
    dial.exec();
    for(auto a : handler->opened_lists){
        ui->centralGridLayout->removeItem(a->v_layout);
        a->in_layout = false;
    }
    refreshMainLayout(false);
    for(auto a : handler->opened_lists)
        a->content->rebuild();
}

/* pri prestaveni seznamu
 * postara se o spravne poradi predavani focusu
 */

void MainWindow::handleTab(){
    QWidget *first, *second;
    QWidgetItem *myItem;
    first = second = nullptr;
    for(auto &a : handler->opened_lists){
        if(first == nullptr){
            myItem = dynamic_cast <QWidgetItem*>(a->h_layout2->itemAt(0));
            first = myItem->widget();
            continue;
        }
        myItem = dynamic_cast <QWidgetItem*>(a->h_layout2->itemAt(0));
        second = myItem->widget();
        QWidget::setTabOrder(first, second);
        first = second;
    }
}

/* aktualizuje hlavni layout
 * pokud byl ubran seznam, odebere ho z cgl a zlikviduje
 * projde vsechny seznamy, a pokud novy neni zarazen v cgl, tak ho zaradi,
 * zaregistruje obsluhu udalosti a da mu focus
 */

void MainWindow::refreshMainLayout(bool removing){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    int row, col;
    unsigned int count;
    row = col = count = 0;
    if(removing){
        auto &a = *(handler->opened_lists.end()-1);
        ui->centralGridLayout->removeItem(a->v_layout);
        handler->opened_lists.pop_back();
        delete a;
        if(handler->opened_lists.size() > 1){ //odebere predposledni, kdyby byl roztazeny do vice sloupcu
                auto &a = *(handler->opened_lists.end()-1);
                ui->centralGridLayout->removeItem(a->v_layout);
                a->in_layout = false;
        }

        try{
            handler->opened_lists.at(handler->opened_lists.size() - 1)->content->setFocus();
        }catch(std::exception e) {}
    } else if(handler->opened_lists.size() > 1){ //odebere predposledni, kdyby byl roztazeny do vice sloupcu
        auto &a = *(handler->opened_lists.end()-2);
        ui->centralGridLayout->removeItem(a->v_layout);
        a->in_layout = false;
    }
    for(auto a : handler->opened_lists){
        ++count;
        if(!a->in_layout){
            a->in_layout = true;
            if((count == handler->opened_lists.size()) && row) //pridava posledni - muze se roztahnout do vice sloupcu
                ui->centralGridLayout->addLayout(a->v_layout, row, col, 1, data_instance->col_count - col);
            else
                ui->centralGridLayout->addLayout(a->v_layout, row, col, 1, 1);
            if(handler->opened_lists.size() < data_instance->col_count)
                a->tb2->setMaximumWidth(ui->centralWidget->width() / handler->opened_lists.size());
            else
                a->tb2->setMaximumWidth(ui->centralWidget->width() / data_instance->col_count);

            a->content->setFocus();
            QObject::connect(a, &OpenedListHandle::updated, this, &MainWindow::handleTab);
            if((a->view_type == a->TREE) || (a->view_type == a->LIST))
                QObject::connect((MyTreeView *)a->content, &MyTreeView::focused, this, &MainWindow::updateFocus);
            else if(a->view_type == a->ICON)
                QObject::connect((MyIconView *)a->content, &MyIconView::tab, this, &MainWindow::handleTab);
        }
        if(!(++col % data_instance->col_count)){
            col = 0;
            ++row;
        }

    }
}

/* obsluha stisku klaves,
 * pomoci mapovani kodu klaves na struktury popisujici tlacitka
 */

void MainWindow::keyPressEvent(QKeyEvent *k){
    try{
        ButtonHandle<MainHandler> &tbtt = handler->tool_btts.at((Qt::Key)k->key());
        emit(tbtt.btt->clicked());
    }catch(std::out_of_range &e){
        QMainWindow::keyPressEvent(k);
    }
}

