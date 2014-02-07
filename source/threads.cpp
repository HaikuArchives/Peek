// THREAD FOR SLOW SELECTION

#include "WindowPeek.h"
#include <math.h>
#include <stdio.h>

int32 selectionWait(void* data) {
  ViewFile *parent = (ViewFile*)data;
  int32 oldNum = 0;
  int32 veryOldNum = 0;
  while (1==1) {
    oldNum = parent->Selected();
    snooze(300000);    
    if (oldNum == parent->Selected() && oldNum != veryOldNum) {
     veryOldNum = oldNum;
     parent->threadCall(oldNum);
    }
  }
  return B_OK;
}

int32 slideshowThread(void*data) {
  ViewFile *parent = (ViewFile*)data;
  bool firstRun = true;
  while ( true ) {

    if (firstRun == false) snooze( parent->setup->SlideshowDelay() );
                      else firstRun = false;

    if (parent->setup->Sliding()) parent->Slideshow();

  }
  return B_OK;
}

// END OF THREAD
