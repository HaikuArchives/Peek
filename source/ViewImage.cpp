
#include "WindowPeek.h"
#include "PeekApp.h"
#include <Roster.h>


// PICVIEW ----------------------QUICK & DIRTY BUT HEY ---------------

class PicView : public BView {
   public:
      PicView(BRect, char*, Setup*, Language*);
      void ImageLoaded(bool, float diffx, float diffy);
      void FileLoaded(bool);
      void FileIs(BEntry*);
      BRect clippingRegion;
      bool clipArea;
   private:
      virtual void MouseDown(BPoint);
      virtual void MouseMoved(BPoint,uint32,const BMessage*);
      virtual void MouseUp(BPoint);
      virtual void MessageReceived(BMessage *);

      bool mouseDown;
      float dx,dy;
      BPoint mousePos;
      Language* words;
      Setup* setup;
      bool image, file, clipping;
      BEntry *fileIs;
};



PicView::PicView(BRect r, char* name, Setup* s, Language* w) 
:  BView( r, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS) {
  setup = s;
  words = w;
  mouseDown = false;
  image = false;
  file = false;  
  fileIs = NULL;
  clipArea = false;
}


void PicView::ImageLoaded(bool a, float x, float y) {  image = a; dx = x; dy = y;}
void PicView::FileLoaded(bool a) { file = a; }
void PicView::FileIs(BEntry *e) { fileIs = e; }

void PicView::MessageReceived(BMessage *e) {  
       BView::MessageReceived(e); 
}

void PicView::MouseDown(BPoint a) {

  MakeFocus();
  
  uint32 button;
  BPoint dummy;
  GetMouse(&dummy,&button);
  int32 action = P_SCROLLING;
  if (button == B_PRIMARY_MOUSE_BUTTON) action = setup->LeftMouseAction();
  if (button == B_SECONDARY_MOUSE_BUTTON) action = setup->RightMouseAction();
  if (button == B_TERTIARY_MOUSE_BUTTON) action = setup->MiddleMouseAction();


  if (action == P_SCROLLING || action == P_CLIPPING) {
    if (action == P_CLIPPING) { 
            clipping = true; 
            if (clipArea) {
                 float pointx = Bounds().Width()/2 - dx/2;
                 float pointy = Bounds().Height()/2 - dy/2;
                 if (pointy < 1) pointy = 0;
                 if (pointx < 1) pointx = 0;

                 clippingRegion.left += pointx;
                 clippingRegion.top += pointy;
                 clippingRegion.right += pointx;
                 clippingRegion.bottom += pointy;

               SetViewColor(255,255,255);
               SetDrawingMode(B_OP_INVERT);
               StrokeRect( clippingRegion );
               SetDrawingMode(B_OP_COPY);
               SetViewColor(0,0,0);
               clipArea = false;
            }
       }  else clipping = false;
    mouseDown = true;
    mousePos = a;
    SetMouseEventMask(1);
    return;
  }

  if (action == P_FULLSCREEN) {
      ((WindowPeek*)Window())->SetFullScreen( ! ((WindowPeek*)Window())->setup->FullScreen() );    
       return;
  }
  
  if (action == P_OPENNEW) {
       if (file == false) return;
       entry_ref b;
       fileIs->GetRef(&b);
       be_roster->Launch(&b);
       return;
  }

  if (action == P_DRAGGING && image) {
    BBitmap *dragImage;
    BMessage *msg;
    bool negotiated = setup->DragMode();
     
    // here comes the CTRL keyboard modifier
 
    if ( (modifiers() & B_CONTROL_KEY) == B_CONTROL_KEY ) negotiated = ! negotiated;
 
    //  
 
    if (negotiated) {
      setup->SetNegotiationMessage(   ((PeekApp*)be_app)->SetupNegotiationMessage(setup->DragAction())      );
      msg = new BMessage (*setup->NegotiationMessage());
      BMimeType k("image");          
      dragImage = new BBitmap(BRect(0,0,31,31),B_CMAP8);
      msg->AddString("source","Peek");
      if (k.GetIcon(dragImage,B_LARGE_ICON) == B_OK) DragMessage(msg,dragImage, B_OP_ALPHA, BPoint(16,16));
      delete msg;
    } else if (file) {
       msg = new BMessage(B_SIMPLE_DATA);
       entry_ref tempR;
       fileIs->GetRef(&tempR);
       msg->AddRef("refs",&tempR);

       if (setup->DragAction() != 0) msg->AddInt32("be:actions",setup->DragAction() );
     
       BMimeType k("image");          
       dragImage = new BBitmap(BRect(0,0,31,31),B_CMAP8);
       msg->AddString("source","Peek");
       if (k.GetIcon(dragImage,B_LARGE_ICON) == B_OK) DragMessage(msg,dragImage, B_OP_ALPHA, BPoint(16,16));
      delete msg;
    }  

   return;
  }
  
  if (action == P_HIDEFILES) {
      ((WindowPeek*)Window())->SetCompactMode(  ((WindowPeek*)Window())->setup->CompactMode()   );
      return;
  }
  
}
  


