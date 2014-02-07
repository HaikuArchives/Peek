#ifndef VIEW_FILE_H
#define VIEW_FILE_H

#include <View.h>
#include <NodeMonitor.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Mime.h>
#include "ViewFileList.h"
#include "PListItem.h"
#include <OS.h>
#include <ScrollView.h>
#include <StatusBar.h>
#include "constants.h"
#include "Setup.h"
#include "Language.h"
#include <Path.h>
#include <List.h>


class ViewFile : public BView {
   public:
      ViewFile(BRect, char*, char*, Setup*, Language*);
      void ChangeDirectory( const char* );
      int32 totalOfType(char* );
      int32 totalImages();
      int32 Selected();
      BList* SelectedList();
      void Select(int32);
      bool SelectNextImage(int32);
      void Slideshow();
      int32 TurnSlideshowOn();
      int32 TurnSlideshowOff();

      BEntry *EntryAt(int32);
      void threadCall(int32);
      void Notified();
      int32 BuildListing();

      void Select(BEntry*);
      Language *words;
      Setup *setup;
      ViewFileList *fileList;
   private:
      int32 totalImageFiles;
      virtual void DetachedFromWindow();
      thread_id selectionThread, slideshowThreadID;
      char currentPath[B_PATH_NAME_LENGTH];
};


#endif

