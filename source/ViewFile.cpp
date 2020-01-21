#include "ViewFile.h"
#include <Window.h>
#include <Application.h>
#include "WindowPeek.h"
#include <math.h>
#include <stdio.h>


// external threads -- see threads.cpp for the actual functions
extern int32 selectionWait(void *);
extern int32 slideshowThread(void *);


ViewFile::ViewFile(BRect R, char* name, char* path, Setup *s, Language *w)
:  BView(R,name, B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW | B_FRAME_EVENTS)
{
   setup = s;
   words = w;
   strcpy( currentPath, path);
   fileList = new ViewFileList( BRect(10, 10, R.Width() - 5 -  B_V_SCROLL_BAR_WIDTH, R.Height() - 10 - B_H_SCROLL_BAR_HEIGHT), "FileList", setup, words);
   fileList->SetInvocationMessage(new BMessage(PEEK_FILE_INVOKED));
   fileList->SetSelectionMessage(new BMessage(PEEK_FILE_SELECTED));

   BuildListing();

   BScrollView *fileListScroll = new BScrollView("fileScroll",fileList, B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS, true, true);

   AddChild(fileListScroll);

   selectionThread = spawn_thread(selectionWait, "selectionThread", B_NORMAL_PRIORITY  , (void*)this);
   resume_thread(selectionThread);
   slideshowThreadID = spawn_thread(slideshowThread, "slideshowThread", B_NORMAL_PRIORITY  , (void*)this);
    
};


void ViewFile::ChangeDirectory(const char* path ) {
  setup->CurrentPath( currentPath );
  if (strcmp(currentPath,path)!=0) {
     setup->SetCurrentPath(path);
     BuildListing();
  }
}

int32 ViewFile::Selected() {
  return fileList->CurrentSelection();
}

void ViewFile::Select(int32 num) {
  fileList->Select(num);
  fileList->ScrollToSelection();
}


// SelectedList -- This will return a BList containing the entry_ref's of all the files
//                 or directories selected in the file list.
//                 NB: if 1 or less entries are selected, the method will return NULL
//                 YOU MUST DELETE THE BLIST *AND* THE ENTRY_REFS when you're done.
BList* ViewFile::SelectedList() {
     BList *result = new BList();
     int32 selected = 0;
     int32 count = 0;
     while ( (selected = fileList->CurrentSelection(count)) >= 0 ) {
         PListItem* sam = (PListItem*) fileList->ItemAt( selected );
         entry_ref *toAdd = new entry_ref( sam->Ref() );
         result->AddItem( toAdd );
         count ++;
     }

     if (count <= 1) {
          if (count == 1) {
                             entry_ref *toDel = (entry_ref*) result->ItemAt(0);
                             result->MakeEmpty();
                             delete toDel;
                          }
          delete result;
          result = NULL;
     }
     return result;
}


// EntryAt -- returns the BEntry of the currently selected file in the
//            file list.  DELETE the BEntry when you're done with it.
//            It is yours....
BEntry* ViewFile::EntryAt(int32 num) {
  BEntry *bob = new BEntry();
  ((PListItem*)fileList->ItemAt(num))->GetEntry(bob);
  return ( bob );
}

// SelectNextImage -- This selects the next image in the file list in the direction
//                    specified by dir.  ( dir >= 0 ) is down the list.  ( dir < 0 )
//                    is up the list.  
//                    This returns true if there really was another image to load.
//                                 false if there are no other images in the list and
//                                       we've looped round to the starting selection.
bool ViewFile::SelectNextImage(int32 dir) {
  bool found = false;
  int32 direction;
  if (dir < 0) direction = -1;
          else direction =  1;
  int32 start   = fileList->CurrentSelection();
  if (start < 0) {                                   // is something isn't selected, choose the last in the list.
                   start = 0;
                 }  
  int32 current = start + direction;
  while ( current != start ) {
     if (current > fileList->CountItems()-1) current = 0;
     if (current < 0) current = fileList->CountItems() - 1;

     BEntry *bob = new BEntry();
     ((PListItem*) (fileList->ItemAt(current) ))->GetEntry(bob);
     if (((WindowPeek*)Window())->IsType(bob,"image")) { 
                                                          found = true;
                                                          delete bob;
                                                          break;
                                                       }  
     delete bob;
     current += direction;
  }

  if (found)  fileList->Select(current);
  return found;
}

