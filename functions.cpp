#include "functions.h"
#include "osinterface.h"
#include <string>

/* upravi retezec ve vystupnim parametru aby koncil jednim oddelovacem
 * a neobsahoval zdvojene oddelovace
*/

void repairPath(std::string &path){
    int pos;
    std::string doubled;
    doubled.push_back(OSInterface::dir_sep);
    doubled.push_back(OSInterface::dir_sep);
    while((pos = path.find(doubled)) != std::string::npos){
        path = path.substr(0, pos) + path.substr(pos + 1, path.size());
    }
    if(path[path.size() - 1] != OSInterface::dir_sep)
        path.push_back(OSInterface::dir_sep);
}

/* porovna, jestli retezec expr odpovida vzoru */

bool matchExpression(std::string &expr, std::string &pattern){

    int pos = pattern.find('*');
    for(int i = 0; i < pos; ++i){
        if(expr[i] != pattern[i])
            return false;
    }

    for(unsigned int i = 0; i < pattern.size() - pos - 1; ++i){
        if(expr[expr.size() - 1 - i] != pattern[pattern.size() - 1 - i])
            return false;
    }
    return true;
}

/* vrati jmeno souboru ziskane z cesty */

std::string getBasename(std::string &path){
    int pos;
    if(path.empty()) return "";
    if((pos = path.find_last_of(OSInterface::dir_sep)) == -1) return "";
    return path.substr(pos + 1, path.size());
}

/* vrati cestu k souboru, bez samotneho jmena */

std::string getPath(std::string &path){
    int pos;
    if(path.empty()) return "";
    if((pos = path.find_last_of(OSInterface::dir_sep)) == -1) return "";
    return path.substr(0, pos);
}

/* vrati koncovku souboru */

std::string getExtension(std::string &path){
    if(path.empty()) return "";
    std::string fn = getBasename(path);
    int pos;
    if((pos = path.find_last_of('.')) == -1) return "";
    return path.substr(pos + 1, path.size());
}

/* rozhodne, zdali se jedna o archiv */

bool isArch(std::string &path){
    std::string ext(getExtension(path));
    return ((ext == "gz") || (ext == "bz2") || (ext == "zip") || (ext == "rar"));
}

/* jedna se o obrazek */

bool isImg(std::string &path){
    std::string ext(getExtension(path));
    return ((ext == "png") || (ext == "jpg") || (ext == "gif"));
}

/* soubor ma typ, ktery umi otevrit v externim programu */

bool isKnown(std::string &path){
    std::string ext(getExtension(path));
    auto it = extern_programmes.find(ext);
    if(it != extern_programmes.end())
        return true;
    return false;
}

/* pokud jmeno souboru obsahuje nepovolene znaky, vrati prvni z techto znaku */

char isValidFn(std::string &path){
    std::string forbidden(" <>|\:()&;#?*");
    forbidden.push_back(OSInterface::dir_sep);
    for(char &a : forbidden){
        if(path.find(a) != std::string::npos)
            return a;
    }
    return 0;
}