void PicView::MouseMoved(BPoint a, uint32 transit, const BMessage *e) {
  if (mouseDown && clipping) {
    if (clipArea) {
       SetViewColor(255,255,255);
       SetDrawingMode(B_OP_INVERT);
       StrokeRect( clippingRegion );
       SetDrawingMode(B_OP_COPY);
       SetViewColor(0,0,0);
    }   

     clippingRegion.Set( mousePos.x, mousePos.y, a.x,a.y );
     clipArea = true;

       SetViewColor(255,255,255);
       SetDrawingMode(B_OP_INVERT);
       StrokeRect( clippingRegion );
       SetDrawingMode(B_OP_COPY);
       SetViewColor(0,0,0);
     
  }

  if (mouseDown && clipping == false) {
       float hmin,hmax,vmin,vmax;
       ScrollBar(B_HORIZONTAL)->GetRange(&hmin,&hmax);   // get max & mins
       ScrollBar(B_VERTICAL)->GetRange(&vmin,&vmax);  

      float xe = 0;
      float ye = 0;
      
      if (mousePos.x < a.x) xe = - (a.x - mousePos.x); else xe = (mousePos.x - a.x);
      if (mousePos.y < a.y) ye = - (a.y - mousePos.y); else ye = (mousePos.y - a.y);

      float xdiff =  xe + ScrollBar(B_HORIZONTAL)->Value();
      float ydiff =  ye + ScrollBar(B_VERTICAL)->Value();
      
      if (xdiff < hmin) xdiff = hmin;   // make sure they're in range
      if (xdiff > hmax) xdiff = hmax;   // make sure they're in range
      if (ydiff < vmin) ydiff = vmin;   // make sure they're in range
      if (ydiff > vmax) ydiff = vmax;   // make sure they're in range

      Window()->Lock();
      ScrollTo(xdiff,ydiff);
      Window()->Unlock();
      
   }   
}

void PicView::MouseUp(BPoint a) {
  mouseDown = false;
  SetMouseEventMask(0);
  if (clipping) {

     float pointx = Bounds().Width()/2 - dx/2;
     float pointy = Bounds().Height()/2 - dy/2;
     if (pointy < 1) pointy = 0;
     if (pointx < 1) pointx = 0;

      clippingRegion.left -= pointx;
      clippingRegion.top -= pointy;
      clippingRegion.right -= pointx;
      clippingRegion.bottom -= pointy;
   }
 clipping = false;
}


// PICVIEW ----------------- END HERE ----------------

ViewImage::ViewImage(BRect r, char* name, Setup* s, Language* l)
: BView( r, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS)
 {
  setup = s;
  words = l;

  image = NULL;
  file = NULL;
 picView = new PicView( 
             BRect( 1, 1, r.Width() - B_V_SCROLL_BAR_WIDTH, r.Height() - B_H_SCROLL_BAR_HEIGHT),
             "picView" , setup, words);
  picView->SetViewColor(0,0,0);


  BScrollView *tempView = new BScrollView( "picScrollView", picView, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM , 0, true, true);
  AddChild(tempView);
  picView->ScrollBar(B_VERTICAL)->SetRange(0,0);
  picView->ScrollBar(B_HORIZONTAL)->SetRange(0,0);

}

ViewImage::~ViewImage() {
  Clear();
}



// Clear -- deletes any files or images in memory and sets the corresponding
//          values in PicView to whatever they need to be.
void ViewImage::Clear() {
   if (image != NULL) delete image;
   if (file != NULL) delete file;
   file = NULL;
   image = NULL;
   ((PicView*)picView)->ImageLoaded(false,0,0);
   ((PicView*)picView)->FileLoaded(false);
   ((PicView*)picView)->clipArea = false;
}

