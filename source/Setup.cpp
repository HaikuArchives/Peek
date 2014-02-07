#include "Setup.h"
#include "constants.h"
#include "ConfigureClass.h"
#include <be/support/String.h>

// copies & sets the default language filename into/from target.
void Setup::DefaultLanguage(char* target) { strcpy(target,defaultLanguage); }
void Setup::SetDefaultLanguage(char* target) { strcpy(defaultLanguage,target); }

// copies & sets Peek's current directory into/from target.
void Setup::RunningPath(char* target) { strcpy(target,runningPath); }
void Setup::SetRunningPath(char* target) { strcpy(runningPath,target); }

// copies & sets Peek's current directory into/from target.
void Setup::CurrentPath(char* target) { strcpy(target,currentPath); }
void Setup::SetCurrentPath(const char* target) { strcpy(currentPath,target); }


// copies & sets Peek's version into/from target.
void Setup::Version(char* target) { strcpy(target,version); }
void Setup::SetVersion(char* target) { strcpy(version,target); }

// follow file
bool Setup::FollowFile() { return followFile; }
void Setup::SetFollowFile(bool a) { followFile = a; }

// follow file
bool Setup::FullScreen() { return fullScreen; }
void Setup::SetFullScreen(bool a) { fullScreen = a; }

// follow file
bool Setup::RememberPath() { return rememberPath; }
void Setup::SetRememberPath(bool a) { rememberPath = a; }

// sliding
bool Setup::Sliding() { return sliding; }
void Setup::SetSliding(bool a) { sliding = a; }

// sliding delay
unsigned long Setup::SlideshowDelay() { return slideshowDelay; }
void Setup::SetSlideshowDelay(unsigned long a ) { slideshowDelay = a; }


// run non-image files
bool Setup::RunNonImageFiles() { return runNonImageFiles; }
void Setup::SetRunNonImageFiles(bool a) { runNonImageFiles = a; }


// images only
bool Setup::ImagesOnly() { return imagesOnly; }
void Setup::SetImagesOnly(bool a) { imagesOnly = a; }

// center load
bool Setup::SlideLoadIntoCenter() { return slideCenterLoad; }
void Setup::SetSlideLoadIntoCenter(bool a) { slideCenterLoad = a; }


// window Frame
BRect Setup::WindowFrame() { return windowFrame;}
void  Setup::SetWindowFrame(BRect R) { windowFrame = R;}

// file Frame
float Setup::FileListRight() { return fileListRight;}
void  Setup::SetFileListRight(float R) { fileListRight = R;}

// viewing Mode
int32 Setup::ViewingMode() { return viewingMode;}
void  Setup::SetViewingMode(int32 n) { viewingMode = n;}

// slideshow
int32 Setup::SlideshowMode() { return slideshowMode;}
void  Setup::SetSlideshowMode(int32 n) { slideshowMode = n;}

// thumbnail Mode
bool Setup::DragMode() { return dragMode;}
void  Setup::SetDragMode(bool a) { dragMode = a;}

// thumbnail Mode
bool Setup::CompactMode() { return compactMode;}
void  Setup::SetCompactMode(bool a) { compactMode = a;}

// negotiation Message
BMessage* Setup::NegotiationMessage() { return negotiationMessage;}
void  Setup::SetNegotiationMessage(BMessage* n) { negotiationMessage = n;}

// mouse mapping
long Setup::RightMouseAction() { return rightMouseAction; }
long Setup::MiddleMouseAction() { return middleMouseAction; }
long Setup::LeftMouseAction() { return leftMouseAction; }
long Setup::WheelMouseAction() { return wheelMouseAction; }

void Setup::SetRightMouseAction(long a) { rightMouseAction = a; }
void Setup::SetMiddleMouseAction(long a) { middleMouseAction = a; }
void Setup::SetLeftMouseAction(long a) { leftMouseAction = a; }
void Setup::SetWheelMouseAction(long a) { wheelMouseAction = a; }

// dragging
long Setup::DragAction() { return dragAction; }
void Setup::SetDragAction(long a) { dragAction = a; }

