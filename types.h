
#include "mainhandler.h"
#include "mytreeview.h"
#include "osinterface.h"

#include <dirent.h>
#include <QWidget>
#include <memory>
#include <QString>
#include <QPushButton>
#include <QStandardItem>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QGridLayout>
#include <QLineEdit>
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QObject>
#include <map>
#include <set>
#include <string>
#include <thread>

#ifndef TYPES_H
#define TYPES_H


class MainHandler;
class OSInterface;

struct Data{
    unsigned int size_in, max_lists, col_count, init_count;
    static const long socd_record = 0x06054b50, bufsize = 4096;
    std::string init_dir, home_path;
    std::map<std::string, std::string> extern_programmes;
    enum {B = 0, KB = 1, MB = 2, GB = 3};
    std::vector<std::thread> thrs;
    static std::shared_ptr<Data> getInstance();

private:
    static std::shared_ptr<Data> inst;
};

template <class T>
struct ButtonHandle{
    QAbstractButton *btt;
    Qt::Key keycode;
    std::string label;
    void (T::*fnc)(void);
    ButtonHandle(Qt::Key k, std::string l, void (T::*f)(void)): keycode (k), label(l){ btt = new QPushButton(QString::fromStdString(l)); fnc = f;}
};

struct dirEntryT{
    std::string name, type_name, ext_name, mod_time, perms;
    enum class Type{FILE, DIR, UNKNOWN, LINK, ARCHIVE, EXE};
    Type type;
    long long byte_size;
    long long show_size;
    dirEntryT():type(Type::FILE), byte_size(0), show_size(0) {}
};


typedef struct{
    std::string src_path;
    std::set<std::string> source_files, paths;
    std::map<std::string,std::set<std::string>> destination_files;
    enum class Cmd{COPY, MOVE, VIEW, RENAME, REMOVE };
    Cmd cmd;
} cmd_info_T;

class MyViewType{
protected:
    QIcon dir_icon,ar_icon, base_icon;
    QFont base_font, bold_font, italic_font;
public:
    std::set<std::string> multi_selection;
    bool is_focused, marked, recursive;
    std::string path, pattern;
    OSInterface *osi;
    int w, col_width, index;
    virtual std::string getSelected() = 0;
    virtual void rebuild(int idx = 0) = 0;
    virtual int getSelIdx() = 0;
    virtual int getIdxOf(std::string &) = 0;
    virtual void setFocus() = 0;
    virtual void unFocus() = 0;
    virtual void focus() = 0;
    virtual void mark(bool) = 0;
    virtual void die() = 0;
    MyViewType(std::string p, std::string pat, OSInterface *os) : is_focused(false), marked(false), path(p), pattern(pat), osi(os){
        std::shared_ptr<Data> data_instance = Data::getInstance();
        dir_icon =  QIcon(QString::fromStdString(data_instance->home_path + "/icons/folder-open-blue.png"));
        ar_icon = QIcon(QString::fromStdString(data_instance->home_path + "/icons/database.png"));
        base_icon = QIcon(QString::fromStdString(data_instance->home_path + "/icons/doc-plain-blue.png"));
        italic_font.setFamily("Verdana");
        italic_font.setItalic(true);
        bold_font.setBold(true);
        bold_font.setFamily("Verdana");
        base_font.setFamily("Verdana");
    }

    virtual ~MyViewType() {}
};

#endif //TYPES_H
