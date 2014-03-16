#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QtWidgets>

class OptionDialog : public QDialog
{
    Q_OBJECT
    QComboBox *combo;
    QLineEdit *le_init_count, *le_col_count, *le_max_lists, *le_dir;
public:
    OptionDialog();
    ~OptionDialog();
public slots:
    void save();
};

#endif // OPTIONDIALOG_H