// Refresh --  This invalidates around the image that has just been loaded, or moved, or
//             whatever. It allows for smooth moving and redrawing.
//             It's also responsible for calling AdjustScrollBars()
void ViewImage::Refresh() {

  if (image != NULL && setup->ViewingMode() != PEEK_IMAGE_TILE && setup->ViewingMode() != PEEK_IMAGE_SCALE_TO_WINDOW) {
   float pointx = picView->Bounds().Width()/2 - image->Bounds().Width()/2;
   float pointy = picView->Bounds().Height()/2 - image->Bounds().Height()/2;
   if (pointy < 1) pointy = 1;
   if (pointx < 1) pointx = 1;

   BRegion viewRegion(picView->Bounds());
   viewRegion.Exclude(  BRect(pointx+1,pointy+1, pointx + image->Bounds().Width(), pointy + image->Bounds().Height()) );
   picView->Invalidate(&viewRegion);
  }
  
  if (image == NULL) picView->Invalidate(Bounds());
  AdjustScrollBars();    
}


void ViewImage::AdjustScrollBars() {
  if (image == NULL || setup->ViewingMode() == PEEK_IMAGE_TILE || setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW) {
     picView->ScrollBar(B_VERTICAL)->SetRange(0,0);
     picView->ScrollBar(B_HORIZONTAL)->SetRange(0,0);
  } else {
     float width  = image->Bounds().Width() - picView->Bounds(). Width() +  2;
     float height = image->Bounds().Height() - picView->Bounds().Height() + 2;
     if (width < 1) width = 0;
     if (height < 1) height = 0;
     picView->ScrollBar(B_VERTICAL)->SetRange(0,height);
     picView->ScrollBar(B_HORIZONTAL)->SetRange(0,width);
  }   
}


//  GetEntry -- returns the BEntry of the file loaded at the moment (or NULL) if the
//              image has no file or is not loaded. 
//              DO NOT DELETE. It does not belong to you.
BEntry* ViewImage::GetEntry() {
  return file;
}

// LoadImage --  This loads the bitmap passed into memory. A new BBitmap is created.
//               You can delete the BBitmap you passed it once this method is finished.
//               And old images in memory are deleted.
int32 ViewImage::LoadImage(BBitmap *picBitmap) {
  if (picBitmap == NULL) return B_ERROR;
  Clear();
  if (setup->ViewingMode() == PEEK_IMAGE_THUMBNAILS) {
        ((WindowPeek*)Window())->ToggleThumbnailView();
  }   

  image = new BBitmap(picBitmap);
  if (image == NULL) {
                       Window()->Unlock();
                       return B_ERROR;
                     }
  ((PicView*)picView)->ImageLoaded(true, image->Bounds().Width(), image->Bounds().Height());

  Window()->SetTitle("unsaved data");
  if (setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE) ResizeToImage();

  if ((setup->LoadIntoCenter()) && (setup->ViewingMode()==PEEK_IMAGE_WINDOW_TO_IMAGE )) 
         ( (WindowPeek*)Window()  )->CenterWindow();

  Refresh();
  return B_OK;
}


// LoadImage -- this is the version which loads a BEntry
//              The window must be locked before you call this. This does not handle that.
//
int32 ViewImage::LoadImage(BEntry *picFile) {
  if (picFile == NULL) return B_ERROR;
  Clear();
  if (setup->ViewingMode() == PEEK_IMAGE_THUMBNAILS) {
      ((WindowPeek*)Window())->ToggleThumbnailView();
    }  
 
  file = new BEntry(*picFile);

  entry_ref tempRef;
  file->GetRef(&tempRef);
  image = BTranslationUtils::GetBitmap(&tempRef);
  if (image == NULL) return B_ERROR;    
  ((PicView*)picView)->ImageLoaded(true, image->Bounds().Width(), image->Bounds().Height());
  ((PicView*)picView)->FileLoaded(true);
  ((PicView*)picView)->FileIs(file);

  char name[B_FILE_NAME_LENGTH];
  file->GetName(name);
  Window()->SetTitle(name);

  // ----- center first and then resize. It looks better.
  if ((setup->LoadIntoCenter()) && (setup->ViewingMode()==PEEK_IMAGE_WINDOW_TO_IMAGE )) 
      (  (WindowPeek*)Window()  ) -> CenterWindow();

  if (setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE) ResizeToImage();
  // end center stuff

  Refresh();
  return B_OK;
}

