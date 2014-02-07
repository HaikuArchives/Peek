#include "ConfigureClass.h"
#include <be/storage/FindDirectory.h>
#include <be/storage/Path.h>
#include <be/storage/File.h>
#include <be/storage/Entry.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

// ******************* CONSTRUCTORS & DECONSTRUCTORS **************

ConfigureClass::ConfigureClass(const char * fname)
{
  // Seed the random thing for the random_string method
   srand(time(NULL));
  // Now we locate the user's settings directory and prepend it
  // to the fname parameter to achieve a fully-qualified pathname
  BPath settings_path;
  find_directory(B_USER_SETTINGS_DIRECTORY, &settings_path);
  settings_path.Append(fname);
  configure_filename = settings_path.Path();
  // It's stored now in configure_filename for future reference

  // Make sure it exists!!  This is important
  BFile *test_file  = new BFile( configure_filename.String() , B_READ_ONLY );

    if ( test_file->InitCheck() != B_OK )
    {    // ok, doesn't exist. create it with the end string
       BString end_String = "**E_N_D** = done";
       test_file->SetTo(configure_filename.String() , B_WRITE_ONLY | B_CREATE_FILE);
       writeLine(test_file, &end_String );
       new_file = true;
    }
    else new_file = false;

  delete test_file;
}


ConfigureClass::~ConfigureClass()
{
}

bool ConfigureClass::IsNew()
{
  return (new_file);
}

// ************************ FILE ACCESS  ************************

// This will search the configuration file for a "setting_name" setting and
// return the line number and the string at that position.
// You MUST allocate the BString before you call this.
// Returns:   0 = Everything OK
//           -1 = couldn't open file
//           -2 = couldn't find the setting

int32 ConfigureClass::setting_locate(  const char *setting_name, int32 *line_number, BString *value)
{
   BString compare_string = setting_name;
           compare_string = compare_string.ToUpper();
   BFile *configure_file = new BFile(configure_filename.String(), B_READ_ONLY);
   if ( configure_file->InitCheck() != B_OK )
   {
      delete configure_file;
      return -1;
   }

   BString reader = "";
   BString left = "";
   BString right = "";
   int32 line = -1;
   
                             // **E_N_D** is our termination line. I hope no one uses this.
   while (left != "**E_N_D**")
   {
     left = "";
     right = "";

     reader = readLine(configure_file);
     line++;

       // split reader up into it's two sides
     left.Append(reader.String(), reader.FindFirst("="));
     left = left.ToUpper();
     right = reader; 
     right.Remove(0,reader.FindFirst("=")+2);
     left.RemoveAll(" ");
//     right.RemoveAll(" ");
     if (left == compare_string) 
     {
       value->SetTo(right);    // return the value
       delete configure_file;
       *line_number = line;
       return 0;
     }
   }

   *line_number = -1;
   delete configure_file;
   return -2;
}

// This will just replace the setting "setting_name" with the whole string
// value.
// Returns:   0 = Everything OK 
//            1 = Added new setting
//           -1 = couldn't open file

int32 ConfigureClass::setting_replace( const char *setting_name, BString *value)
{
   BString compare_string = setting_name;
           compare_string = compare_string.ToUpper();
   // open the file
   BFile *configure_file = new BFile(configure_filename.String(), B_READ_ONLY);
      // Check to make sure it opened correctly
   if ( configure_file->InitCheck() != B_OK )
   {
      delete configure_file;
      return -1;                // if not, return -2
   }

   // Create the temp file
    // first get a random filename
     BString temp_file = "";
     random_string(10, &temp_file);
     temp_file.Prepend(configure_filename);
   // Open the file
   BFile *temp = new BFile( temp_file.String(), B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE );

   BString reader = "";
   BString left = "";
   BString right = "";
   bool found = false;
                             // **E_N_D** is our termination line. I hope no one uses this.
   while (left != "**E_N_D**")
   {
     left = "";
     right = "";

     reader = readLine(configure_file);

     left.Append(reader.String(), reader.FindFirst("="));
     left = left.ToUpper();
     right = reader; 
     right.Remove(0,reader.FindFirst("=")+2);
     left.RemoveAll(" ");
//     right.RemoveAll(" ");
     if (left == compare_string) 
     {
       right = *value;
       found = true;
     }
     if (left == "**E_N_D**") break;
     left.Append( " = " );
     left.Append( right );
     writeLine(temp,&left);
   }

   if (found == false)
   {
      BString to_add = compare_string;
      to_add.Append(" = ");
      to_add.Append( *value );
      writeLine(temp,&to_add);
   }
   

   left = "**E_N_D** = done";
   writeLine(temp,&left);

   delete configure_file;
   delete temp;

   // Now fix what we've done by renaming the new file to the
   // old file name
   BEntry *pt = new BEntry(temp_file.String());
           pt->Rename( configure_filename.String(), true );
          delete pt;
          
   return 0;
}

// ************** SIMPLE read & write ************************

