#ifndef SETUP_H
#define SETUP_H

#include "Language.h"
#include <View.h>
#include <Message.h>

class Setup {

   public:
      void DefaultLanguage(char*);
      void RunningPath(char*);
      void CurrentPath(char*);
      void Version(char*);
      BRect WindowFrame();
      float FileListRight();
      int32 ViewingMode();
      int32 SlideshowMode();
      bool CompactMode();
      bool ThumbnailMode();
      BMessage* NegotiationMessage();
      bool FollowFile();
      bool RememberPath();
      bool ImagesOnly();
      bool SlideLoadIntoCenter();
      bool FullScreen();
      bool RunNonImageFiles();
      bool Sliding();
      unsigned long SlideshowDelay();
      long RightMouseAction();
      long LeftMouseAction();
      long MiddleMouseAction();
      long WheelMouseAction();
      long DragAction();
      bool DragMode();
      bool AllowFullscreenStart();

      void SetDefaultLanguage(char*);
      void SetRunningPath(char*);
      void SetCurrentPath(const char*);
      void SetVersion(char*);
      void SetWindowFrame(BRect);
      void SetFileListRight(float);
      void SetViewingMode(int32);
      void SetSlideshowMode(int32);
      void SetNegotiationMessage(BMessage*);
      void SetFollowFile(bool);
      void SetRememberPath(bool);
      void SetImagesOnly(bool);

      void SetFullScreen(bool);
      void SetCompactMode(bool);
      void SetSlideLoadIntoCenter(bool);
      void SetRunNonImageFiles(bool);
      void SetSliding(bool);
      void SetSlideshowDelay(unsigned long);
      void SetLeftMouseAction(long);
      void SetRightMouseAction(long);
      void SetMiddleMouseAction(long);
      void SetWheelMouseAction(long);
      void SetDragAction(long);
      void SetDragMode(bool);
      void SetAllowFullscreenStart(bool);
      // ***********
      void SaveSettings();
      int32 LoadSettings();
      // ***********
      
   private:
      BMessage *negotiationMessage;
      char version[10];
      char runningPath[B_PATH_NAME_LENGTH];
      char currentPath[B_PATH_NAME_LENGTH];
      char defaultLanguage[30];
      bool fullScreen, followFile, imagesOnly;
      bool slideCenterLoad, runNonImageFiles, sliding, dragMode, rememberPath;
      bool compactMode;
      int32 viewingMode;
      int32 slideshowMode;
      uint32 slideshowDelay;
      int32 leftMouseAction, rightMouseAction, middleMouseAction, wheelMouseAction;
      int32 dragAction;
      float fileListRight;
      bool fullscreenStart;
      
      BRect windowFrame;
};

#endif