// CropToSelection -- just takes the selected area, if any, and makes it the
//                    whole image

void ViewImage::CropToSelection() {
  if (image == NULL) return;
  BBitmap *newImage = getClipping();
  if (newImage == NULL) return;
  LoadImage(newImage);
}

//  creating a thumbnail view

void ViewImage::CreateThumbImage(char* path) {
  Clear();
  BDirectory *dir = new BDirectory(path);

  int size = setup->ThumbSize();
  int32 totalEntries = 0;
  totalEntries = ((WindowPeek*)Window())->TotalImageFiles();
  
  long int perrow = (long int)(Bounds().Width() / (size + 10));

  long int width =  perrow * (size + 10) + 20;
  long int height = ((totalEntries / perrow) + 1 ) * (size + 10) + 20;

  image = new BBitmap( BRect(0,0,width,height), B_RGB32, true);
  ((PicView*)picView)->ImageLoaded(true, image->Bounds().Width(), image->Bounds().Height());
  image->Lock();

  BView *sammy = new BView( BRect(0,0,width,height), "tempView",0,B_WILL_DRAW);
  image->AddChild(sammy);
  
  int32 i= 0;
  dir->Rewind();
  BEntry *current = new BEntry();
  entry_ref bob;
  BRect a;
  while (dir->GetNextEntry(current, true) == B_OK) {

    current->GetRef(&bob);

    if (((WindowPeek*)Window())->IsType(current,"image")) {
        BBitmap *tempImage = BTranslationUtils::GetBitmap(&bob);
        
        a.Set( 10 + (i % perrow) * (size + 10), 10 + i / perrow * (size + 10),
               10 + (i % perrow) * (size + 10) + size, 10 + i / perrow * (size + 10) + size
                );
        
        if (tempImage != NULL) { 
             sammy->DrawBitmap(tempImage,a);
             delete tempImage;
          }   
        i++;
    }
  }

  image->RemoveChild(sammy);
  delete sammy;
  delete dir;
  image->Unlock();
  Refresh();
}

// -------------

// ScrollBy -- merely passes it all along to the picView so that it can handle it
//             since it is the main star of the show.
void ViewImage::ScrollBy(float x, float y) {
    float hi,ha;
    float vi,va;
    float newx, newy;
    picView->ScrollBar( B_HORIZONTAL )->GetRange( &hi, &ha );
    picView->ScrollBar( B_VERTICAL   )->GetRange( &vi, &va );

    newx =  picView->ScrollBar( B_HORIZONTAL )->Value() + x;
    newy =  picView->ScrollBar( B_VERTICAL )->Value() + y;
    
    if (newx < hi) newx = hi;
    if (newx > ha) newx = ha;
    if (newy < vi) newy = vi;
    if (newy > va) newy = va;

    picView->ScrollTo(newx,newy);
}


void ViewImage::ResizeToImage() {
   if (setup->FullScreen()) return;
   if (image == NULL) return;
//    WINDOW RESIZED <---------------------------------
    float  width = Bounds().Width() - image->Bounds().Width() - B_V_SCROLL_BAR_WIDTH - 4;
    float height = Bounds().Height() - image->Bounds().Height() - B_H_SCROLL_BAR_HEIGHT - 4;

    BScreen theScreen;
      // make sure the window doesn't exceed the bottom and right side of the screen
    if (Window()->Frame().right - width > theScreen.Frame().right - 10) width = -theScreen.Frame().right + Window()->Frame().right +10;
    if (Window()->Frame().bottom - height > theScreen.Frame().bottom - 10) height = -theScreen.Frame().bottom + Window()->Frame().bottom +10;

    Window()->ResizeBy( -width, -height);
}


// getClipping -- this will return the part of the image selected
//                if there is a selection, otherwise NULL.
BBitmap* ViewImage::getClipping() {
 if (((PicView*)picView)->clipArea == false) return NULL;

 BRect clipRegion = ((PicView*)picView)->clippingRegion;
 BBitmap *dragImage = new BBitmap(BRect(0,0,clipRegion.Width(),clipRegion.Height()), image-> ColorSpace(),true,false);

 BView *tempView = new BView(dragImage->Bounds(),"yo",0,0);
    dragImage->AddChild(tempView);
    dragImage->Lock();
    tempView->MovePenTo( -clipRegion.left, -clipRegion.top);
    tempView->DrawBitmap(image);
    dragImage->Unlock();
    dragImage->RemoveChild(tempView);
    delete tempView;
 return dragImage;
}