// ** Simple readLine from an open BFile
BString ConfigureClass::readLine(BFile* file)
{
  BString buffer;
  buffer = "";
  char k;
  while (file->Read(&k,1) == 1)
  {
     if (k != '\n') buffer.Append(&k,1);
               else break;
  }
  return buffer;
}

// Simple writeline to an open BFile with the string
void ConfigureClass::writeLine(BFile* file, BString* string)
{
  file->Write(string->String(), string->Length() );
  file->Write("\n", 1 );
}


// ******************* SET-ING FUNCTIONS ************************

//eg: this->Set("ALLOW_ALL_USERS",true);
//eg: this->Set("WINDOW_FRAME", BRect(50,50,400,400) );
//eg: this->Set("NUMBER_OF_PEOPLE", 600 );
//eg: this->Set("USERNAME","sammy");

void ConfigureClass::Set(const char* setting_name, bool value)
{
  BString to_add = "";
  if (value) to_add = "YES";
        else to_add = "NO";
  setting_replace(setting_name, &to_add );
}

void ConfigureClass::Set(const char* setting_name, BRect value)
{
  int32 top, bottom, left, right;
  
  top = (int32)value.top;
  left = (int32)value.left;
  bottom = (int32)value.bottom;
  right = (int32)value.right;

  BString total = "";
  BString temp;

  int_to_string(left,&temp);
  total.Append(temp);  total.Append("-");
  int_to_string(top,&temp);
  total.Append(temp);  total.Append("-");
  int_to_string(right,&temp);
  total.Append(temp);  total.Append("-");
  int_to_string(bottom,&temp);
  total.Append(temp);

  setting_replace(setting_name,&total);

}

void ConfigureClass::Set(const char* setting_name, int32 value)
{
  BString num;
  int_to_string(value,&num);
  setting_replace(setting_name,&num);
}

void ConfigureClass::Set(const char* setting_name, char* value)
{
  BString to_add = value;
  setting_replace(setting_name, &to_add );
}


// ******************* VALUE-ING FUNCTIONS ************************

// This checks for YES(true) or NO(false). If the setting doesn't exist, it will
// return false. Hmm.. so I guess it just checks for YES.
bool ConfigureClass::ValueBool(const char* setting_name)
{
  int32 line_num;
  BString setting;
  if (setting_locate(setting_name,&line_num,&setting) == 0)
  {
    if (setting == "YES") return true;
  }
  return false;
}

// This will just return the BRect value of setting_name.
// It's stored as x1-y1-x2-y2 in the file.
// If it doesn't exist, it will return (0,0,0,0)
BRect ConfigureClass::ValueBRect(const char* setting_name)
{
  int32 line_num;
  BString setting;
  BRect return_value;
  return_value.Set(0,0,0,0);
  if (setting_locate(setting_name,&line_num,&setting) == 0)
  {
    float x1,y1,x2,y2;
    BString number, full;
    full = setting;
    
    number = "";
    number.Append(full, full.FindFirst("-") );
    x1 = atof(number.String());
    full.Remove(0,full.FindFirst("-") + 1);

    number = "";
    number.Append(full, full.FindFirst("-") );
    y1 = atof(number.String());
    full.Remove(0,full.FindFirst("-") + 1);

    number = "";
    number.Append(full, full.FindFirst("-") );
    x2 = atof(number.String());
    full.Remove(0,full.FindFirst("-") + 1);

    number = full;
    y2 = atof(number.String());
    
    return_value.Set(x1,y1,x2,y2);
  }
  return return_value;
}

int32 ConfigureClass::ValueInt32(const char* setting_name)
{
  int32 line_num;
  BString setting;
  int32 return_value;
  return_value = 0;
  if (setting_locate(setting_name,&line_num,&setting) == 0)
  {
    return_value = atol(setting.String());
  }
  return return_value;
}

void ConfigureClass::ValueChar(const char* setting_name, BString* ans)
{
  int32 line_num;
  BString setting;
  BString return_value;
  return_value = "";
  if (setting_locate(setting_name,&line_num,&setting) == 0)
  {
    return_value = setting;
  }
  *ans = return_value;
}


// ************************** STUFF

// Just generate a random string of length "n" and return it in p;
void ConfigureClass::random_string(int n, BString *p)
{
   BString ret = "";
   int i = 0;
   
   for ( i = 0; i < n; i++ )
   {
      char k;
      k = 'A' + (random() % 26);
      ret = ret.Append( k , 1 );
   }
   
   *p = ret;
}



// As the name says,
void ConfigureClass::int_to_string(int num, BString * ans)
{
   BString result = "";

   int temp = num;

    // special case: n = 0 => we just need to return "0"
   if (num == 0)
    {
      *ans = "0";
      return;
    }

   
   while (temp > 0)
   {
    char k = '0' + (temp % 10);
    temp = temp / 10;
    result.Prepend(&k,1);
   }

   *ans = result;
}


// -------------------------------- FINISHED ------------------------------


