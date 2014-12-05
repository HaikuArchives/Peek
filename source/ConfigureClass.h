#ifndef _DJM_ConfigureClass
#define _DJM_ConfigureClass

/*
Copyright © 2001, Durand John Miller
All rights reserved.


Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following 
conditions are met:

   1. Redistributions of source code must retain the
      above copyright notice, this list of conditions and 
      the following disclaimer.
   2. Redistributions in binary form must reproduce the above 
      copyright notice, this list of conditions and the following 
      disclaimer in the documentation and/or other materials provided 
      with the distribution.
   3. The name of the author may not be used to endorse or promote 
      products derived from this software without specific prior 
      written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER 
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN 
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <be/support/String.h>
#include <be/interface/Rect.h>
#include <be/storage/File.h>

class ConfigureClass
{
   public:
      ConfigureClass(const char *);
     ~ConfigureClass();
   
        bool ValueBool(const char*);
       BRect ValueBRect(const char*);
       int32 ValueInt32(const char*);
      void ValueChar(const char*, BString*);

      void Set(const char*, bool);
      void Set(const char*, BRect);
      void Set(const char*, int32);
      void Set(const char*, char*);
    
      bool IsNew();
   
   private:
      int32 setting_locate(  const char *, int32 *, BString *);
      int32 setting_replace( const char *, BString *);
      BString readLine(BFile*);
         void writeLine(BFile*, BString*);

      BString configure_filename;
      void random_string(int, BString *);
      void int_to_string(int, BString *);      

      bool new_file;
};

#endif