// Bitmap will return a pointer to the current image in memory. Or null.
BBitmap* ViewImage::Bitmap() {
    if (image == NULL) return NULL;
    return image;
}

void ViewImage::Draw(BRect R) {
  if (image == NULL) return;
  
  if (setup->ViewingMode() == PEEK_IMAGE_NORMAL  ||  setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE ||
      setup->ViewingMode() == PEEK_IMAGE_THUMBNAILS ) {
     float pointx = picView->Bounds().Width()/2 - image->Bounds().Width()/2;
     float pointy = picView->Bounds().Height()/2 - image->Bounds().Height()/2;
     if (pointy < 1) pointy = 1;
     if (pointx < 1) pointx = 1;
     picView->DrawBitmap(image, BPoint(pointx,pointy) );
  }


  if (setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW) {
     picView->DrawBitmap(image, BRect(1,1,picView->Bounds().Width()-1,picView->Bounds().Height()-1) );
  }

  if (setup->ViewingMode() == PEEK_IMAGE_TILE) {
       for (float i=1; i < picView->Bounds().Width(); i+= image->Bounds().Width() + 1) {
         for (float j=1; j < picView->Bounds().Height(); j+= image->Bounds().Height() + 1) {
          picView->DrawBitmap(image, BPoint(i,j) );
         }
       }
  }

  if (((PicView*)picView)->clipArea) {
       float left, top;
       BRect dada = ((PicView*)picView)->clippingRegion;
        left = picView->Bounds().Width()/2 - image->Bounds().Width()/2 ;
        top =  picView->Bounds().Height()/2 - image->Bounds().Height()/2 ;
        if (top < 1) top = 0;
        if (left < 1) left = 0;
       left += dada.left;
       top += dada.top;

       picView->SetViewColor(255,255,255);
       picView->SetDrawingMode(B_OP_INVERT);
       picView->StrokeRect(  BRect( left,top, left + dada.Width(), top + dada.Height()) );
       picView->SetDrawingMode(B_OP_COPY);
       picView->SetViewColor(0,0,0);
  }

  
  Sync();
}

// DRAG AND DROP BEGINS HERE

bool ViewImage::SupportedFormat(const char *s) {
      translator_id *list;
      int32 num;
      BTranslatorRoster::Default()->GetAllTranslators(&list,&num);

       for (int i=0; i < num; i++) {
         int32 numOfTypes;
         const translation_format *in_types;
         BTranslatorRoster::Default()->GetInputFormats(list[i],&in_types,&numOfTypes);
         for (int j = 0; j <numOfTypes; j++) if ( strcmp(in_types[j].MIME,s) == 0) return true;
       }
 return false;
}



