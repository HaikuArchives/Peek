#include <MenuItem.h>
#include <Message.h>
#include "Setup.h"
#include "constants.h"

// menu helpers -----------------------------------------------------------

BMenuItem* menuMaker(char* label, BMessage* send,char trigger, bool marked) {
  BMenuItem *bob = new BMenuItem( label, send, trigger );
  bob->SetMarked(marked);
  return bob;
}

BMenuItem* slideshowHelper( char* label, int32 delay, Setup* setup ) {
      BMessage *msg = new BMessage(PEEK_SLIDESHOW_DELAY);
      msg->AddInt32("delay",delay);
      BMenuItem *temp = new BMenuItem(label,msg   );
      if (setup->SlideshowDelay() == (unsigned long)(delay * 1000000) ) temp->SetMarked(true);  
     return temp;
}

BMenuItem* mouseHelper( char* label, int32 button, int32 opp, Setup* setup ) {
      BMessage *msg = new BMessage( PEEK_MOUSE_CHANGE );  
      msg->AddInt32("button",button);  
      msg->AddInt32("action",opp);
      BMenuItem *temp = new BMenuItem(label,msg );

      if (button == 0) if (setup->LeftMouseAction() == opp)   temp->SetMarked(true);
      if (button == 1) if (setup->MiddleMouseAction() == opp) temp->SetMarked(true);
      if (button == 2) if (setup->RightMouseAction() == opp)  temp->SetMarked(true);

      return temp;
}


// ---------------- end menu helpers

