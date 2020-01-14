#include "WindowPeek.h"
#include "PeekApp.h"
#include <NodeMonitor.h>
#include <Alert.h>
#include <GridLayout.h>
#include <be/kernel/fs_info.h>
#include <be/storage/Volume.h>
#include <be/translation/TranslationUtils.h>

extern BMenuItem* menuMaker(char* label, BMessage* send,char trigger, bool marked);
extern BMenuItem* slideshowHelper( char* label, int32 delay, Setup* setup );
extern BMenuItem* mouseHelper( char* label, int32 button, int32 opp, Setup* setup );


// Save as Menu

BMenu* WindowPeek::SaveAsMenu( char* name, int32 msgnum) {
    BMenu *tempItemB= new BMenu(name);

    translator_id *list;
    int32 num;
    BTranslatorRoster::Default()->GetAllTranslators(&list,&num);
 
      for (int i=0; i < num; i++) {
         int32 numOfTypes;
         const translation_format *in_types,*out_types;            // WE'RE LOOKING FOR ALL TYPES THAT ACCEPT BBITMAPS AS INPUT
         BTranslatorRoster::Default()->GetInputFormats(list[i],&in_types,&numOfTypes);
         
         for (int j = 0; j <numOfTypes; j++) {
          if (in_types[j].type == B_TRANSLATOR_BITMAP) {
             BTranslatorRoster::Default()->GetOutputFormats(list[i],&out_types,&numOfTypes);
              for (int po = 0; po < numOfTypes; po ++) 
                if (out_types[po].type != B_TRANSLATOR_BITMAP) {
                   // Now we set up the internal messages
                   BMessage *msg = new BMessage(msgnum);
                   msg->AddInt32("translator_id",list[i]);
                   msg->AddInt32("type_const",out_types[po].type);
                   msg->AddString("type_name",out_types[po].name);
                   msg->AddString("mime_type",out_types[po].MIME);
                   tempItemB->AddItem(new BMenuItem(out_types[po].name,msg));
                 }   
              break;
          }
         }
      }

  return tempItemB;

}

// ------ Constructor

