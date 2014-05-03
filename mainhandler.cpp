#include "mainhandler.h"
#include "osinterface.h"
#include "mydialog.h"
#include "functions.h"
#include "types.h"
#include <iostream>
#include <QStandardItemModel>
#include <QInputDialog>
#include <vector>
#include <set>
#include <map>
#include <QTreeView>
#include <string>
#include <sstream>
#include <fstream>

/* konstruktor;
 * inicializuje globalni promenne na default, pote precte obsah konfiguracniho souboru
 * a pripadne zmeni obsah promennych.
 */

MainHandler::MainHandler(QObject *parent) :
    QObject(parent)
{
    std::shared_ptr<Data> data_instance = Data::getInstance();
    data_instance->init_dir = OSInterface::getCWD();
    data_instance->home_path = OSInterface::getCWD();
    loadSettings(data_instance);
    loadApps(data_instance);
}

/*
 * nacte jmena aplikaci pro otevreni danych koncovek
 */

void MainHandler::loadApps(std::shared_ptr<Data> data_instance){
    try{
        std::string ext, app, line, option;
        std::ifstream in("extern.conf");
        while(in.good()){
            getline(in, line);
            std::stringstream ss(line);
            if(line.empty()) break;
            ss >> option;
            if(!option.empty()){
                ss >> app;
                if(!app.empty()){
                    unsigned int pos;
                    while((pos = option.find(',')) != std::string::npos){
                        ext = option.substr(0, pos);
                        option = option.substr(pos + 1, option.size());
                        data_instance->extern_programmes.emplace(ext, app);
                    }
                    data_instance->extern_programmes.emplace(option, app);
                }
            }
        }
    }catch(std::exception e) { std::cout << e.what() << std::endl; }
}

/*
 * nacte obsah konfiguracniho souboru
 */

void MainHandler::loadSettings(std::shared_ptr<Data> data_instance){
    std::string line, option, d;
    int param;

    try{
        std::ifstream in("dv.conf");
        while(1){
            getline(in, line);
            std::stringstream ss(line);
            if(line.empty()) break;
            ss >> option;
            if((option != "start_directory") && (option != "home_path")){
                ss >> param;
                if(option == "column_count") data_instance->col_count = param;
                else if(option == "max_list_count") data_instance->max_lists = param;
                else if(option == "init_list_count") data_instance->init_count = param;
                else if(option == "size_in") data_instance->size_in = param;
            } else{
                ss >> d;
                if(option == "start_directory"){
                    if(d == "cwd")
                        data_instance->init_dir = OSInterface::getCWD();
                    else
                        data_instance->init_dir = d;
                }else if(option == "home_path")
                    data_instance->home_path = d;
            }
        }
    }catch(std::exception e) { std::cout << e.what() << std::endl; }

}

/* vytvori strukturu cmd_info_T;
 * ve vystupnich parametrech is_src a is_dst preda informaci, jestli existuje zdrojova a cilova cast prikazu
 * add_dst urcuje, jestli se jedna o typ prikazu, kde je treba pridat cilovou cast
 * v source se preda ukazatel na zpracovavany list - kvuli reloadu.
 */

void MainHandler::prepare_cmd(cmd_info_T &cmd_info, bool &is_src, bool &is_dst, bool add_dst, OpenedListHandle * &source){
    is_dst = is_src = false;
    std::string file;
    cmd_info.paths.clear();
    if(add_dst){
        for(auto &a : opened_lists){
            if(((a->content->marked) || opened_lists.size() == 2) && (!a->content->is_focused)){ // je oznacen jako cil nebo je jediny zbyvajici
                is_dst = true;
                cmd_info.paths.insert(a->content->path);
            }
        }
    }
    for(auto &a : opened_lists){
        if(a->content->is_focused){
            cmd_info.src_path = a->content->path;
            for(auto src :  a->content->multi_selection){
                is_src = true;
                source = a;
                cmd_info.source_files.insert(src);
                for(auto &b : cmd_info.paths){
                    cmd_info.destination_files[src].insert(b + OSInterface::dir_sep + getBasename(src));
                }
            }
        }
    }
}

void MainHandler::refresh(OpenedListHandle *o){
    refreshLists(o, false);
}

/*
 * aktualizuje dany(e) seznam(y)
 */

void MainHandler::refreshLists(OpenedListHandle *src, bool all){
    std::cout << "Refreshing" << std::endl;
    src->os->dirs.clear();
    src->os->getDirInfo(src->path, "*");
    src->content->multi_selection.clear();
    src->content->rebuild();
    if(all){
        for(auto &a : opened_lists){
            if((a->content->marked) || opened_lists.size() == 2){
                a->os->dirs.clear();
                a->os->getDirInfo(a->path, "*");
                a->content->rebuild();
            }
        }
    }
}

/* slot reagujici na vyvolani kopirovani
 * pripravi strukturu cmd_info_T, zkontroluje korektnost operace
 * pokud vse odpovida, vyvola signal, ktery je zachycen v hlavnim okne a provede reload,
 * jinak chybove hlasky
 */

