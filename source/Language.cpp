#include "Language.h"
#include <string.h>

//  In the language files:
//    + serves as a /n for all intents and purposes
//    /n serves as an end - of - entry marker


int Language::Load(const char* name) {
  BFile *languageFile = new BFile (name, B_READ_ONLY);
  
  if (languageFile->InitCheck() != B_OK) return B_ERROR;
 
  for (int i = 0; i < L_MAX_LINES; i++) {
    strcpy( lines[i], "");
    char k = '.';
    while (k != '\n') {
      if (languageFile->Read(&k,1) != 1) return B_ERROR;
      if (k != '\n' && k != '+') strncat(lines[i],&k,1);
      if (k == '+') strncat(lines[i],"\n",1);
    }
  }

  return B_OK;
}

char* Language::Return(int num) {
  return lines[num];
}