WindowPeek::WindowPeek( BRect R, char* name , Setup* s, Language *w)
: BWindow(R,name,B_DOCUMENT_WINDOW,B_WILL_ACCEPT_FIRST_CLICK | B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS) {

  setup = s;
  words = w; 

  // ---- MENU BAR
  mainMenu = new BMenuBar(BRect(0,0,0,0), "menuBar");

  BMenu *tempMenu, *tMenu, *bMenu;
  tempMenu = new BMenu( words->Return(L_BMENU_FILE));
  

  tempMenu->AddItem(  SaveAsMenu( words->Return(L_BMENUITEM_SAVE_AS), PEEK_SAVE_AS ) );
  tempMenu->AddSeparatorItem();
  tempMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_TRASH),  new BMessage(PEEK_DELETE_FILE) , 'T' )  );
  tempMenu->AddSeparatorItem();
  tempMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_QUIT),  new BMessage(B_QUIT_REQUESTED) )  );
  
  mainMenu->AddItem(tempMenu);  

  // Options Menu

  tempMenu = new BMenu( words->Return(L_BMENU_OPTIONS) );    

    tMenu = new BMenu( words->Return( L_BMENUITEM_SLIDESHOW) );

     bMenu = new BMenu( words->Return( L_BMENUITEM_DELAY) );

       BMessage *msg;
       BMenuItem *temp;

       bMenu->AddItem( slideshowHelper( "1 s", 1, setup ) );
       bMenu->AddItem( slideshowHelper( "2 s", 2, setup ) );
       bMenu->AddItem( slideshowHelper( "4 s", 4, setup ) );
       bMenu->AddItem( slideshowHelper( "8 s", 8, setup ) );
       bMenu->AddItem( slideshowHelper( "16 s", 16, setup ) );
       bMenu->AddItem( slideshowHelper( "32 s", 32, setup ) );
       bMenu->AddItem( slideshowHelper( "1 min", 60, setup ) );
       bMenu->AddItem( slideshowHelper( "2 min", 120, setup ) );
       bMenu->AddItem( slideshowHelper( "4 min", 240, setup ) );
       bMenu->AddItem( slideshowHelper( "5 min", 300, setup ) );
       bMenu->AddItem( slideshowHelper( "10 min", 600, setup ) );


     tMenu->AddItem(bMenu);
     
     bMenu = new BMenu( words->Return( L_BMENUITEM_SEQUENCE) );
     
      
      temp = new BMenuItem( words->Return(L_BMENUITEM_DESCENDING),  				new BMessage(PEEK_SLIDESHOW_DESCENDING) ); 
       if (setup->SlideshowMode() == PEEK_SLIDESHOW_DESCENDING ) temp->SetMarked(true);
       bMenu->AddItem(  temp );            
      temp = new BMenuItem( words->Return(L_BMENUITEM_ASCENDING),  				new BMessage(PEEK_SLIDESHOW_ASCENDING) );
       if (setup->SlideshowMode() == PEEK_SLIDESHOW_ASCENDING ) temp->SetMarked(true);
       bMenu->AddItem( temp  );
      temp = new BMenuItem( words->Return(L_BMENUITEM_RANDOM),					  	new BMessage(PEEK_SLIDESHOW_RANDOM) );
       if (setup->SlideshowMode() == PEEK_SLIDESHOW_RANDOM) temp->SetMarked(true);       
       bMenu->AddItem( temp  );

     tMenu->AddItem(bMenu);
     
     tMenu->AddSeparatorItem();

      temp = new BMenuItem( words->Return(L_BMENUITEM_LOAD_INTO_CENTER),  				new BMessage(PEEK_LOAD_INTO_CENTER) ) ;
      if (setup->SlideLoadIntoCenter()) temp->SetMarked(true);
       tMenu->AddItem( temp  );            

     
   tempMenu->AddItem(tMenu);

    tMenu = new BMenu( words->Return( L_BMENU_LANGUAGE) );

    CreateLanguageMenu(tMenu, PEEK_LANGUAGE_CHANGE);
     
   tempMenu->AddItem(tMenu);
   
    tMenu = new BMenu( words->Return( L_BMENUITEM_MOUSE) );

         bMenu = new BMenu ( words->Return(L_BMENUITEM_MAPPING) );
         
         BMenu *cMenu;
         
           cMenu = new BMenu ( words->Return(L_BMENUITEM_LEFT_BUTTON) );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_SCROLLING),  0, P_SCROLLING,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_DRAGGING),   0, P_DRAGGING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_CLIPPING),   0, P_CLIPPING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_POPUP_MENU), 0, P_POPUPMENU,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_OPEN_NEW),   0, P_OPENNEW,    setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_FULL_SCREEN),0, P_FULLSCREEN, setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_HIDE_FILES), 0, P_HIDEFILES,  setup)  );                  

         bMenu->AddItem(cMenu);

         cMenu = new BMenu ( words->Return(L_BMENUITEM_MIDDLE_BUTTON) );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_SCROLLING),  1, P_SCROLLING,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_DRAGGING),   1, P_DRAGGING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_CLIPPING),   1, P_CLIPPING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_POPUP_MENU), 1, P_POPUPMENU,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_OPEN_NEW),   1, P_OPENNEW,    setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_FULL_SCREEN),1, P_FULLSCREEN, setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_HIDE_FILES), 1, P_HIDEFILES,  setup)  );                  
         bMenu->AddItem(cMenu);

         cMenu = new BMenu ( words->Return(L_BMENUITEM_RIGHT_BUTTON) );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_SCROLLING),  2, P_SCROLLING,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_DRAGGING),   2, P_DRAGGING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_CLIPPING),   2, P_CLIPPING,   setup)  );       
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_POPUP_MENU), 2, P_POPUPMENU,  setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_OPEN_NEW),   2, P_OPENNEW,    setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_FULL_SCREEN),2, P_FULLSCREEN, setup)  );
           cMenu->AddItem(  mouseHelper( words->Return(L_BMENUITEM_HIDE_FILES), 2, P_HIDEFILES,  setup)  );                  
         bMenu->AddItem(cMenu);

         cMenu = new BMenu ( words->Return(L_BMENUITEM_WHEEL_MOUSE) );
           cMenu->AddItem(  new BMenuItem(words->Return(L_BMENUITEM_WHEEL_MOUSE_SCROLL_FILE),  new BMessage(P_WHEEL_SCROLL_LIST) ) );
           cMenu->AddItem(  new BMenuItem(words->Return(L_BMENUITEM_WHEEL_MOUSE_SCROLL_IMAGE), new BMessage(P_WHEEL_SCROLL_IMAGE) ) );
         bMenu->AddItem(cMenu);



      tMenu->AddItem(bMenu);
     
         bMenu = new BMenu( words->Return( L_BMENUITEM_MOUSE_DRAGGING) );

         msg = new BMessage( PEEK_DRAG_MODE );  msg->AddInt32("action",B_COPY_TARGET);
         temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_COPY), msg );
         if (setup->DragAction() == B_COPY_TARGET) temp->SetMarked(true);
         bMenu->AddItem( temp );       

         msg = new BMessage( PEEK_DRAG_MODE );  msg->AddInt32("action",B_MOVE_TARGET);
         temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_MOVE), msg );
         if (setup->DragAction() == B_MOVE_TARGET) temp->SetMarked(true);
         bMenu->AddItem(  temp  );

         msg = new BMessage( PEEK_DRAG_MODE );  msg->AddInt32("action",B_LINK_TARGET);
         temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_LINK), msg );
         if (setup->DragAction() == B_LINK_TARGET) temp->SetMarked(true);
         bMenu->AddItem( temp  );

         msg = new BMessage( PEEK_DRAG_MODE );  msg->AddInt32("action",B_TRASH_TARGET);
         temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_TRASH), msg );
         if (setup->DragAction() == B_TRASH_TARGET) temp->SetMarked(true);         
         bMenu->AddItem( temp );

         msg = new BMessage( PEEK_DRAG_MODE );  msg->AddInt32("action",0);
         temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_NONE), msg );
         if (setup->DragAction()) temp->SetMarked(true);         
         bMenu->AddItem( temp );

      tMenu->AddItem(bMenu);

      tMenu->AddSeparatorItem();
      temp = new BMenuItem( words->Return(L_BMENUITEM_MOUSE_SWAP), new BMessage(PEEK_DRAG_SWITCH) );
      if (setup->DragMode()) temp->SetMarked(true);
      tMenu->AddItem( temp  );
     
   tempMenu->AddItem(tMenu);
 
       bMenu = new BMenu( words->Return(L_BMENUITEM_FILE_LIST) );
       
         temp = new BMenuItem( words->Return(L_BMENUITEM_EXEC_NON_IMAGE), new BMessage(PEEK_EXEC_NON) );
         if (setup->RunNonImageFiles()) temp->SetMarked(true);
         bMenu->AddItem( temp );
         temp = new BMenuItem( words->Return(L_BMENUITEM_TOGGLE_IMAGES_ONLY),  				new BMessage(PEEK_TOGGLE_IMAGES_ONLY) );
         if (setup->ImagesOnly()) temp->SetMarked(true);
         bMenu->AddItem( temp );
         temp = new BMenuItem( words->Return(L_BMENUITEM_FOLLOW_FILE),  				new BMessage(PEEK_FILE_LIST_FOLLOW) );
         if (setup->FollowFile()) temp->SetMarked(true);
         bMenu->AddItem( temp );

      tempMenu->AddItem( bMenu );

       bMenu = new BMenu( words->Return(L_BMENUITEM_MISC) );
       
