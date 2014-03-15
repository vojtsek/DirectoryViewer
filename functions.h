#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>

void repairPath(std::string &);
bool matchExpression(std::string &, std::string &);
std::string getBasename(std::string &);
std::string getPath(std::string &);
std::string getExtension(std::string &);
bool isArch(std::string &);
bool isImg(std::string &);
bool isKnown(std::string &);
char isValidFn(std::string &);

#endif // FUNCTIONS_H