void MainHandler::copy(){
    cmd_info_T cmd_info;
    cmd_info.cmd = cmd_info.Cmd::COPY;
    bool is_dst, is_src;
    OpenedListHandle *src = nullptr;
    prepare_cmd(cmd_info, is_src, is_dst, true, src);
    for(auto &a : cmd_info.paths){
        if(a == cmd_info.src_path){
            std::string err("Destination is the same.");
            emit(error(err));
            return;
        }
    }
    if(is_dst && is_src){
        emit(confirm1("Copy",cmd_info));
        refreshLists(src, true);
    }else{
        if(!is_dst){
            std::string err("No selected destinations.");
            emit(error(err));
        }
        if(!is_src){
            std::string err("No selected source files.");
            emit(error(err));
            if(src){
                src->content->multi_selection.clear();
                src->content->rebuild();
            }
        }
    }
}

/* slot reagujici na vyvolani mazani
 * pripravi strukturu cmd_info_T, zkontroluje korektnost operace
 * pokud vse odpovida, vyvola signal, ktery je zachycen v hlavnim okne a provede reload,
 * jinak chybove hlasky
 */

void MainHandler::remove() {
    cmd_info_T cmd_info;
    cmd_info.cmd = cmd_info.Cmd::REMOVE;
    bool is_dst, is_src;
    OpenedListHandle *src;
    prepare_cmd(cmd_info, is_src, is_dst, false, src);
    if(is_src){
        emit(confirm2("Remove",cmd_info));
        refreshLists(src, false);
    }else{
        std::string err("No source file selected.");
        emit(error(err));
    }
}

/* slot reagujici na vyvolani presunuti
 * pripravi strukturu cmd_info_T, zkontroluje korektnost operace
 * pokud vse odpovida, vyvola signal, ktery je zachycen v hlavnim okne a provede reload,
 * jinak chybove hlasky
 */

void MainHandler::move() {
    cmd_info_T cmd_info;
    cmd_info.cmd = cmd_info.Cmd::MOVE;
    bool is_dst, is_src;
    OpenedListHandle *src;
    prepare_cmd(cmd_info, is_src, is_dst, true, src);
    for(auto &a : cmd_info.paths){
        if(a == cmd_info.src_path){
            std::string err("Destination is the same.");
            emit(error(err));
            return;
        }
    }
    if(is_dst && is_src){
        emit(confirm1("Move",cmd_info));
        refreshLists(src, true);
        if(!is_dst){
            std::string err("No selected destinations.");
            emit(error(err));
        }
        if(!is_src){
            std::string err("No selected source files.");
            emit(error(err));
        }
    }
}

/* slot reagujici na vyvolani prejmenovani
 * pripravi strukturu cmd_info_T, zkontroluje korektnost operace a ziska jmena novych souboru
 * pokud vse odpovida, zavola rename, ktery uz provede akci, potom reload,
 * jinak chybove hlasky
 */

void MainHandler::rename() {
    cmd_info_T cmd_info;
    bool is_dst, is_src;
    OpenedListHandle *src;
    prepare_cmd(cmd_info, is_src, is_dst, false, src);
    if(is_src){
        for(auto &a : cmd_info.source_files){
            std::string file = a;
            QString lbl("Enter new name:");
            QString txt("new name for \n");
            txt.append(QString::fromStdString(getBasename(file)));
            std::string str = cmd_info.src_path;
            str.push_back(OSInterface::dir_sep);
            std::string name = QInputDialog::getText(nullptr, lbl, txt).toStdString();
            str.append(name);
            if(name.empty()){
                std::string err("Empty names not allowed.");
                emit(error(err));
                break;
            }
            cmd_info.destination_files[a].insert(str);
        }
        OSInterface::rename(cmd_info);
        refreshLists(src, false);
    }else{
        std::string err("No selected source files.");
        emit(error(err));
    }
}

/* slot reagujici na vyvolani vytvoreni adresare
 * zkontroluje korektnost operace a ziska jmeno noveho adresare
 * pokud vse odpovida, zavola create, ktery uz provede akci, potom reload,
 * jinak chybove hlasky
 */

void MainHandler::create() {
    char inval = 0;
    for(auto &a : opened_lists){
        if(a->content->is_focused){
            QString lbl("Enter new name:");
            QString txt("Name of new folder:\n");
            std::string name = QInputDialog::getText(nullptr, lbl, txt).toStdString();
            if(name.empty()){
                std::string err("Empty names not allowed.");
                emit(error(err));
                break;
            }
            if((inval = isValidFn(name)) != 0){
                std::string err("Invalid character '");
                err.push_back(inval);
                err.push_back('\'');
                emit(error(err));
                break;
            }
            repairPath(a->content->path);
            try{
                OSInterface::create(a->content->path + name);
            }catch(OSException *e) {e->process();}
            a->os->dirs.clear();
            a->os->getDirInfo(a->path, "*");
            a->content->rebuild();
        }
    }
}

/* slot reagujici na pridani dalsiho seznamu - vytvori ho a informuje mainwindow */

void MainHandler::list_added(){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    if(opened_lists.size() < data_instance->max_lists){
        OpenedListHandle *no = new OpenedListHandle(data_instance->init_dir);
        opened_lists.push_back(no);
        QObject::connect(no, &OpenedListHandle::refreshed, this, &MainHandler::refresh);
        emit(ch_list(false));
    }
}

/* slot reaguji na odstraneni seznamu - informuje mainwindow */

void MainHandler::list_removed(){ //nic nemaze, protoze je ho treba nejdriv odstranit z layoutu
    if(opened_lists.size())
        emit(ch_list(true));
}