// ********************************************************************************************
         temp = new BMenuItem( words->Return(L_BMENUITEM_ALLOW_FULLSCREEN_ON_START),  new BMessage(PEEK_ALLOW_FULLSCREEN_START) );
         if (setup->AllowFullscreenStart()) temp->SetMarked(true);
         bMenu->AddItem( temp );
       
      tempMenu->AddItem( bMenu );

 
      tempMenu->AddSeparatorItem();
      tempMenu->AddItem( new BMenuItem( words->Return(L_BMENUITEM_MAKE_DEFAULT), new BMessage(PEEK_MAKE_DEFAULT) )  );             
      tempMenu->AddSeparatorItem();
      tempMenu->AddItem( new BMenuItem( words->Return(L_BMENUITEM_SAVE_OPTIONS), new BMessage(PEEK_SAVE_OPTIONS) )  );       



  mainMenu->AddItem(tempMenu);

  // Image Menu

  tempMenu = new BMenu( words->Return(L_BMENU_IMAGE) );

      temp = new BMenuItem( words->Return(L_BMENUITEM_CROP_SELECTION),  new BMessage(PEEK_IMAGE_CROP_SELECTION), 'Y' );
      tempMenu->AddItem(temp );
 
      tempMenu->AddSeparatorItem();
 
      tMenu = new BMenu( words->Return(L_BMENUITEM_SET_DESKTOP) );

         bMenu = new BMenu ( words->Return(L_BMENUITEM_CURRENT) );
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_TILED),  new BMessage(PEEK_DESKTOP_TILE_ONE) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_CENTERED),  new BMessage(PEEK_DESKTOP_CENTER_ONE) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_SCALED),  new BMessage(PEEK_DESKTOP_SCALE_ONE) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_MANUAL),  new BMessage(PEEK_DESKTOP_MANUAL_ONE) )  );       

      tMenu->AddItem(bMenu);

         bMenu = new BMenu ( words->Return(L_BMENUITEM_ALL) );
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_TILED),  new BMessage(PEEK_DESKTOP_TILE_ALL) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_CENTERED),  new BMessage(PEEK_DESKTOP_CENTER_ALL) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_SCALED),  new BMessage(PEEK_DESKTOP_SCALE_ALL) )  );       
         bMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_MANUAL),  new BMessage(PEEK_DESKTOP_MANUAL_ALL) )  );       
 
      tMenu->AddItem(bMenu);
  tempMenu->AddItem(  tMenu  );

  tempMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_LOAD_DESKTOP),  new BMessage(PEEK_LOAD_DESKTOP) )  );

     tMenu = new BMenu( words->Return(L_BMENUITEM_SCREEN_CAPTURE) ); 
     tMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_WITHOUT_PEEK),  new BMessage(PEEK_SCREEN_CAPTURE_WITHOUT), 'M' )  );
     tMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_WITH_PEEK),  new BMessage(PEEK_SCREEN_CAPTURE_WITH), ';' )  );

  tempMenu->AddItem( tMenu );
   
  mainMenu->AddItem(tempMenu);


  tempMenu = new BMenu( words->Return(L_BMENU_VIEW) );    

     tMenu = new BMenu( words->Return( L_BMENUITEM_VIEWING_MODES) );

   

     temp = new BMenuItem( words->Return(L_BMENUITEM_NORMAL),  				new BMessage(PEEK_IMAGE_NORMAL) ,'1');
     if (setup->ViewingMode() == PEEK_IMAGE_NORMAL) temp->SetMarked(true);
     tMenu->AddItem( temp );
     temp = new BMenuItem( words->Return(L_BMENUITEM_TILED),  				new BMessage(PEEK_IMAGE_TILE) ,'2');
     if (setup->ViewingMode() == PEEK_IMAGE_TILE) temp->SetMarked(true);
     tMenu->AddItem( temp );
     temp = new BMenuItem( words->Return(L_BMENUITEM_STRETCHED_TO_WINDOW),  	new BMessage(PEEK_IMAGE_SCALE_TO_WINDOW) ,'3');
     if (setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW) temp->SetMarked(true);     
     tMenu->AddItem( temp );
     temp = new BMenuItem( words->Return(L_BMENUITEM_WINDOW_RESIZED),  		new BMessage(PEEK_IMAGE_WINDOW_TO_IMAGE) ,'4');
     if (setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE) temp->SetMarked(true);          
     tMenu->AddItem( temp );

     // scaled nicely addition
     temp = new BMenuItem( words->Return(L_BMENUITEM_SCALED_NICELY),  		new BMessage(PEEK_IMAGE_SCALED_NICELY) ,'5');
     if (setup->ViewingMode() == PEEK_IMAGE_SCALED_NICELY) temp->SetMarked(true);          
     tMenu->AddItem( temp );
       //      **********

     tempMenu->AddItem(tMenu);

     tempMenu->AddSeparatorItem();

     temp = new BMenuItem( words->Return(L_BMENUITEM_TOGGLE_FULL_SCREEN),  new BMessage(PEEK_TOGGLE_FULL_SCREEN) , 'F');
     if (setup->FullScreen()) temp->SetMarked(true);
     tempMenu->AddItem( temp );
     temp = new BMenuItem( words->Return(L_BMENUITEM_TOGGLE_FILE_LIST),  new BMessage(PEEK_TOGGLE_FILE_LIST) , 'H');
     if (setup->CompactMode() == false) temp->SetMarked(true);
     tempMenu->AddItem( temp );
     temp = new BMenuItem( words->Return(L_BMENUITEM_TOGGLE_SLIDE_SHOW),  new BMessage(PEEK_TOGGLE_SLIDE_SHOW) , 'S');
     if (setup->Sliding()) temp->SetMarked(true);
     tempMenu->AddItem( temp );


  mainMenu->AddItem(tempMenu);


  tempMenu = new BMenu( words->Return(L_BMENU_HELP) );
  tempMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_ABOUT),  new BMessage(B_ABOUT_REQUESTED) )  );
  tempMenu->AddItem(  new BMenuItem( words->Return(L_BMENUITEM_DOCUMENTATION),  new BMessage(PEEK_DOCUMENTATION) )  );

  mainMenu->AddItem(tempMenu);
  
  AddChild(mainMenu);
  // ---- END OF MENU BAR

  alles = new AllesView( BRect(Bounds().left, mainMenu->Bounds().bottom, Bounds().right, Bounds().bottom) , "alles" );


  float top = mainMenu->Bounds().Height() + 1;
        menubar_height  = mainMenu->Bounds().Height();
  /*
      // Adjusting for the button bar
        float button_bar_height = 50;
        top = top + button_bar_height;
      // ******* temporary BView for nice effect
        BView *temp_view = new BView( BRect(0,top - button_bar_height,Bounds().Width(),top - 1 ), "button_bar", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FRAME_EVENTS );
               temp_view->SetViewColor( 230,200,230 );
           alles->AddChild(temp_view);
      //            there we go *********
  */ 
  
  float right = setup->FileListRight();
  
  BRect *newFrame = new BRect( 0, top, right, Bounds().Height());
     filePane = new ViewFile( *newFrame , "fileFrame", "/boot/home" , setup, words);
  newFrame->Set( right + 12, top, Bounds().Width() - B_V_SCROLL_BAR_WIDTH - 1, Bounds().Height() - B_H_SCROLL_BAR_HEIGHT - 1 );
     imagePane = new PictureViewer("imagePane",  setup );

  // ok
  
  alles->SetViewColor(mainMenu -> ViewColor());
  filePane->SetViewColor(mainMenu -> ViewColor());
  
  
  imagePane->EmptyList();
  
  BScrollView * imageScroll = new BScrollView("imagePaneScrollView", imagePane, B_FRAME_EVENTS, true, true, B_NO_BORDER);
  
  //Creating a Layout:
  
  BGridLayout * allesLayout = new BGridLayout(0.0, B_USE_DEFAULT_SPACING);
  
  allesLayout -> AddView(filePane, 0, 0);
  allesLayout -> AddView(imageScroll, 1, 0, 5, 1);
  
  allesLayout -> SetColumnWeight(1, 5.0);
  
  alles -> SetLayout(allesLayout);
  
  alles->AddChild( filePane );
  alles->AddChild( imageScroll );
  
  AddChild( alles );
  alles->MakeFocus();
  imagePane->Refresh();

  // If we were in compact mode when shut down, go back into it
  if (setup->CompactMode())
  {
      setup->SetCompactMode(false);
      SetCompactMode(true);
  }

  // If we were in fullscreen mode when we were shut down and we're
  // allowed to start up in fullscreen mode, go back into it.
  if (setup->FullScreen())
  {
   setup->SetFullScreen(false);     
   if (setup->AllowFullscreenStart())  SetFullScreen(true);
  }

}