void ViewImage::HandleCopyTarget(BMessage *e) {
  entry_ref targetRef;
  bool delly = false;
  BBitmap *thePic = getClipping();
  if (thePic == NULL) {  
     thePic = image;
     delly = true;
   }  
  // memory or file transfer ?
  if (e->FindRef("directory",&targetRef) == B_OK)  {

      BDirectory *targetDir = new BDirectory( &targetRef );
      if (targetDir->InitCheck() == B_OK) {
         BFile *targetFile = new BFile(targetDir,e->FindString("name"),B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
         if (targetFile->InitCheck() == B_OK) {

          BBitmap *dragImage = NULL;
          BBitmapStream *strm;
          strm = new BBitmapStream(thePic);    

          translator_id trans_id;
          uint32 type;

          GetTranslatorInfo(e->FindString("be:filetypes"),&trans_id,&type);
          BTranslatorRoster::Default()->Translate(trans_id,strm,NULL,targetFile,type);
          BNodeInfo *ni = new BNodeInfo(targetFile);
          ni->SetType(e->FindString("be:filetypes"));
          if (dragImage == NULL) strm->DetachBitmap(&thePic);
           else {
            strm->DetachBitmap(&dragImage);
            delete dragImage;
           }
          delete ni;
          delete strm;
         } 
        delete targetFile;
       }   
     delete targetDir;
     return;
   } 
   
  else {
     BMallocIO *data = new BMallocIO;
     BBitmapStream *imageSrc;
     imageSrc = new BBitmapStream(thePic);

     BMessage *package = new BMessage(B_MIME_DATA);
     translator_id trans_id;
     uint32 type;

     GetTranslatorInfo(e->FindString("be:types"),&trans_id,&type);
     if (BTranslatorRoster::Default()->Translate( trans_id, imageSrc, NULL, data, type ) != B_OK) return;
     package->AddData( e->FindString("be:types"),  B_MIME_DATA, data->Buffer(), data->BufferLength());
     e->SendReply(package);        

     imageSrc->DetachBitmap(&thePic);

     delete data;
     delete imageSrc;
     delete package;
  } 
  if (delly == false) delete thePic;
}

void ViewImage::GetTranslatorInfo(const char *m, translator_id *t, uint32 *t2) {
      translator_id *list;
      int32 num;
      BTranslatorRoster::Default()->GetAllTranslators(&list,&num);

       for (int i=0; i < num; i++) {
         int32 numOfTypes;
         const translation_format *out_types;
         BTranslatorRoster::Default()->GetOutputFormats(list[i],&out_types,&numOfTypes);
         for (int j = 0; j <numOfTypes; j++) if ( strcmp(out_types[j].MIME,m) == 0) {
           *t = list[i];
           *t2 = out_types[j].type;
           return;
         }
       }
 *t = 0;
 *t2 = 0;
}



void ViewImage::HandleMimeData(BMessage *e) {
    int32 numBytes;
    type_code type;
    const void *data;
    const char *m_type;
    if (e->GetInfo( B_MIME_TYPE, 0, (char**)&m_type, &type, &numBytes) != B_OK) return;

    e->FindData(m_type,B_MIME_TYPE,&data,&numBytes);
    BMemoryIO memStream(data, numBytes);
    BBitmapStream imageStream;
    if (strcmp(m_type,"image/x-be-bitmap")==0) {
     uint8 buffer[1024];
     int32 size;
     while ((size=memStream.Read(buffer,1024))>0) 
       imageStream.Write(buffer,size);
    } 
    else if ( BTranslatorRoster::Default()->Translate(&memStream,NULL,NULL, &imageStream, B_TRANSLATOR_BITMAP, 0, m_type) != B_OK) return;

    BBitmap *bmp;
    imageStream.DetachBitmap(&bmp);
    LoadImage(bmp);
    delete bmp;
    return;  
}

void ViewImage::HandleTrashTarget(BMessage *e) {
}

void ViewImage::HandleSimpleData(BMessage *e) {
                                           // CASE 1:  we have a file
     entry_ref ref;
     if (e->FindRef("refs", &ref) == B_OK )  {
       BEntry *file = new BEntry(&ref, true);
       if (file->IsDirectory()) ((WindowPeek*)Window())->FollowFile(file);
                           else {
                                  LoadImage(file);  // if it doesn't work, it might be a directory
                                  if (setup->FollowFile()) ((WindowPeek*)Window())->FollowFile(file);
                                 } 
       delete file;
       return;
     }

           // CASE 2:   we don't have a file.

      // make sure we're not wasting our time
     if (  strcmp(e->FindString("source"),"Peek") == 0 ) {
         BBitmap *sam  = getClipping();
         if (sam == NULL) { 
                            delete sam;    // we are, so we go away.
                            return;
                           }  
         delete sam;
     }    

     const char *m_type;
     int32 i = 0, action;
     while ( e->FindInt32("be:actions",i,&action) == B_OK && action != B_COPY_TARGET) i++;
     if (action != B_COPY_TARGET) return;
     i = 0;
     while (e->FindString("be:types",i,&m_type) == B_OK && !SupportedFormat(m_type)) i++;
     if (!m_type) return; 
     
     BMessage yahoo(B_COPY_TARGET);
     yahoo.AddString("be:types",m_type);
     e->SendReply(&yahoo,this);
}


void ViewImage::MessageReceived(BMessage *e) {

   switch (e->what) {
     case B_SIMPLE_DATA: HandleSimpleData(e); break;                          
     case B_MIME_DATA: HandleMimeData(e); break;                          
     case B_TRASH_TARGET: HandleTrashTarget(e); break;                          
     case B_COPY_TARGET: HandleCopyTarget(e); break;                          
     default: BView::MessageReceived(e); break;
   }

}

void ViewImage::FrameResized(float dx,float dy) {
  Refresh();
}

float ViewImage::Width() {
  if (image == NULL) return 0;
  return image->Bounds().Width();
}

float ViewImage::Height() {
  if (image == NULL) return 0;
  return image->Bounds().Height();
}