#ifndef _P_LIST_ITEM_H
#define _P_LIST_ITEM_H

#include <ListItem.h>
#include <View.h>
#include <Entry.h>
#include <stdlib.h>

class PListItem : public BListItem 
{
  public: 
   PListItem(BEntry *, char * = NULL);
   PListItem(entry_ref e, bool isD = false, char *name = NULL);
   
   ~PListItem();
   virtual void DrawItem(BView *, BRect, bool);
   int32 GetEntry(BEntry *);
   entry_ref Ref();
   
  private:
   bool isDirectory;
   entry_ref *theFile;
   char theName[B_FILE_NAME_LENGTH];
};

#endif