// yuppidee doo daa
bool WindowPeek::QuitRequested() {
  be_app->PostMessage(B_QUIT_REQUESTED);
  return true;
}

// -------------------------------------------------------------------------
//                      Loading functions for images
//       Use these to load images and nothing else. It makes it easier
//       to update huge chunks of code. thanks...
// -------------------------------------------------------------------------

// LoadImage(BEntry*) -- Takes a BEntry and makes a BList out of it's entry_ref
//                       and then sends it to LoadImages(BList);
//                       You MUST delete the BEntry yourself!
void WindowPeek::LoadImage(BEntry* e) {
    // Follow the file
   if (setup->FollowFile()) FollowFile(e, false);   
   // end follow files.
   Lock();
   imagePane->LoadEntry(e);

    // Set the window title to the file name
           char fileName[B_FILE_NAME_LENGTH];
           e->GetName(fileName);
           SetTitle( fileName );
    // done

   filePane->fileList->ScrollToSelection();
   Unlock(); 
}

// LoadImage(BBitmap*) -- Just sends it through to the PictureViewer right now.
//                        Hm... You can NOT delete rhe BBitmap when the method returns.
//                        Let's hope this works
void WindowPeek::LoadImage(BBitmap* b) {
   imagePane->LoadBitmap( b );
   SetTitle("unsaved data");
}


// --------------------------

void WindowPeek::FollowFile(BEntry* file, bool sel) {
  if (file == NULL) return;
  if (setup->Sliding()) return;
   
  BPath *bpath;
  
  if (file->IsDirectory()) {
     bpath = new BPath(file);
  } else {
    BEntry *temp = new BEntry();
    file->GetParent(temp);
    bpath = new BPath(temp);
    delete temp;
  }

  filePane->ChangeDirectory(bpath->Path());
  if (sel) filePane->Select(file);
  delete bpath;
}

// CenterWindow -- This CenterWindow sucks. Please fix it before
// anything happens

void WindowPeek::CenterWindow() {
   if (setup->FullScreen()) return;

   // calculate the center
   BScreen *sam = new BScreen();
   float left = sam->Frame().Width() / 2 - Frame().Width()/2;
   float top = sam->Frame().Height() / 2 - Frame().Height()/2;
   if (top < 30) top = 30;
   if (left < 5) left = 5;

   // go to it
   MoveTo(left,top);
   delete sam;
}


// Example:  IsType( my_entry, "image" ) == true
//           IsType( my_entry, "application" ) == false
//
bool WindowPeek::IsType(BEntry *file, char* str) {
       bool answer = false;
       BNode *myNode = new BNode(file);
       BNodeInfo *myNodeInfo = new BNodeInfo(myNode);
       
       char typem[B_MIME_TYPE_LENGTH];
       myNodeInfo->GetType(typem);

       BMimeType *total = new BMimeType(typem);
       BMimeType *superdude = new BMimeType();
       
       if (total->GetSupertype(superdude) == B_OK)
          if (strcmp(str,superdude->Type())==0) answer = true;
       
        // CD-ROM or read-only volume fix.
        //  Previously it would not load cd-rom files, and i presume, read-only files too.
       if (answer == false)
        {
           // first check if it's a read-only volume. If it isn't, then our first guess
           // or check was probably right and we should accept it.
            BPath full_path;
            file->GetPath(&full_path);
            BVolume *test_volume = new BVolume( dev_for_path( full_path.Path() ) );
            if (test_volume->IsReadOnly())     // make sure it's on a read-only volume
            {
              if (strcmp(str,"image") == 0)
               {
                // ok, it is. We need to do something here to check again.
                // SLOOWW here... It has to load each image to check if it is actually one! erg..
                BBitmap *test_bitmap;
                test_bitmap = BTranslationUtils::GetBitmapFile( full_path.Path() );
                if (test_bitmap != NULL)
                {
                  delete test_bitmap;
                  answer = true;
                }
               } // ok.
            }
            delete test_volume;
        }
        // end of fix
       
       delete superdude;
       delete total;
       delete myNodeInfo;
       delete myNode;
       return answer;  
}


