#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "LanguageConstants.h"
#include <File.h>

class Language {
   public:
     int Load(const char*);
     char* Return(int);
   
   private:
      int totalStrings;
      char lines[L_MAX_LINES][255];
};

#endif