// totalOfType -- returns the total number of files of type *s (eg. "image", "image/jpeg")
//                in the current directory.

int32 ViewFile::totalOfType(char* s) {
  int32 total = 0;
  int32 i = fileList->CountItems();
  for (int32 k = 0; k < i; k ++) {
     BEntry *bob = new BEntry();
     ((PListItem*) (fileList->ItemAt(k) ))->GetEntry(bob);
     if (((WindowPeek*)Window())->IsType(bob,s)) total++;
     delete bob;
  }
 return total;
}

// totalImages   -- returns the total number of image file
//                  in the current directory.

int32 ViewFile::totalImages() {
 return totalImageFiles;
}



// threadCall -- The selectionWait thread calls this method if the selection in the list
//               has been there for a decent amount of time (0.6 seconds).  This makes it
//               easier to scroll through lists without having to load each image.
void ViewFile::threadCall(int32 num) {
   if (fileList->CurrentSelection() < 0) return;
   if (setup->Sliding()) return;


   PListItem *sam;

   sam = (PListItem*) fileList->ItemAt( fileList->CurrentSelection() );
   entry_ref *refToAdd =  new entry_ref;
   *refToAdd = sam->Ref();
      
   BEntry *temp = new BEntry( refToAdd );

   WindowPeek* motherWindow = ((WindowPeek*)Window());

   if (motherWindow->IsType(temp,"image"))
   {
           motherWindow->LoadImage(temp);
   }

   delete temp;
}


// Slideshow -- This method is called by the slideshowThread to
//              carry on with the slideshow when required.
//


void ViewFile::Slideshow() {
  Window()->Lock();
  int32 direction = 0;
  if (setup->SlideshowMode() == PEEK_SLIDESHOW_DESCENDING) direction = 1;
  if (setup->SlideshowMode() == PEEK_SLIDESHOW_ASCENDING) direction = -1;

  if (direction != 0)
  {
     if (SelectNextImage(direction) == false) 
     {
          BMessage *msg = new BMessage(PEEK_TOGGLE_SLIDE_SHOW);
          be_app->PostMessage(msg, Window());
          delete msg;
     }     
  }
  else
      {
          int32 oldNum = fileList->CurrentSelection();
          BEntry *toLoad = new BEntry();
          do 
          {
            oldNum = (oldNum + rand() % fileList->CountItems()) % fileList->CountItems();
            ((PListItem*) fileList->ItemAt(oldNum)) -> GetEntry(toLoad);
          }  while ( (((WindowPeek*)Window())->IsType(toLoad, "image") == false) );
          fileList->Select( oldNum );
          delete toLoad;
      }

 WindowPeek* motherWindow = ((WindowPeek*)Window());
 threadCall(0);
 if (setup->SlideLoadIntoCenter()) motherWindow->CenterWindow();

 Window()->Unlock();
}


//  TurnSlideshowOn --  resumes the thread
int32 ViewFile::TurnSlideshowOn() {
   if (totalImages() <= 1) return B_ERROR;
   resume_thread(slideshowThreadID);
   return B_OK;
}

//  TurnSlideshowOff -- suspends the thread
int32 ViewFile::TurnSlideshowOff() {
   suspend_thread(slideshowThreadID);
   return B_OK;
}

// Select -- it will highlight and scroll to the BEntry supplied it, if it exists within 
//           the list.
//
void ViewFile::Select(BEntry* f) {
  char name[B_FILE_NAME_LENGTH];
  char nameTwo[B_FILE_NAME_LENGTH];
  f->GetName(name);
  int32 num = fileList->CountItems();
  BEntry *temp = new BEntry();
  for (int32 i = 0; i < num; i++) {
      PListItem *sam = (PListItem*)(fileList->ItemAt(i));
      sam->GetEntry(temp);
      temp->GetName(nameTwo);
      if (strcmp(nameTwo,name) == 0) {
        fileList->Select(i);
        break;
      }
  }
  delete temp;
  fileList->ScrollToSelection();
}


