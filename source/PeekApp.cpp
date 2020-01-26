#include "PeekApp.h"
#include <string.h>
#include <Roster.h>
#include <StorageKit.h>
#include <Alert.h>
#include <List.h>
#include <image.h>
#include <private/interface/AboutWindow.h>

PeekApp::PeekApp() : BApplication("application/x-vnd.Peek") {
  // set up the Setup object
  setup = new Setup();
  if (LoadSetup()) 
  {
   SetDefaults();
   setup->SaveSettings();
  }

  setup->SetSliding(false);       // We shouldn't start up sliding, so fix this just in case.


    // I removed the "Remember Path" option. it's silly. Now it starts up in the home directory
  BPath homePath;
  if (find_directory(B_USER_DIRECTORY,&homePath) == B_OK) setup->SetCurrentPath( homePath.Path() );

   // Get the running path of Peek and store it in Setup
    app_info appInfo;
    be_app->GetAppInfo(&appInfo);

    BPath *tempPath = new BPath(&appInfo.ref);
    tempPath->GetParent(tempPath);
    char runningPath[B_PATH_NAME_LENGTH];
    strcpy(runningPath,tempPath->Path());
    delete tempPath;
    setup->SetRunningPath(runningPath);
  // setup is now updated with info


  // load language
  words = new Language();
  LoadLanguage();

  mainWindow = new WindowPeek(setup->WindowFrame(), "Peek", setup, words);
  
  // The same size as the default WindowFrame.
  mainWindow->SetSizeLimits(600, B_SIZE_UNLIMITED, 400, B_SIZE_UNLIMITED);
}


void PeekApp::LoadLanguage() {
  char fileName[B_FILE_NAME_LENGTH];
  char temp[B_FILE_NAME_LENGTH];
  setup->DefaultLanguage(temp);
  setup->RunningPath(fileName);
  strcat(fileName,"/languages/");
  strcat(fileName,temp);
  if (words->Load(fileName) != B_OK) {
    (new BAlert("no default","Default language file not found (or corrupt)! trying for english.","pour for vour?"))->Go();
     setup->RunningPath(fileName);
     strcat(fileName,"/languages/english");
      if (words->Load(fileName) != B_OK) {
        (new BAlert("no language","English not found  (or corrupt). Your Peek needs the file languages/english to run. Try renaming one of the other language files to 'english' or download a full copy of Peek. ","no!"))->Go(); 
        be_app->PostMessage(B_QUIT_REQUESTED);
      }
   }  
}


void PeekApp::Quit() 
{
  SaveSetup();
  BApplication::Quit();
}

void PeekApp::ReadyToRun() 
{
   mainWindow->Show();
}


// RefsReceived -- This is called when files are dropped onto Peek or Peek
//                 is loaded via a double-click on image files in Tracker
//
//                 If (IsLaunching() == true) then it's the double-click case.
//                 We only accept 1 file via this method.
void PeekApp::RefsReceived(BMessage* e) 
{
  entry_ref file;
  if (e->FindRef("refs",&file) != B_OK) return;

  if ( IsLaunching() )  mainWindow->MoveTo(10,50);
  //SetCompactMode( true );

  BEntry *dada = new BEntry( &file, true );
  if ( mainWindow->IsType(dada, "image") )  mainWindow->LoadImage( dada );
  delete dada;
}



void PeekApp::MessageReceived(BMessage* e) {
  switch (e->what) {
    case PEEK_SAVE_FILE:	{  mainWindow->PostMessage(e); 
                               break;
                             }
    case PEEK_LANGUAGE_CHANGE:  {
                                  mainWindow->Quit();
                                  char lang[B_FILE_NAME_LENGTH];
                                  strcpy(lang, e->FindString("filename"));
                                  setup->SetDefaultLanguage(lang);
                                  LoadLanguage();
                                  mainWindow = new WindowPeek( setup->WindowFrame(), "Peek", setup, words);
                                  mainWindow->Show();
                                  break;
                                }
    case PEEK_DOCUMENTATION:    ShowHTML();  break;
    case B_NODE_MONITOR:        mainWindow->PostMessage(B_NODE_MONITOR); break;
    default: BApplication::MessageReceived(e); break;
  }
}

void PeekApp::AboutRequested() {
  BAboutWindow* about = new BAboutWindow(PeekVersion, "application/x-vnd.Peek"); 
  about->AddText(words->Return(L_BALERT_ABOUT)); 
  about->Show();
}

