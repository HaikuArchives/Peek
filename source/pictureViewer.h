#ifndef PICTURE_VIEWER_H
#define PICTURE_VIEWER_H

#include <View.h>
#include <List.h>
#include <Bitmap.h>
#include <Entry.h>
#include <Region.h>
#include <Window.h>
#include <Screen.h>
#include <ScrollBar.h>
#include <Mime.h>

#include "constants.h"
#include "Setup.h"

#include <be/translation/TranslationUtils.h>


class PictureViewer : public BView {

  public:
    PictureViewer(char*, Setup*);

    // Acceptable loading methods
    void LoadBitmap( BBitmap* );
    void LoadEntry ( BEntry*  );

    // outside access methods
    BEntry* CurrentFile();
    BBitmap* Bitmap();

    // control methods
    void Refresh();
    void EmptyList();
    void CropToSelection();
    void ClearClipping();

    // display controls
    void ResizeToImage();
    void SetZoom(float);
    float GetZoom() const;
  
  private:
    BBitmap* thePic;
    entry_ref* theRef;

    // stuff
    void AdjustScrollBars();

    void DrawSingleImage( BView *);
    virtual void MessageReceived(BMessage* );
    virtual void Draw(BRect);
    virtual void DrawClipping();
    virtual void FrameResized(float, float);

    BBitmap* GetClipping();

//    user interface stuff
    void HandleSimpleData(BMessage*);
    void HandleMimeData(BMessage*);
    void HandleTrashTarget(BMessage*);
    void HandleCopyTarget(BMessage*);
    bool SupportedFormat(const char *);
    void GetTranslatorInfo(const char *, translator_id *, uint32 *);

    virtual void MouseUp   ( BPoint );
    virtual void MouseDown ( BPoint );
    virtual void MouseMoved( BPoint, uint32, const BMessage* );    


//    Scrolling control
    bool mouseDown;
    bool clipping, busyClipping;
    BRect clippingRegion;
    BPoint oldMousePos;

    // Zoom
    float imgZoom;

    Setup *setup;
};


#endif