// Notified --  What the hell does this do?  oh.. when the directory is changed, we are
//              updated and then the list refreshes. (  node-watcher stuff & "user pressed enter" stuff) 
//
void ViewFile::Notified() {
   if (setup->Sliding()) TurnSlideshowOff();
   
   if (fileList->CurrentSelection() < 0) return;
   
   PListItem *sam = ((PListItem*) fileList->ItemAt(fileList->CurrentSelection()));
   if (sam == NULL) return;

   BEntry *tempOne = new BEntry();
   sam->GetEntry(tempOne);
   BPath tempPath(tempOne);
   delete tempOne;
   
   setup->SetCurrentPath( tempPath.Path() );    // put the new current path into the system.(ie Setup object)

   BuildListing();
}



// BuildListing --  This builds the list that you see.  It also counts all the image files in the
//                  directory so that it speeds up other file list operations later on.
//
int32 ViewFile::BuildListing() {

  setup->CurrentPath(currentPath);
  totalImageFiles = 0;

  fileList->Hide();
  int32 counter = fileList->CountItems();
  for (int32 i=0; i < counter; i++) {
    PListItem *scaredOne = (PListItem*)fileList->RemoveItem( (int32) 0);
    delete scaredOne;
  }

  fileList->Show();


  fileList->MakeEmpty();

  BDirectory tempDirectory(currentPath);
  if (tempDirectory.InitCheck() != B_OK) return B_ERROR;
  
  node_ref thisDirNode;
  status_t stat;
  
    stop_watching(be_app_messenger);
    tempDirectory.GetNodeRef(&thisDirNode);
    stat = watch_node(&thisDirNode, B_WATCH_DIRECTORY, be_app_messenger);

  // HERE COMES THE MAJOR SPEEDING UP THINGY
  //   --- ADD EVERYTHING TO THE BLIST newList and then
  //         ADDLIST (newList );
  BList *newList = new BList();
    
  tempDirectory.Rewind();
  BEntry tempEntry(currentPath,true);
  tempEntry.GetParent(&tempEntry);

  if (strcmp(currentPath,"/") != 0) 
    newList->AddItem( (void*)(new PListItem( &tempEntry, ".." )) );

  entry_ref tempRef;
  bool imagesOnly = setup->ImagesOnly();
  WindowPeek* motherWindow = (WindowPeek*) Window(); 
  bool isDir;
  PListItem *newPList;


  // Handling thumbnail stuff here
    BList *theRefs = new BList();
    bool thumbMode = false;
  // finish thumbnail

  // Main listing loop!!  This needs to be as fast as possible!
  while ( tempDirectory.GetNextEntry( &tempEntry, true ) == B_OK ) {
    tempEntry.GetRef( &tempRef );
   
    if (tempEntry.IsDirectory()) isDir = true;
                            else isDir = false;

    if (!isDir) {
	BNode tempNode( &tempEntry );
	BNodeInfo tempNodeInfo( &tempNode );
	char mimeType[B_MIME_TYPE_LENGTH];

	if (tempNodeInfo.GetType(mimeType) != B_OK)
		continue;

	strtok(mimeType, "/");

	if (strcmp(mimeType, "image") != 0)
		continue;
    }

    newPList = new PListItem( tempRef, isDir );                            

    if ( isDir ) newList->AddItem( (void*) newPList  );
    else {
        if (motherWindow->IsType(&tempEntry,"image") ) {
                       // increment and add to the file list
                       totalImageFiles ++;
                       newList->AddItem( (void*) newPList );  
                        // now add to the ThumbnailList
                       if ( thumbMode ) {
                              entry_ref *tempThumbRef = new entry_ref;
                              *tempThumbRef = tempRef;
                              theRefs->AddItem( (void*) tempThumbRef );
                        }
                        // done;
                   } else                   
                        if (imagesOnly == false) newList->AddItem( (void*) newPList );  
     }
  }
  // end main loop
 

  fileList->AddList( newList );  // Add the list here

  delete theRefs;
  delete newList;

  return B_OK;
}


//  DetachedFromWindow -- This must kill the slideshow and selection threads and
//                        make sure that setup->Sliding = false just in case;
//
void ViewFile::DetachedFromWindow() {
  kill_thread(selectionThread);
  kill_thread(slideshowThreadID);
  setup->SetSliding(false);
}

