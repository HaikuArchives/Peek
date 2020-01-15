#ifndef VIEW_FILE_LIST_H
#define VIEW_FILE_LIST_H

#include <ListView.h>
#include <Directory.h>
#include <Entry.h>
#include "PListItem.h"
#include <NodeInfo.h>
#include <Node.h>
#include <Bitmap.h>
#include "LanguageConstants.h"
#include "Setup.h"
#include "Language.h"

class ViewFileList : public BListView {
   public:
     ViewFileList( char*, Setup*, Language*);
     virtual void MessageReceived(BMessage *);
   private:
     Setup *setup;
     Language *words;
     virtual bool InitiateDrag(BPoint, int32, bool);
};


#endif 