// ------------------------

void WindowPeek::CreateLanguageMenu(BMenu* menu, uint32 bmsg) {
  if (menu == NULL) return;
  char dir[B_PATH_NAME_LENGTH];
  setup->RunningPath(dir);
  strcat(dir,"/languages/");
  
  BDirectory *langDir = new BDirectory(dir);
  if (langDir->InitCheck() != B_OK) return;
  langDir->Rewind();

  int32 number = langDir->CountEntries();

  for (int32 i = 0; i < number; i ++) {
     BMessage *msg = new BMessage(bmsg);
     BEntry *pos = new BEntry();
     langDir->GetNextEntry(pos);
     pos->GetName(dir);
     msg->AddString("filename",dir);
     menu->AddItem(  new BMenuItem( dir,  msg) );            
     delete pos;
  }
  
  delete langDir;

}

void WindowPeek::CaptureScreen(bool h) {
  Lock();
  if (!h) Hide();
   BScreen desk;
   BBitmap *screenDump;
   UpdateIfNeeded();
   snooze(1000000);  // safety snooze to ensure the screen is all refreshed.  Perhaps is should be larger to account for slower systems?
   desk.GetBitmap(&screenDump);
   LoadImage(screenDump);  
  if (!h) Show();
  Unlock();
}


BEntry* WindowPeek::GetDesktopBackground() {
 BEntry *answer = NULL;

 BMessage msg;
 BPath *desktopPath = new BPath();
 find_directory(B_DESKTOP_DIRECTORY,desktopPath);
 BDirectory *desktopDirectory = new BDirectory(desktopPath->Path());

 if (desktopDirectory->InitCheck() == B_OK) {
   attr_info d_info;
   if (desktopDirectory->GetAttrInfo(B_BACKGROUND_INFO,&d_info) == B_OK) {

        char buffer[d_info.size];
        desktopDirectory->ReadAttr(B_BACKGROUND_INFO,d_info.type,0,buffer,d_info.size);

        if (msg.Unflatten(buffer) == B_OK) {
          long int workspaceNum = 0;     
           while ( (( msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum)) & Workspaces()) != Workspaces()  &&
                   ((uint32) msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum)) != 0
                   ) workspaceNum++;
            if ( (uint32)msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum) != 0) answer = new BEntry(msg.FindString(B_BACKGROUND_IMAGE,workspaceNum));
        }
   }
 } 

 delete desktopPath;
 delete desktopDirectory;

 return answer;
}

#include <Roster.h>

void WindowPeek::SetDesktopBackground(int32 mode,bool all) {
 BEntry *file = imagePane->CurrentFile();  // NB!!!! DON"T DELETE file AGAIN. NOT OURS
 if (file == NULL) return;

 BMessage msg,reply, wakeup;

 BPath *desktopPath = new BPath();
 find_directory(B_DESKTOP_DIRECTORY,desktopPath);
 BDirectory *desktopDirectory = new BDirectory(desktopPath->Path());

 if (desktopDirectory->InitCheck() == B_OK) {
   attr_info d_info;                             
   if (desktopDirectory->GetAttrInfo(B_BACKGROUND_INFO,&d_info) == B_OK) {
       // gather our info together
        char buffer[d_info.size];       
        if (all == false) {
             desktopDirectory->ReadAttr(B_BACKGROUND_INFO,d_info.type,0,buffer,d_info.size);
             msg.Unflatten(buffer);
        }      


       char fileName[B_FILE_NAME_LENGTH];
       BPath tempPath;
       file->GetPath(&tempPath);
       strcpy(fileName,tempPath.Path());
       int32 places = B_ALL_WORKSPACES;
       if (!all) places = Workspaces();
       // convert
       
       // first remove existing information

       long int workspaceNum = 0;
       int32 dummy = 0;

       uint32 sam;
 
       sam  =  msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum,&dummy);
       while ( 
               ( msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum,&dummy) == B_OK)  &&
               ((dummy & places) != places )
             ) workspaceNum++;

        if (msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum,&dummy) == B_OK) {
            // found it!  there's a desktop thingy already. remove it. It's at workspaceNum
           if (dummy == places ) {
             msg.ReplaceString( B_BACKGROUND_IMAGE, workspaceNum, fileName);
             msg.ReplaceInt32( B_BACKGROUND_WORKSPACES, workspaceNum, places);
             msg.ReplaceInt32( B_BACKGROUND_MODE, workspaceNum, mode);
             msg.ReplacePoint( B_BACKGROUND_ORIGIN, workspaceNum, BPoint(0,0));
             msg.ReplaceBool( B_BACKGROUND_ERASE_TEXT, workspaceNum, true);
           } else {
             msg.ReplaceInt32( B_BACKGROUND_WORKSPACES, workspaceNum, msg.FindInt32(B_BACKGROUND_WORKSPACES,workspaceNum) - places);

             msg.AddString( B_BACKGROUND_IMAGE, fileName);
             msg.AddInt32( B_BACKGROUND_WORKSPACES, places);
             msg.AddInt32( B_BACKGROUND_MODE, mode);
             msg.AddPoint( B_BACKGROUND_ORIGIN, BPoint(0,0));
             msg.AddBool( B_BACKGROUND_ERASE_TEXT, true);

           }
              
         } else {
           // okay, add the info. Nothing there.
             msg.AddString( B_BACKGROUND_IMAGE, fileName);
             msg.AddInt32( B_BACKGROUND_WORKSPACES, places);
             msg.AddInt32( B_BACKGROUND_MODE, mode);
             msg.AddPoint( B_BACKGROUND_ORIGIN, BPoint(0,0));
             msg.AddBool( B_BACKGROUND_ERASE_TEXT, true);
         }

        char buffer2[msg.FlattenedSize()];
        msg.Flatten(buffer2, msg.FlattenedSize());
        desktopDirectory->WriteAttr(B_BACKGROUND_INFO,d_info.type,0,buffer2,msg.FlattenedSize());
    }
 }

 delete desktopPath;
 delete desktopDirectory;

 // wake up tracker
 wakeup.what = B_RESTORE_BACKGROUND_IMAGE;
 BMessenger("application/x-vnd.Be-TRAK").SendMessage(&wakeup,&reply);

 if (mode == B_BACKGROUND_MODE_USE_ORIGIN) be_roster->Launch("application/x-vnd.Be-BACK");
}


