#include "osinterface.h"
#include "functions.h"
#include "mydialog.h"
#include "types.h"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <algorithm>
#include <QTimer>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <QProcess>

#ifdef __unix__

#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <cstring>
#include <sys/stat.h>

#endif

OSInterface::OSInterface()
{
}

/* zkontroluje, jestli ma smysl otevirat soubor jako text */

bool OSInterface::isOpenable(std::string path){
    if(isArch(path))
        return false;
    if (getSize(path) > pow(1024, 2)){
        std::string warn = "File is quite large. Proceed with opening?";
        if(QMessageBox::question(nullptr, "Open file", QString::fromStdString(warn), QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::No)
            return false;
    }
    return true;
}

/* zpracuje strukturu cmd_info_T, provede kopii souboru
 * vola prislusnou funkci
 */

void OSInterface::copy(cmd_info_T &ci){
    QMessageBox *msg_box;
    std::shared_ptr<Data> data_instance = Data::getInstance();
    for(auto src : ci.source_files){
        for(auto dstf : ci.destination_files[src]){
            msg_box = new QMessageBox();
            msg_box->setWindowTitle("copying file");
            msg_box->setText(QString::fromStdString("copying\n" + src));
            msg_box->setStandardButtons(QMessageBox::NoButton);

            data_instance->thrs.emplace_back([&]() {try {std::this_thread::yield();  doCopy(src, dstf); } catch(OSException *e){} delete msg_box;});
            msg_box->exec();
            data_instance->thrs[0].join();
            data_instance->thrs.clear();
        }
    }
}
void OSInterface::showInfo(std::string info){
    MyDialog::MsgBox(info);
}

/* zpracuje strukturu cmd_info_T, provede presunuti souboru
 * vola prislusnou funkci zavisejici na OS
 */

void OSInterface::move(cmd_info_T &ci){
    QMessageBox  *msg_box;
    std::shared_ptr<Data> data_instance = Data::getInstance();
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    for(auto src : ci.source_files){
        for(auto dstf : ci.destination_files[src]){
            msg_box = new QMessageBox();
            msg_box->setWindowTitle("moving file");
            msg_box->setText(QString::fromStdString("moving\n" + src));
            msg_box->setStandardButtons(QMessageBox::NoButton);

            data_instance->thrs.emplace_back([&]() {
                try { doMove(src, dstf);
                    while(!msg_box->isActiveWindow()) {std::this_thread::sleep_for (std::chrono::milliseconds(100)); }
                }catch(OSException *e){} delete msg_box;});
            msg_box->exec();
            data_instance->thrs[0].join();
            data_instance->thrs.clear();
        }
    }
}

/* zpracuje strukturu cmd_info_T, provede prejmenovani souboru
 * vola prislusnou funkci zavisejici na OS
 */

void OSInterface::rename(cmd_info_T &ci){
    for(auto src : ci.source_files){
        for(auto dstf : ci.destination_files[src]){
            try{
                doMove(src, dstf);
                std::stringstream ss;
                ss << "Rename: " << src << " -> " << dstf << std::endl;
                ss << "Success!";
                std::string str = ss.str();
                // MyDialog::MsgBox(str);
            }catch(OSException *e){
                e->process();
            }
        }
    }
}

/* zpracuje strukturu cmd_info_T, provede smazani souboru
 * vola prislusnou funkci zavisejici na OS
 */

void OSInterface::remove(cmd_info_T &ci){

    for(auto src : ci.source_files){
        try{
            if(isDir(src)){
                std::string warn = src + "\n is a directory. Remove anyvway? \n";
                if(QMessageBox::question(nullptr, "Remove directory", QString::fromStdString(warn), QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::No)
                    throw std::exception();
            }
            doRemove(src);
            std::stringstream ss;
            ss << "Remove " << src << std::endl;
            ss << "Success!";
            std::string str = ss.str();
            //  MyDialog::MsgBox(str);
        }catch(OSException *e){
            e->process();
        }
        catch(std::exception e) {} //nepotvrzeni smazani adresare
    }

}

/* vlastni provedeni kopie souboru
 * je implementovano standartnimi prostredky c++
 * a je platformove nezavisle
 * lisi se jen pripadne vytvoreni adresare
 */

void OSInterface::doCopy(std::string &src, std::string &dst){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    if(isDir(src)){ //je to adresar
        OSInterface os;
        os.getDirInfo(src, "*");
#ifdef __unix__
        if(mkdir(dst.c_str(), 0755) == -1)
#endif
#ifdef __WIN32__
            if(mkdir(dst.c_str()) == -1)
#endif
                throw new OSException(src, strerror(errno));
        for(auto &a : os.dirs){ // rekurzivni kopie podadresaru
            std::string nsrc, ndst;
            nsrc = src;
            nsrc.push_back(dir_sep);
            nsrc.append(a->name);
            ndst = dst;
            ndst.push_back(dir_sep);
            ndst.append(a->name);
            doCopy(nsrc, ndst);
        }
    }else{ // kopie souboru pomoci buferu
        std::ifstream file(src, std::ios::in | std::ios::binary | std::ios::ate);
        std::ifstream test(dst, std::ios::in | std::ios::binary | std::ios::ate);
        if(test.tellg() > 0)
            throw new OSException(dst, "File exists");
        test.close();
        std::ofstream nfile(dst, std::ios::out | std::ios::binary | std::ios::ate);
        char buf[data_instance->bufsize];
        long long copied = 0;
        if(!nfile.is_open())
            throw new OSException(dst, "Failed to create.");
        if (file.is_open())
        {
            long long size = file.tellg();
            file.seekg (0, std::ios::beg);
            while(file.good() && !file.eof()){
                file.read(buf, data_instance->bufsize);
                nfile.write(buf, file.gcount());
                copied += file.gcount();
                if(nfile.fail())
                    throw new OSException(dst, "Failed to write.");
            }
            if(copied != size)
                throw new OSException(src, "Failed to copy.");
        }else
            throw new OSException(src, "Failed to open.");
        nfile.flush();
        file.close();
        nfile.close();
    }
}

/* rekurzivne zjisti soucet velikosti souboru v adresarovem podstromu */

size_t OSInterface::computeDirSize(std::string path){
    if(!isDir(path))
        //return 0;
        throw new OSException(path, "Failed to open dir.");
    size_t size(0);
    OSInterface os;
    os.getDirInfo(path, "*");
    for(auto &a : os.dirs){
        if(isDir(path + dir_sep + a->name))
            size += computeDirSize(path + dir_sep + a->name);
        else
            size += getSize(path + dir_sep + a->name);
    }
    return size;
}

/* spusti externi aplikaci
 * jako parametr preda jmeno souboru
 * provadi se, pokud je typ souboru znamy
 */

void OSInterface::openFile(std::string &path){
    std::shared_ptr<Data> data_instance = Data::getInstance();
    std::string ext(getExtension(path));
    std::string app(data_instance->extern_programmes.at(ext));
    QString program(app.c_str());
    QStringList arguments;
    arguments << QString::fromStdString(path);

    QProcess *myProcess = new QProcess();
    myProcess->start(program, arguments);
}

#ifdef __unix__

/* absolutni cesta k aktualnimu adresari */

std::string OSInterface::getCWD(){
    char buf[255];
    getcwd(buf, 255);
    return std::string(buf);
}

/* vytvoreni slozky */

void OSInterface::create(std::string path){
    if(mkdir(path.c_str(), 0755) == -1)
        throw new OSException(path, strerror(errno));
}


/* provede presunuti souboru
 * v pripade, ze jde o adresar, vytvori novy a presune podstrom, potom smaze stary
 */

void OSInterface::doMove(std::string &src, std::string &dst){
    std::stringstream ss;
    if(isDir(src)){
        OSInterface os;
        os.getDirInfo(src, "*");
        if(mkdir(dst.c_str(), 0755) == -1)
            throw new OSException(src, strerror(errno));
        for(auto &a : os.dirs){
            std::string nsrc, ndst;
            nsrc = src;
            nsrc.push_back(dir_sep);
            nsrc.append(a->name);
            ndst = dst;
            ndst.push_back(dir_sep);
            ndst.append(a->name);
            doMove(nsrc, ndst);
        }
        if(rmdir(src.c_str()) == -1)
            throw new OSException(src, strerror(errno));
    }else{

        ss << "Move " << src << " to: " << dst << std::endl;
        if(((link(src.c_str(), dst.c_str())) == -1) || (unlink(src.c_str()) == -1))
            throw new OSException(ss.str(), strerror(errno));
    }
}

/* provede smazani souboru,
 * pokud je to adresar, nejprve smaze podstrom */

void OSInterface::doRemove(std::string &src){
    std::stringstream ss;
    if(isDir(src)){
        OSInterface os;
        os.getDirInfo(src, "*");
        for(auto &a : os.dirs){
            std::string nsrc;
            nsrc = src;
            nsrc.push_back(dir_sep);
            nsrc.append(a->name);
            doRemove(nsrc);
        }
        if(rmdir(src.c_str()) == -1)
            throw new OSException(src, strerror(errno));
    }else{
        ss << "Remove " << src << std::endl;
        if((unlink(src.c_str())) == -1)
            throw new OSException(ss.str(), strerror(errno));
    }
}

/* cesta do korene */

std::string OSInterface::getPrefix(){ return "/"; }

/* vrati velikost souboru v bytech */

std::size_t OSInterface::getSize(std::string p){
    struct stat *finfo = new struct stat();
    lstat(p.c_str(), finfo);
    return finfo->st_size;
}

/* urci, je-li soubor predany parametrem adresar */

bool OSInterface::isDir(std::string path){
    DIR *dir;
    if((dir = opendir(path.c_str())) == NULL){
        return false;
    }
    closedir(dir);
    return true;
}

/* ziska informace o souborech v adresari
 * projde adresar a ziska info o jednotlivych souborech,
 * to pak ulozi do vectoru dirs
 * nakonec vektor seradi, adresare na zacatek, protoze obsah musi korespondovat se zobrazenym seznamem
 */

void OSInterface::getDirInfo(std::string path, std::string pattern){
    DIR *dir;
    if((dir = opendir(path.c_str())) == NULL){
        throw new OSException(path, "Failed to open dir.");
        //return;
    }
    struct dirent *entry = new struct dirent();
    struct stat *finfo = new struct stat();
    std::string abs_path(path), name;
    dirEntryT *de;
    while((entry = readdir(dir))){
        de = new dirEntryT();
        name = entry->d_name;
        de->ext_name = getExtension(name);
        if((name == ".") || name == "..") continue;
        if((!matchExpression(name, pattern)) && !isDir(abs_path + dir_sep + name)) continue;
        lstat((abs_path + dir_sep + name).c_str(), finfo);
        if(finfo == NULL)
            throw new OSException(name, std::string("failed to read file info."));
        de->name = name;
        if(S_ISREG(finfo->st_mode)){
            de->type = de->Type::FILE;
            de->type_name = "FILE";
        }else if(S_ISDIR(finfo->st_mode)){
            de->type = de->Type::DIR;
            de->type_name = "DIR";
        }else if(S_ISLNK(finfo->st_mode)){
            de->type = de->Type::LINK;
            de->type_name = "LINK";
        }else{
            de->type = de->Type::UNKNOWN;
            de->type_name = "UNKNOWN";
        }
        if(isArch(de->name)){
            de->type_name = "ARCHIVE";
            de->type = de->Type::ARCHIVE;
        }
        de->byte_size = finfo->st_size;
        char buf[255];
        sprintf(buf, "%s", ctime(&finfo->st_mtime));
        std::stringstream ss;
        mode_t mode = finfo->st_mode;
        std::string date(buf);
        date = date.substr(0, date.size() - 1);
        de->mod_time = date;
        if((mode & S_IRWXU) & S_IRUSR) ss << "r"; else ss << "-";
        if((mode & S_IRWXU) & S_IWUSR) ss << "w"; else ss << "-";
        if((mode & S_IRWXU) & S_IXUSR) ss << "x"; else ss << "-";
        ss << " ";
        if((mode & S_IRWXG) & S_IRGRP) ss << "r"; else ss << "-";
        if((mode & S_IRWXG) & S_IWGRP) ss << "w"; else ss << "-";
        if((mode & S_IRWXG) & S_IXGRP) ss << "x"; else ss << "-";
        ss << " ";
        if((mode & S_IRWXO) & S_IROTH) ss << "r"; else ss << "-";
        if((mode & S_IRWXO) & S_IWOTH) ss << "w"; else ss << "-";
        if((mode & S_IRWXO) & S_IXOTH) ss << "x"; else ss << "-";
        de->perms = std::string(ss.str());
        dirs.push_back(de);
    }
    closedir(dir);
    std::sort(dirs.begin(), dirs.end(),[=](dirEntryT *d1, dirEntryT *d2){
        if ((d1->type == d1->Type::DIR) && (d2->type != d2->Type::DIR))
            return true;
        else if ((d1->type != d1->Type::DIR) && (d2->type == d2->Type::DIR))
            return false;
        else
            return d1->name < d2->name; });
}

#endif // __unix__

#ifdef __WIN32__

#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <windows.h>
#include <time.h>
#include <direct.h>
#include <cstring>
#include <stdlib.h>

/* vrati cestu do aktualniho adresare */

std::string OSInterface::getCWD(){
    char buf[255];
    _getcwd(buf, 255);
    return std::string(buf);
}

/* vytvoreni adresare */

void OSInterface::create(std::string path){
    std::wstring p(path.begin(), path.end());
    if(!CreateDirectory(p.c_str(), NULL))
        throw new OSException(path, strerror(errno));
}

/* provede presunuti souboru */

void OSInterface::doMove(std::string &src, std::string &dst){
    std::wstring oldf, newf;
    std::stringstream ss;
    oldf.assign(src.begin(), src.end());
    newf.assign(dst.begin(), dst.end());
    ss << "Move " << src << " to: " << dst << std::endl;
    if(!MoveFile(oldf.c_str(), newf.c_str()))
        throw new OSException(ss.str(), strerror(errno));
}

/* provede smazani souboru,
 * pokud je to adresar, nejprve smaze podstrom */

void OSInterface::doRemove(std::string &src){
    std::wstring oldf;
    std::stringstream ss;
    oldf.assign(src.begin(), src.end());
    ss << "Remove " << src << std::endl;
    if(isDir(src)){
        std::string warn = src + "\n is a directory. Remove anyvway? \n";
        if(QMessageBox::question(nullptr, "Remove directory", QString::fromStdString(warn), QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::No)
            throw std::exception();
        OSInterface os;
        os.getDirInfo(src, "*");
        for(auto &a : os.dirs){
            std::string nsrc;
            nsrc = src;
            nsrc.push_back(dir_sep);
            nsrc.append(a->name);
            doRemove(nsrc);
        }
        if(!RemoveDirectory(oldf.c_str()))
            throw new OSException(ss.str(), strerror(errno));
    }else{
        if(!DeleteFile(oldf.c_str()))
            throw new OSException(ss.str(), strerror(errno));
    }
}

/* cesta do korene */

std::string OSInterface::getPrefix(){ return "C:\\"; }

/* vrati velikost souboru v bytech */

std::size_t OSInterface::getSize(std::string p){
    std::size_t size = 0;
    if(isDir(p)){
        return 0;
    }
    std::ifstream f(p, std::ios::binary | std::ios::ate);
    size = f.tellg();
    f.close();
    return size;
}

/* urci, je-li soubor predany parametrem adresar */

bool OSInterface::isDir(std::string path){

    DWORD ftype = GetFileAttributesA(path.c_str());
    if (ftype == INVALID_FILE_ATTRIBUTES)
        return false;
    if (ftype & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

/* ziska informace o souborech v adresari
 * projde adresar a ziska info o jednotlivych souborech,
 * to pak ulozi do vectoru dirs
 * nakonec vektor seradi, adresare na zacatek, protoze obsah musi korespondovat se zobrazenym seznamem
 */

void OSInterface::getDirInfo(std::string path, std::string pattern){
    WIN32_FIND_DATA data;
    repairPath(path);

    path.append("*");
    std::wstring ppath, whole_name;
    ppath.assign(path.begin(), path.end());
    path.pop_back();
    dirEntryT *de;
    HANDLE hFile = FindFirstFile(ppath.c_str(), &data);

    if  ((hFile == INVALID_HANDLE_VALUE) && (GetLastError() != ERROR_FILE_NOT_FOUND)) return;
    //throw new OSException(path, "Failed to open dir.");
    std::string name, tmpname;
    while(FindNextFile(hFile, &data) != 0 || GetLastError() != ERROR_NO_MORE_FILES)
    {
        de = new dirEntryT();
        whole_name = whole_name = data.cFileName;
        name.assign(whole_name.begin(), whole_name.end());
        tmpname = path + name;
        std::cout << name << std::endl;
        whole_name.assign(tmpname.begin(), tmpname.end());
        de->ext_name = getExtension(name);
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        if(!GetFileAttributesEx(whole_name.c_str(), GetFileExInfoStandard, &fileInfo)) continue;
        DWORD ftype = fileInfo.dwFileAttributes;
        if (ftype == INVALID_FILE_ATTRIBUTES) continue;
        // throw new OSException(name, "Invalid file");
        if((name == ".") || name == "..") continue;
        if((!matchExpression(name, pattern)) && !isDir(name)) continue;
        de->name = name;
        if((ftype & FILE_ATTRIBUTE_NORMAL) || (ftype & FILE_ATTRIBUTE_SYSTEM) || (ftype & FILE_ATTRIBUTE_COMPRESSED)){
            de->type = de->FILE;
            de->type_name = "FILE";
        }else if(ftype & FILE_ATTRIBUTE_DIRECTORY){
            de->type = de->DIR;
            de->type_name = "DIR";
        }else if(ftype & FILE_ATTRIBUTE_VIRTUAL){
            de->type = de->LINK;
            de->type_name = "LINK";
        }else{
            de->type = de->UNKNOWN;
            de->type_name = "UNKNOWN";
        }
        if(isArch(de->ext_name)){
            de->type_name = "ARCHIVE";
            de->type = de->ARCHIVE;
        }
        de->byte_size = getSize(name);
        SYSTEMTIME stUTC, stLocal;
        LPTSTR lpszString;
        FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
        char buf[255];
        sprintf(buf, "%02d.%02d.%d  %02d:%02d",
                stLocal.wDay, stLocal.wMonth, stLocal.wYear,
                stLocal.wHour, stLocal.wMinute);
        de->mod_time = std::string(buf);

        dirs.push_back(de);
    }
    std::sort(dirs.begin(), dirs.end(),[=](dirEntryT *d1, dirEntryT *d2){
        if ((d1->type == d1->DIR) && (d2->type != d2->DIR))
            return true;
        else if ((d1->type != d1->DIR) && (d2->type == d2->DIR))
            return false;
        else
            return d1->name < d2->name; });
}

#endif // __WIN32__
