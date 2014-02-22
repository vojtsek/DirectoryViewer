#include "functions.h"
#include <string>

bool matchExpression(std::string expr, std::string pattern){

  int pos = pattern.find('*');
  for(int i = 0; i < pos; ++i){
      if(expr[i] != pattern[i])
        return false;
    }

  for(int i = 0; i < pattern.size() - pos - 1; ++i){
      if(expr[expr.size() - 1 - i] != pattern[pattern.size() - 1 - i])
        return false;
    }
  return true;
}
