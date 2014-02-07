#ifndef WINDOW_PEEK_H
#define WINDOW_PEEK_H

#include <Window.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Screen.h>
#include <Entry.h>
#include <ScrollBar.h>
#include <ScrollView.h>

#include "Setup.h"
#include "constants.h"
#include "ViewFile.h"

#include "pictureViewer.h"

#include <Background.h>
#include <FindDirectory.h>
#include <fs_attr.h>
#include <FilePanel.h>
#include <String.h>
#include "AllesView.h"
#include "pictureViewer.h"
#include <List.h>
#include <BitmapStream.h>

class WindowPeek : public BWindow {

   public:
    WindowPeek(BRect, char*, Setup*, Language*);

    PictureViewer *imagePane;
    ViewFile *filePane;

    void LoadImage(BEntry*);
    void LoadImage(BBitmap*);
    
    void SetSliding( bool );   
    void SetFullScreen( bool ); 
    void SetCompactMode( bool );
    
    void ExecuteProgram(int32, bool);
    int32 TotalImageFiles();
       
    BMenuBar *mainMenu;

    void FollowFile(BEntry*, bool sel = true);

    virtual void FrameMoved(BPoint);

    bool IsType(BEntry *, char*);

    void CenterWindow();

    virtual void MessageReceived(BMessage *);

    Setup *setup;
   private:
    BFilePanel *saveWindow;
    BMenu* SaveAsMenu( char*, int32);
    void FileSave(BMessage*);
    void SaveFile(BMessage*);

    void MenuTick( BMessage*, bool , bool);
    void DeleteFile();
    void MakeDefault();
    virtual bool QuitRequested();
    virtual void FrameResized(float, float);
    virtual void DispatchMessage(BMessage *, BHandler *);
    void CaptureScreen(bool);

    void CreateLanguageMenu(BMenu*, uint32);

    BEntry* GetDesktopBackground();
    void SetDesktopBackground(int32,bool);
    
    float menubar_height;
    
    AllesView *alles;
   
    Language *words;
};

#endif