#include "optiondialog.h"
#include <QtWidgets>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>

extern int size_in, max_lists, col_count, init_count;
extern std::string init_dir;

OptionDialog::OptionDialog()
{
    std::stringstream ss;
    QGridLayout *layout = new QGridLayout();
    combo = new QComboBox();
    ss << col_count;
    le_col_count = new QLineEdit(QString::fromStdString(ss.str()));
    le_dir = new QLineEdit(QString::fromStdString(init_dir));
    le_dir->setFixedWidth(220);
    ss.str("");
    ss.clear();
    ss << init_count;
    le_init_count = new QLineEdit(QString::fromStdString(ss.str()));
    ss.str("");
    ss.clear();
    ss << max_lists;
    le_max_lists = new QLineEdit(QString::fromStdString(ss.str()));
    QPushButton *save_btt = new QPushButton("Save");
    QObject::connect(save_btt, SIGNAL(clicked()), this, SLOT(save()));
    combo->addItem("B");
    combo->addItem("KB");
    combo->addItem("MB");
    combo->addItem("GB");
    combo->setCurrentIndex(size_in);

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

    layout->addWidget(save_btt, 5, 2);
    setLayout(layout);
}

void OptionDialog::save(){
    size_in = combo->currentIndex();
    col_count = atoi(le_col_count->text().toStdString().c_str());
    max_lists = atoi(le_max_lists->text().toStdString().c_str());
    init_count = atoi(le_init_count->text().toStdString().c_str());
    init_dir = le_dir->text().toStdString();
    std::ofstream out("dv.conf");
    out << "column_count " << col_count << std::endl;
    out << "size_in " << size_in << std::endl;
    out << "max_list_count " << max_lists << std::endl;
    out << "init_list_count " << init_count << std::endl;
    out << "start_directory " << init_dir << std::endl;
    out.close();
    close();
}

OptionDialog::~OptionDialog(){
    delete combo;
    delete le_init_count;
    delete le_col_count;
    delete le_max_lists;
    delete le_dir;
}