// fullscreen start
bool Setup::AllowFullscreenStart() { return fullscreenStart; }
void Setup::SetAllowFullscreenStart(bool a) { fullscreenStart = a; }




/* 

  THIS IS WHERE ARE ARE GOING TO SAVE OUR SETTINGS USING A SIMPLE
  ConfigureClass (by me, hooray)

*/

void Setup::SaveSettings()
{
  ConfigureClass *myConfig = new ConfigureClass("Peek_settings");

  myConfig->Set( "VERSION",  version);
  myConfig->Set( "LANGUAGE",  defaultLanguage );
  myConfig->Set( "FOLLOW_FILE", followFile );
  myConfig->Set( "IMAGES_ONLY", imagesOnly );
  myConfig->Set( "SLIDESHOW_DELAY", (int32)slideshowDelay );
  myConfig->Set( "LEFT_MOUSE", leftMouseAction );
  myConfig->Set( "RIGHT_MOUSE", rightMouseAction );
  myConfig->Set( "MIDDLE_MOUSE", middleMouseAction );
  myConfig->Set( "WHEEL_MOUSE", wheelMouseAction );
  myConfig->Set( "DRAG_ACTION",  dragAction );
  myConfig->Set( "DRAG_MODE",  dragMode );
  myConfig->Set( "COMPACT_MODE",  compactMode );

  myConfig->Set( "VIEWING_MODE", viewingMode );
  myConfig->Set( "SLIDESHOW_MODE",  slideshowMode );
  myConfig->Set( "WINDOW_FRAME", windowFrame );
  myConfig->Set( "FILELIST_RIGHT", (int32)fileListRight );

  myConfig->Set( "RUN_NON_IMAGE_FILES", runNonImageFiles );
  myConfig->Set( "SLIDE_CENTER_LOAD", slideCenterLoad );
  myConfig->Set( "ALLOW_FULLSCREEN_START", fullscreenStart );
  myConfig->Set( "FULLSCREEN", fullScreen );

  delete myConfig;
}

int32 Setup::LoadSettings()
{
  ConfigureClass *myConfig = new ConfigureClass("Peek_settings");

   if (myConfig->IsNew())
   {
    delete myConfig;
    return B_ERROR; 
   }

   BString temp;
   myConfig->ValueChar("VERSION",&temp);
   this->SetVersion((char*)temp.String());
   
   myConfig->ValueChar("LANGUAGE",&temp);
   this->SetDefaultLanguage((char*)temp.String() );

   followFile = myConfig->ValueBool( "FOLLOW_FILE" );
   imagesOnly = myConfig->ValueBool( "IMAGES_ONLY" );
   slideshowDelay = myConfig->ValueInt32("SLIDESHOW_DELAY");

   leftMouseAction = myConfig->ValueInt32("LEFT_MOUSE");
   rightMouseAction = myConfig->ValueInt32("RIGHT_MOUSE");
   middleMouseAction = myConfig->ValueInt32("MIDDLE_MOUSE");
   wheelMouseAction = myConfig->ValueInt32("WHEEL_MOUSE");
   dragAction = myConfig->ValueInt32("DRAG_ACTION");
   dragMode = myConfig->ValueBool("DRAG_MODE");
   viewingMode = myConfig->ValueInt32("VIEWING_MODE");
   slideshowMode = myConfig->ValueInt32("SLIDESHOW_MODE");

   windowFrame = myConfig->ValueBRect( "WINDOW_FRAME" );
   fileListRight = myConfig->ValueInt32( "FILELIST_RIGHT" );

   runNonImageFiles = myConfig->ValueBool( "RUN_NON_IMAGE_FILES" );
   slideCenterLoad = myConfig->ValueBool( "SLIDE_CENTER_LOAD" );
   compactMode = myConfig->ValueBool( "COMPACT_MODE" );
   fullscreenStart = myConfig->ValueBool( "ALLOW_FULLSCREEN_START");
   fullScreen = myConfig->ValueBool("FULLSCREEN");

  delete myConfig;
  return B_OK;
}
