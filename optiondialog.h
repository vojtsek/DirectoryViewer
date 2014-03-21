#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include <map>

class OptionDialog : public QDialog
{
    Q_OBJECT
    QGridLayout *layout;
    QComboBox *combo;
    QLineEdit *le_init_count, *le_col_count, *le_max_lists, *le_dir;
    std::map<QLineEdit *, QLineEdit *> ext_apps;
public:
    OptionDialog();
    ~OptionDialog();
public slots:
    void save();
    void addRow();
};

#endif // OPTIONDIALOG_H
