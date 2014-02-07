#include "PListItem.h"

PListItem::PListItem(BEntry *e, char *name) : BListItem()
{
 theFile = NULL;
 theFile = new entry_ref;
 e->GetRef(theFile);
 if (name == NULL) e->GetName(theName);
 else strcpy(theName,name);  
 if (e->IsDirectory()) isDirectory = true;
    else isDirectory = false;
}

PListItem::PListItem(entry_ref e, bool isD, char *name) : BListItem()
{
 theFile = NULL;
 theFile = new entry_ref;
 *theFile = e;
 if (name == NULL) strcpy( theName, e.name);
  else strcpy(theName,name);  
 isDirectory = isD;
}


PListItem::~PListItem() {
  if (theFile != NULL) delete theFile;
}

// GetEntry - requires that e declared before the method is entered!
//            the BEntry is yours, so you delete it and stuff.
int32 PListItem::GetEntry(BEntry *e) {
 if (theFile == NULL) return -1;
 e->SetTo(theFile,true);
 return B_OK;
}

// Ref() -- returns the actual ref
//          Don't delete it, dude.
entry_ref PListItem::Ref() {
  return *theFile;
}



void PListItem::DrawItem(BView *owner, BRect frame, bool complete) {
  owner->SetDrawingMode(B_OP_OVER);
  if (complete) {
    owner->SetHighColor(owner->ViewColor());
    owner->FillRect(frame);
  }  
  if (IsSelected()) { 
     owner->SetHighColor(142,142,142,255);
     owner->FillRect(frame);
   }  else {
   }


  if (isDirectory) {
     owner->SetHighColor(100,0,0,0);
     owner->MovePenTo(frame.left+4, frame.bottom-2);
  } else { 
      owner->SetHighColor(0,0,0,0);
      owner->MovePenTo(frame.left+10, frame.bottom-2);
    }     

  owner->DrawString(theName);
}