void WindowPeek::FrameMoved(BPoint a) {
  if (setup->FullScreen() == false) setup->SetWindowFrame( BRect(a.x, a.y, setup->WindowFrame().Width() + a.x, a.y + setup->WindowFrame().Height()));
}

void WindowPeek::FrameResized(float x, float y) {
   if (setup->FullScreen() == false) setup->SetWindowFrame(Frame());
}


#include <Alert.h>

int32 WindowPeek::TotalImageFiles() {
   return filePane->totalImages();
}
#include <stdio.h>

void WindowPeek::ExecuteProgram( int32 num , bool check = false) {
   if (filePane->Selected() < 0) return;
   if (check)  if (setup->RunNonImageFiles() == false) return;
   BEntry *target = filePane->EntryAt( filePane->Selected() );

   if (check) if (IsType(target,"image")) {
                   delete target;
                   return;
                  }

   entry_ref temp;
   target->GetRef(&temp);
   be_roster->Launch(&temp);
   delete target;
}

void WindowPeek::MakeDefault() {
  if ( 
  (
  new BAlert("check", 
               words->Return(L_CHECK_DEFAULT),
               words->Return(L_YES), 
               words->Return(L_NO) ) )
          ->Go()  
   
   != 0 ) return;


    translator_id *list;
    int32 num;
    BTranslatorRoster::Default()->GetAllTranslators(&list,&num);
 
      for (int i=0; i < num; i++) {
         int32 numOfTypes;
         const translation_format *in_types,*out_types;            // WE'RE LOOKING FOR ALL TYPES THAT CAN CONVERT TO BBITMAP
         BTranslatorRoster::Default()->GetOutputFormats(list[i],&in_types,&numOfTypes);
         
         for (int j = 0; j <numOfTypes; j++) {
          if (in_types[j].type == B_TRANSLATOR_BITMAP) {
             BTranslatorRoster::Default()->GetInputFormats(list[i],&out_types,&numOfTypes);
              for (int po = 0; po < numOfTypes; po ++) 
                if (out_types[po].type != B_TRANSLATOR_BITMAP) {
                       BMimeType *bob = new BMimeType(out_types[po].MIME);
                       bob->SetPreferredApp("application/x-vnd.Peek",B_OPEN);
                       delete bob;
                 }   
              break;
          }
         }
      }

}

// This will move the currently selected file to the user's Trash
// directory.  WARNING!!  This will clobber any files already in
// the trash directory that have the same name as the doomed file
//
void WindowPeek::DeleteFile() {
  BEntry *doomed = imagePane->CurrentFile();
  if (doomed == NULL) return;

  // did it not work for some reason?
  if (doomed->InitCheck() != B_OK) 
  {
    delete doomed; 
    return;
  }

  
  BPath *trashPath = new BPath();   // to hold the trash path
 
  if (find_directory(B_TRASH_DIRECTORY,trashPath) == B_OK) 
  {
     BDirectory *trashDirectory = new BDirectory(trashPath->Path());
     doomed->MoveTo(trashDirectory, NULL, true);    // move it with a clobber 
     delete trashDirectory;
     imagePane->EmptyList();
  }   
  
  delete trashPath;
}

