#include "ViewFileList.h"
#include <Application.h>
#include <Alert.h>
#include <string.h>
#include "ViewFile.h"
#include "WindowPeek.h"


ViewFileList::ViewFileList(char* name, Setup* s, Language* w) 
:  BListView(name , B_SINGLE_SELECTION_LIST, B_FRAME_EVENTS | B_WILL_DRAW) //This supports Layout Management.
{
  setup = s;
  words = w;
}

bool ViewFileList::InitiateDrag(BPoint a, int32 index, bool selected) {
  if (selected == false) return false;

   PListItem *sam = ((PListItem*) ItemAt(index));
   if (sam == NULL) return false;
   BEntry *tempOne = new BEntry();
   if (sam->GetEntry(tempOne) == B_OK ) {
     entry_ref tmp;
     tempOne->GetRef(&tmp);   

     // get picture here
     BNode noddy(tempOne);
     BNodeInfo noddi(&noddy);
     
     BBitmap *snoopy = new BBitmap( BRect(0,0,31,31), B_CMAP8);
     noddi.GetTrackerIcon(snoopy,B_LARGE_ICON);
     
     BMessage *dragee = new BMessage(B_SIMPLE_DATA);
     dragee->AddRef("refs",&tmp);
     dragee->AddString("source","Peek");
      
     DragMessage(dragee, snoopy, B_OP_ALPHA, BPoint(15,15), NULL);
     
     delete dragee;
   } 
   delete tempOne;  

  return true;
}


void ViewFileList::MessageReceived(BMessage *e) {
  switch (e->what) {
    case B_MOUSE_WHEEL_CHANGED: Window()->PostMessage(e); break;
    case B_SIMPLE_DATA:  {
             if (e->FindString("source") != NULL) 
                if (strcmp(e->FindString("source"),"Peek")==0 ) break;
             // moving or copying? hmm..
             status_t stattoo;
             entry_ref dropped;
             bool overwriteAll = false;
             BEntry *heyMan;
             char path[B_PATH_NAME_LENGTH];
             setup->CurrentPath(path);
             BDirectory *heyDude = new BDirectory( path );
             int32 i = 0;
             while (e->FindRef("refs",i++,&dropped) == B_OK) {

               heyMan = new BEntry(&dropped);
               stattoo = heyMan->MoveTo(heyDude, NULL, overwriteAll);
               if (stattoo == B_FILE_EXISTS) {       // exists => overwrite?
                 char overwrite[255];
                 strcpy(overwrite,dropped.name);
                 strcat(overwrite,words->Return(L_REPLACE_FILE));
                 stattoo = B_OK;
                 int32 ok = ((new BAlert("overWrite",overwrite,words->Return(L_YES_TO_ALL),words->Return(L_YES),words->Return(L_NO)))->Go());
                 if (ok == 0) { overwriteAll = true; ok = 1;}
                 if (ok == 1) {
                               stattoo = heyMan->MoveTo(heyDude, NULL, true);
                              }
               }
               if (stattoo != B_OK) {        // some error!!!
                                       ((new BAlert("darnnit",words->Return(L_ERROR_COPYING),words->Return(L_OK)))->Go());
                                       delete heyMan;
                                       break;
                                     } 
               delete heyMan;                 // delete it.

             }
             delete heyDude;
             break;
           } 
    default: BListView::MessageReceived(e); break;
  }
}