void PeekApp::ShowHTML() {
  char fileName[B_PATH_NAME_LENGTH];
  setup->RunningPath(fileName);
  strcat(fileName,"/html/index.html");

  BEntry *htmlFile = new BEntry(fileName);
  if (htmlFile->InitCheck() != B_OK) {
    delete htmlFile;
    return;
  }  
  entry_ref htmlRef;
  htmlFile->GetRef(&htmlRef);
  
  status_t success = be_roster->Launch(&htmlRef);
  if ( success != B_OK && success != B_ALREADY_RUNNING ) 
    (new BAlert("missingAlert", words->Return(L_BALERT_MISSING_HTML), words->Return(L_OK)))->Go();

  delete htmlFile;
}

BMessage* PeekApp::SetupNegotiationMessage(int32 action) {
  // setting up our negotiation message for any other applications.
  BMessage *msg = new BMessage(B_SIMPLE_DATA);
  // see ***************************************

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
                   // **************************************************************
                   // Set up the external negotiation message
                   msg->AddString("be:types",out_types[po].MIME);
                   msg->AddString("be:filetypes",out_types[po].MIME);
                   msg->AddString("be:type_descriptions",out_types[po].name); 
                 }   
              break;
          }
         }
      }


    msg->AddString("be:types",B_FILE_MIME_TYPE);

    if (action == 0) {
       msg->AddInt32("be:actions",B_COPY_TARGET);
       msg->AddInt32("be:actions",B_MOVE_TARGET);
       msg->AddInt32("be:actions",B_TRASH_TARGET);
    } else {
       msg->AddInt32("be:actions",action);
    }


    msg->AddString("be:clip_name","Peek clipping");
    // **************** negotiation message set up now. 
    return msg;
}

// Everyone is going to start off with these setttings, hopefully.
//
void PeekApp::SetDefaults() 
{
   setup->SetVersion(PeekVersion);
   setup->SetDefaultLanguage("english");
   setup->SetWindowFrame(BRect(10,50,610,450));   //  (0,0) -> (600,400) => each (200,400)
   setup->SetFileListRight( 200 );
   setup->SetViewingMode(PEEK_IMAGE_NORMAL);
   setup->SetFollowFile(true);
   setup->SetImagesOnly(false);

   setup->SetSlideLoadIntoCenter(true);
   setup->SetRunNonImageFiles(true);
   setup->SetFullScreen(false);
   setup->SetSliding(false);
   setup->SetSlideshowDelay(4000000);
   setup->SetSlideshowMode(PEEK_SLIDESHOW_DESCENDING);
   
   setup->SetRightMouseAction(P_DRAGGING);
   setup->SetMiddleMouseAction(P_HIDEFILES);
   setup->SetLeftMouseAction(P_SCROLLING);
   setup->SetWheelMouseAction(P_WHEEL_SCROLL_LIST);
   
   setup->SetDragAction(0);
   setup->SetCompactMode(false);
   setup->SetDragMode(true);
   setup->SetRememberPath(false);
   setup->SetAllowFullscreenStart(false);

   BMessage *msg = SetupNegotiationMessage(0);
   setup->SetNegotiationMessage(msg);
}

void PeekApp::SaveSetup() 
{
  setup->SaveSettings();
}

bool PeekApp::LoadSetup() 
{
  // Make sure we're not running right after an older version and
  // we have the old format hanging around.
 
       BPath settings_path;
       find_directory(B_USER_SETTINGS_DIRECTORY, &settings_path);
       settings_path.Append("Peek_settings");
    
       // Make sure it exists and then check validitiy
       BFile *test_file  = new BFile( settings_path.Path() , B_READ_ONLY );

       if ( test_file->InitCheck() == B_OK )
       {    // ok, it exists. Must check for the first two chars to be 'VE' as in 'VERSION'
          char k1 = ' ';
          char k2 = ' ';
          test_file->Read(&k1,1);
          test_file->Read(&k2,1);
          if ( (k1 != 'V') && (k2 != 'E') )
          {
            // Tell the user what we're going to do.
             (new BAlert("new_version","An older version of Peek was installed at some time. I've found the settings file and I'm going to replace it with the newer version. If you want to go back to an older version of Peek, please delete the new settings file in your ~/config/settings directory.  You won't see this message again. :-)","okay"))->Go();
        
                   // ok, it's an old configuration
             BEntry *old_settings_file = new BEntry( settings_path.Path() );
             old_settings_file->Remove();    // remove it
             delete old_settings_file;       // delete it.
            return true;                     // return true so that defaults are set.
          }
       }

      delete test_file;
  

  // Ok, it seems to be valid. Carry on.

  if (setup->LoadSettings() != B_OK) 
  {
    SetDefaults();
    setup->SaveSettings();
  }
  return false;
}
