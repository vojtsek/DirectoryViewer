#include "optiondialog.h"
#include "types.h"
#include <QtWidgets>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <map>

/* konstruktor
 * vytvori dialog umoznujici nastaveni
 */

OptionDialog::OptionDialog(){
    std::shared_ptr<Data> data_instance = Data::getInstance();

    std::stringstream ss;
    layout = new QGridLayout();
    combo = new QComboBox();
    ss << data_instance->col_count;
    le_col_count = new QLineEdit(QString::fromStdString(ss.str()));
    le_dir = new QLineEdit(QString::fromStdString(data_instance->init_dir));
    le_dir->setFixedWidth(220);
    ss.str("");
    ss.clear();
    ss << data_instance->init_count;
    le_init_count = new QLineEdit(QString::fromStdString(ss.str()));
    ss.str("");
    ss.clear();
    ss << data_instance->max_lists;
    le_max_lists = new QLineEdit(QString::fromStdString(ss.str()));
    combo->addItem("B");
    combo->addItem("KB");
    combo->addItem("MB");
    combo->addItem("GB");
    combo->setCurrentIndex(data_instance->size_in);

    layout->addWidget(new QLabel("View size in:"), 0, 0);
    layout->addWidget(combo, 0, 1);
    layout->addWidget(new QLabel("Init list count:"), 1, 0);
    layout->addWidget(le_init_count, 1, 1);
    layout->addWidget(new QLabel("Column count:"), 2, 0);
    layout->addWidget(le_col_count, 2, 1);
    layout->addWidget(new QLabel("Max list count:"), 3, 0);
    layout->addWidget(le_max_lists, 3, 1);
    layout->addWidget(new QLabel("Start directory:"), 4, 0);
    layout->addWidget(le_dir, 4, 1);

    QLineEdit *e, *app;
    int row = 5;
    for(auto a : data_instance->extern_programmes){
        e = new QLineEdit(QString::fromStdString(a.first));
        app = new QLineEdit(QString::fromStdString(a.second));
        ext_apps.emplace(e, app);
        layout->addWidget(e, row, 0);
        layout->addWidget(app, row, 1);
        ++row;
    }

    QPushButton *save_btt = new QPushButton("Save");
    QPushButton *add_btt = new QPushButton("Add");
    QObject::connect(add_btt, SIGNAL(clicked()), this, SLOT(addRow()));
    QObject::connect(save_btt, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget(add_btt, row++, 0);
    layout->addWidget(save_btt, row, 2);
    setLayout(layout);
}

/*
 *
 */

void OptionDialog::addRow(){
    QLineEdit *e = new QLineEdit();
    QLineEdit *app = new QLineEdit();
    int count = 5;
    QLayoutItem *it = layout->itemAtPosition(ext_apps.size() + count, 0); //ty plus jsou kvuli predchozim polozkam.
    layout->removeItem(it);
    delete it;
    it = layout->itemAtPosition(ext_apps.size() + 1 + count, 2);
    layout->removeItem(it);
    delete it->widget();
    ext_apps.emplace(e, app);
    layout->addWidget(e, ext_apps.size() - 1 + count, 0);
    layout->addWidget(app, ext_apps.size() -1 + count, 1);
    QPushButton *save_btt = new QPushButton("Save");
    QPushButton *add_btt = new QPushButton("Add");
    QObject::connect(add_btt, SIGNAL(clicked()), this, SLOT(addRow()));
    QObject::connect(save_btt, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget(add_btt, ext_apps.size() + count, 0);
    layout->addWidget(save_btt, ext_apps.size() + 1 + count, 2);
}

/* slot volany pri ulozeni
 * zpracuje udaje a ulozi do souboru
 */

void OptionDialog::save(){
    std::shared_ptr<Data> data_instance = Data::getInstance();

    data_instance->size_in = combo->currentIndex();
    data_instance->col_count = atoi(le_col_count->text().toStdString().c_str());
    data_instance->max_lists = atoi(le_max_lists->text().toStdString().c_str());
    data_instance->init_count = atoi(le_init_count->text().toStdString().c_str());
    data_instance->init_dir = le_dir->text().toStdString();
    std::ofstream out("dv.conf");
    out << "column_count " << data_instance->col_count << std::endl;
    out << "size_in " << data_instance->size_in << std::endl;
    out << "max_list_count " << data_instance->max_lists << std::endl;
    out << "init_list_count " << data_instance->init_count << std::endl;
    out << "start_directory " << data_instance->init_dir << std::endl;
    out.close();
    out.open("extern.conf");
    for(auto a : ext_apps){
        if(!a.first->text().toStdString().empty() && ! a.second->text().toStdString().empty())
            out << a.first->text().toStdString() << " " << a.second->text().toStdString() << std::endl;
    }
    out.close();
    close();
}

OptionDialog::~OptionDialog(){
    delete combo;
    delete le_init_count;
    delete le_col_count;
    delete le_max_lists;
    delete le_dir;
    for(auto a : ext_apps){
        delete a.first;
        delete a.second;
    }
}