// FileSave saves the actual image in memory.
void WindowPeek::FileSave(BMessage *msg) {
  int32 tid, type;
  translator_id trans_id;
  void *sourceData;
  const char *fileName;
  const char *mime_type;

  entry_ref *saveDirectory = new entry_ref;
  msg->FindInt32("translator_id",&tid);
  msg->FindInt32("type_const",&type);
  msg->FindString("mime_type",&mime_type);
  msg->FindPointer("bitmap_pointer", &sourceData);
  msg->FindString("name",&fileName);
  msg->FindRef("directory",saveDirectory);

  BBitmap *thePic = (BBitmap*)sourceData;
  trans_id = (translator_id)tid;
  uint32 type_const = (uint32)type;

  BBitmapStream *strm = new BBitmapStream(thePic);
  BEntry target(saveDirectory,true);
  target.GetRef(saveDirectory);
 
  BDirectory *targetDir = new BDirectory(saveDirectory);
  if (targetDir->InitCheck() == B_OK) {
     BFile *targetFile = new BFile(targetDir,fileName, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
     if (targetFile->InitCheck() == B_OK) {
       BTranslatorRoster::Default()->Translate(trans_id,strm,NULL,targetFile,type_const);
       BNodeInfo *ni = new BNodeInfo(targetFile);
       ni->SetType(mime_type);
       delete ni;
       delete targetFile;
         BEntry *newFile = new BEntry;      // fixes the not recognizing a save problem
         if (targetDir->FindEntry(fileName,newFile) == B_OK) {
           Lock();
           LoadImage(newFile); 
           Unlock();
          } 
         delete newFile;
     }  
  }
  strm->DetachBitmap(&thePic);  

  delete saveDirectory;
  delete strm;
  delete targetDir;
  delete saveWindow;
}


void WindowPeek::SaveFile(BMessage *msg) {
  if (imagePane->Bitmap()==NULL) return;
  BMessage *msg2 = new BMessage(*msg);
  msg2->what = PEEK_SAVE_FILE;
  msg2->AddPointer("bitmap_pointer", imagePane->Bitmap());
  saveWindow = new BFilePanel(B_SAVE_PANEL);
  saveWindow->SetMessage(msg2);
  saveWindow->Show();
}  

// SetSliding --  if value is true, is till turn on the slideshow
//                        if false, it will turn it off...
void WindowPeek::SetSliding( bool value ) {
   if (setup->Sliding() == value) return;

   if (value) {
               if (filePane->TurnSlideshowOn() == B_OK)
                     setup->SetSliding(true);
              }
   else {
          if (filePane->TurnSlideshowOff() == B_OK)
                  setup->SetSliding(false);
        }                      
}

// SetFullScreen -- if true, it will go full screen
//                  if false, it will go normal screen
void WindowPeek::SetFullScreen( bool value ) {
   if ( setup->FullScreen() == value ) return;

   if (value == false )  {
                          setup->SetFullScreen(false);
                          SetFlags(0);
                          MoveTo( setup->WindowFrame().left, setup->WindowFrame().top );
                          ResizeTo(setup->WindowFrame().Width(),setup->WindowFrame().Height());
                          filePane->ResizeBy(0, B_H_SCROLL_BAR_HEIGHT );
                        }
   else {
           setup->SetFullScreen(true);
           SetFlags(B_NOT_RESIZABLE);
           BScreen *screen = new BScreen();
           MoveTo(0,-menubar_height - 1);
           ResizeTo(screen->Frame().Width() + B_V_SCROLL_BAR_WIDTH + 1,screen->Frame().Height() + B_H_SCROLL_BAR_HEIGHT + 1 + menubar_height + 1);
           filePane->ResizeBy(0, - B_H_SCROLL_BAR_HEIGHT );
           delete screen;
        }
   
}

// SetCompactMode -- if true, will hide the file list and extend the image view everywhere
//                   if false, it does the opposite
void WindowPeek::SetCompactMode( bool value ) {
   if ( setup->CompactMode() == value ) return;
   
   if (value == true) {
                        setup->SetCompactMode(true);
                        filePane->Hide();
                        imagePane->Parent()->MoveTo( -1 ,  imagePane->Parent()->Frame().top );
                        imagePane->Parent()->ResizeTo( Bounds().Width() + 1, Bounds().Height() - imagePane->Parent()->Frame().top );
                      }
   else {
           setup->SetCompactMode(false);
           filePane->Show();
           imagePane->Parent()->MoveTo( setup->FileListRight() + 12,  imagePane->Parent()->Frame().top );
           imagePane->Parent()->ResizeTo( Bounds().Width() -  setup->FileListRight() - 12, Bounds().Height() - imagePane->Parent()->Frame().top );
        }
}
    


// DispatchMessage -- Slightly overided method to handle stuff-ola
//
//
void WindowPeek::DispatchMessage(BMessage *e, BHandler *h) {
  switch (e->what)  {
     case B_KEY_UP:
     case B_MOUSE_WHEEL_CHANGED:
     
                      alles->MessageReceived(e);
                      return;
     default: BWindow::DispatchMessage(e, h);
  }
}



void WindowPeek::MessageReceived(BMessage* e) {
  switch (e->what) {
    case B_NODE_MONITOR: filePane->BuildListing(); break;
    case P_WHEEL_SCROLL_IMAGE: setup->SetWheelMouseAction( P_WHEEL_SCROLL_IMAGE ); break;
    case P_WHEEL_SCROLL_LIST:  setup->SetWheelMouseAction( P_WHEEL_SCROLL_LIST  ); break;
    case PEEK_FILE_INVOKED: {   
                               if (filePane->Selected() < 0) return;
                               BEntry *sam = filePane->EntryAt( filePane->Selected() );
                               if (sam->IsDirectory()) {
                                       if (setup->Sliding()) SetSliding(! setup->Sliding() );
                                       filePane->Notified();
                                     } 
                                 else 
                                 if (setup->RunNonImageFiles()) ExecuteProgram( filePane->Selected(), true ); 
                               delete sam;
                               break;
                             }  
    case PEEK_DELETE_FILE: DeleteFile(); break;
    case PEEK_FILE_SELECTED: {
                                if (filePane->Selected() < 0) break;
                                   if (setup->Sliding()) {
                                         BEntry *sam = filePane->EntryAt( filePane->Selected() );
                                         if (IsType(sam,"image")) LoadImage(sam);
                                         delete sam;
                                   }
                                 break;
                             }
    case PEEK_SLIDESHOW_ASCENDING:  
    case PEEK_SLIDESHOW_DESCENDING:
    case PEEK_SLIDESHOW_RANDOM:
                                    setup->SetSlideshowMode( e->what ); 
                                    MenuTick( e, true , true);
                                    break;
    case PEEK_FILE_LIST_FOLLOW:     
                                    setup->SetFollowFile( !setup->FollowFile() );
                                    MenuTick(e,true,false);
                                    break;
    case PEEK_FILE_LIST_MEMORY:   
                                    setup->SetRememberPath(  ! setup->RememberPath() ); 
                                    MenuTick( e, true , false);                                    
                                    break;
                                  
    case PEEK_SLIDESHOW_DELAY: {
                                  int32 delay = e->FindInt32("delay");
                                  setup->SetSlideshowDelay(delay * 1000000);
                                  MenuTick( e, true , true);
                                  break;
                                }


    // desktop stuff
    case PEEK_LOAD_DESKTOP:  {  
                                BEntry *background = GetDesktopBackground();
                                if (background != NULL) { 
                                       LoadImage(background); 
                                }
                                break;
                             }  
    case PEEK_DESKTOP_TILE_ALL:    SetDesktopBackground(B_BACKGROUND_MODE_TILED, true); break;
    case PEEK_DESKTOP_SCALE_ALL:   SetDesktopBackground(B_BACKGROUND_MODE_SCALED, true); break;
    case PEEK_DESKTOP_CENTER_ALL:  SetDesktopBackground(B_BACKGROUND_MODE_CENTERED, true); break;
    case PEEK_DESKTOP_MANUAL_ALL:  SetDesktopBackground(B_BACKGROUND_MODE_USE_ORIGIN, true); break;

    case PEEK_DESKTOP_TILE_ONE:    SetDesktopBackground(B_BACKGROUND_MODE_TILED, false); break;
    case PEEK_DESKTOP_SCALE_ONE:   SetDesktopBackground(B_BACKGROUND_MODE_SCALED, false); break;
    case PEEK_DESKTOP_CENTER_ONE:  SetDesktopBackground(B_BACKGROUND_MODE_CENTERED, false); break;
    case PEEK_DESKTOP_MANUAL_ONE:  SetDesktopBackground(B_BACKGROUND_MODE_USE_ORIGIN, false); break;

    // viewing modes

    case PEEK_IMAGE_NORMAL:
    case PEEK_IMAGE_WINDOW_TO_IMAGE:
    case PEEK_IMAGE_SCALE_TO_WINDOW:
    case PEEK_IMAGE_SCALED_NICELY:
    case PEEK_IMAGE_TILE:    {
                               setup->SetViewingMode( e->what );
                               imagePane->ClearClipping();  
                               if (e->what == PEEK_IMAGE_WINDOW_TO_IMAGE)
                                   if (setup->FullScreen() == false ) 
                                             imagePane->ResizeToImage();
                               imagePane->Refresh(); 
                               MenuTick( e, true , true);
                               break;
                             } 

   
     // things
     case PEEK_SCREEN_CAPTURE_WITHOUT:  CaptureScreen(false); break;
     case PEEK_SCREEN_CAPTURE_WITH:  CaptureScreen(true); break;

     // other things
     case PEEK_TOGGLE_FILE_LIST:   {    
                                        SetCompactMode( ! setup->CompactMode() );
                                        MenuTick( e, setup->CompactMode() , false);
                                        break;
                                    }    
     case PEEK_TOGGLE_FULL_SCREEN:   {   
                                        SetFullScreen( ! setup->FullScreen() );
                                        MenuTick( e, setup->FullScreen() , false);
                                        break; 
                                      }  
     case PEEK_TOGGLE_SLIDE_SHOW:	{
                                       SetSliding( ! setup->Sliding() ); 
                                       MenuTick( e, setup->Sliding() , false);
                                       break;
                                     }  
     case PEEK_TOGGLE_IMAGES_ONLY:	{  
                                      setup->SetImagesOnly(! setup->ImagesOnly()); 
                                      filePane->BuildListing(); 
                                      MenuTick( e, setup->ImagesOnly() , false);
                                      break;
                                    }  

     case PEEK_MOUSE_CHANGE:	{
     							 int32 button = e->FindInt32("button");
     							 int32 action = e->FindInt32("action");
     							 if (button == 0) setup->SetLeftMouseAction(action);
     							 if (button == 1) setup->SetMiddleMouseAction(action);
      							 if (button == 2) setup->SetRightMouseAction(action);
                                 MenuTick( e, true , true);
    							 break;
     							}
    case PEEK_DRAG_MODE:	{
                                int32 action = e->FindInt32("action");
                                setup->SetDragAction(action);
                                MenuTick( e, true , true);
                                break;
    						}
    case PEEK_DRAG_SWITCH:	{   
                                setup->SetDragMode(! setup->DragMode() );
                                MenuTick( e, setup->DragMode() , false);
                                break;
    						}
    case PEEK_ALLOW_FULLSCREEN_START: {
                                         setup->SetAllowFullscreenStart(! setup->AllowFullscreenStart() );
                                         MenuTick( e, setup->AllowFullscreenStart() , false);
                                         break;
                                      }
    case PEEK_SAVE_AS:	{   SaveFile(e);
							break;
    					}
    case PEEK_SAVE_FILE:	{   FileSave(e);
     							break;
           					}
    case PEEK_SAVE_OPTIONS:	{  ((PeekApp*)be_app)->SaveSetup(); break; }
    case PEEK_MAKE_DEFAULT: MakeDefault(); break;
    case PEEK_EXEC_NON:	{
						  setup->SetRunNonImageFiles( ! setup->RunNonImageFiles()) ;
                          MenuTick( e, setup->RunNonImageFiles() , false);
						  break;
    					}

    case PEEK_LOAD_INTO_CENTER:	{
						  setup->SetSlideLoadIntoCenter( ! setup->SlideLoadIntoCenter()) ;
                          MenuTick( e, setup->SlideLoadIntoCenter() , false);
						  break;
    					}
    case PEEK_IMAGE_CROP_SELECTION: {
                          imagePane->CropToSelection();
                        }

    case PEEK_LANGUAGE_CHANGE:
    case B_ABOUT_REQUESTED:    
    case PEEK_DOCUMENTATION:   be_app->PostMessage(e);  break;
    default: BWindow::MessageReceived(e); break;
  }
}

// MenuTick -- basicall ticks the thingy in the menu
//             if clean == true, the rest of the things in the menu list
//             are set to !ticked.
void WindowPeek::MenuTick( BMessage* e, bool tick, bool clean) {
                          void *bob = NULL;
                          e->FindPointer("source", &bob);
                          ((BMenuItem*)(bob))->SetMarked(tick);
                          BMenuItem *target = ((BMenuItem*)(bob));

                          if (clean) {
                            BMenu *mapa = ((BMenuItem*)(bob))->Menu();
                            
                            int32 num = mapa->CountItems();
                            
                             for (int i = 0; i < num; i++) {
                                if (mapa->ItemAt(i) != target ) {
                                  mapa->ItemAt(i) -> SetMarked(false);   
                                }
                             } 
                          }

}
