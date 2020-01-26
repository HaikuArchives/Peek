#include "pictureViewer.h"
#include "WindowPeek.h"
#include "ViewFile.h"
#include "PeekApp.h"
#include <Roster.h>

// PictureViewer - will replace picView to enhance the viewing experience
//                 and people will go "ooh, aah, wow..."

PictureViewer::PictureViewer(char* name, Setup* s)
: BView( name, B_WILL_DRAW | B_FRAME_EVENTS )
{
  theRef = NULL;
  thePic = NULL;
  setup = s;
  SetViewColor(0,0,0);
}



// LoadEntry --  Takes an entry and then loads the BBitmap and entry_ref
//
//               YOU MUST DELETE THE BEntry
//
void PictureViewer::LoadEntry( BEntry* e ) {
   if ( e == NULL ) return;
   Window()->Lock();
   EmptyList();
   theRef = new entry_ref;
   e->GetRef( theRef );
   thePic = BTranslationUtils::GetBitmap(theRef);

   if ( setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE ) ResizeToImage();
   Window()->Unlock();
   Refresh();
}


// LoadBitmap -- Takes the BBitmap and then loads it accordinly into
//               the list... using the BBitmap constructor of a bitmapNode
//               DO NOT DELETE the BBitmap when the method returns. It's 
//               mine now.
void PictureViewer::LoadBitmap( BBitmap* b ) {
  if (b == NULL) return;
  Window()->Lock();
  EmptyList();

  thePic = b;
  if ( setup->ViewingMode() == PEEK_IMAGE_WINDOW_TO_IMAGE ) ResizeToImage();
  Window()->Unlock();
  Refresh();
}


// Empty List -- gets rid of everything. You should not be afraid of this
//               It's clean.
void PictureViewer::EmptyList() {
   if (thePic == NULL) return;
  clipping = false;

        delete thePic;
        if (theRef != NULL) delete theRef;
        thePic = NULL;
        theRef = NULL;
        Refresh();
}


// -------------------------------------------------------------------
//                           OUTSIDE ACCESS STUFF
//          People from outside can get information from these
//          methods. And do stuff with this.
// -------------------------------------------------------------------


// CurrentFile -- returns a BEntry. It's your BEntry. You MUST
//                delete it when you need to.
BEntry* PictureViewer::CurrentFile() {
    if (thePic == NULL) return NULL;
    if (theRef == NULL) return NULL;
    return ( (new BEntry( theRef, true)) );
}


// Bitmap -- This will return a BBitmap of the whole view
//           NB: Only the currently selected BBitmap at the moment
BBitmap* PictureViewer::Bitmap() {
    return ( thePic );
}


// CropToSelection -- Just crops to the selection
//
//
void PictureViewer::CropToSelection() {
   if (thePic == NULL) return;
   if (clipping == false) return;
   LoadBitmap( GetClipping() );  
}



// ---------------------------------------------------------------
//                             DRAWING METHODS  
// ---------------------------------------------------------------


void PictureViewer::DrawClipping() {
      if (thePic == NULL) return;
      if (clipping) {
  
        float left, top;
        left = (Bounds().Width()/2   - thePic->Bounds().Width() / 2);
        top =  (Bounds().Height()/2  - thePic->Bounds().Height() / 2) ;
        if (top < 1) top = 0;
        if (left < 1) left = 0;
        left += clippingRegion.left;
        top += clippingRegion.top;

         BRect newRect = clippingRegion;
         newRect.left = left;
         newRect.right = left + clippingRegion.Width();
         newRect.top = top;
         newRect.bottom = top + clippingRegion.Height();
         
         if (busyClipping) newRect = clippingRegion;
  
         SetDrawingMode( B_OP_INVERT );
         StrokeRect( newRect );
         SetDrawingMode( B_OP_COPY );
      }
}


// GetClipping -- this will return NULL if the user hasn't selected any region of
//                the screen. If she has, it will return a BBitmap representing that
//                selection. You MUST delete it. It's not mine.
//                Anything else, it returns NULL.
BBitmap * PictureViewer::GetClipping() {
        if (thePic == NULL) return NULL;
        if (clipping == false) return NULL;
         
        BBitmap *dragImage = new BBitmap(BRect(0,0,clippingRegion.Width(),clippingRegion.Height()), thePic->ColorSpace(),true,false);

        BView *tempView = new BView(dragImage->Bounds(),"yo",0,0);
           dragImage->AddChild(tempView);
           dragImage->Lock();
           tempView->SetViewColor(0,0,0);
           float left = -clippingRegion.left;
           float top = -clippingRegion.top;
           tempView->MovePenTo( left, top);
           tempView->DrawBitmap(thePic);
           dragImage->Unlock();
           dragImage->RemoveChild(tempView);
           delete tempView;
        return dragImage;
}


// Draw --  The main draw method
//

void PictureViewer::Draw( BRect R ) {
  if (thePic == NULL) return;
  DrawSingleImage(this);
  Sync();
}


// DrawSingleImage -- takes the first image in the linked list
//                    and draws it
//
//
void PictureViewer::DrawSingleImage( BView *target_view) {
   
   int32 mode = setup->ViewingMode();
   
   if (
        (mode == PEEK_IMAGE_NORMAL) ||
        (mode == PEEK_IMAGE_WINDOW_TO_IMAGE)
      ) 
   {
      float x = target_view->Bounds().Width()  / 2;
      float y = target_view->Bounds().Height() / 2;
      x = x - thePic->Bounds().Width()/2;
      y = y - thePic->Bounds().Height()/2;
      if (x < 1) x = 1;
      if (y < 1) y = 1;
      target_view->DrawBitmap( thePic , BPoint(x,y) );
   }
   
   if (
        (mode == PEEK_IMAGE_TILE)
      )
   {
      for (float i = 0; i < target_view->Bounds().Width(); i += thePic->Bounds().Width()) {
       for (float j = 0; j < target_view->Bounds().Height(); j += thePic->Bounds().Height()) {
           if (thePic != NULL) target_view->DrawBitmap( thePic, BPoint( i,j ) ); 
              else break;
        }
       if (thePic == NULL) break;
      }
   }
  
   
   if ( (mode == PEEK_IMAGE_SCALE_TO_WINDOW ) )
   {
      target_view->DrawBitmap( thePic, BRect( 1,1, Bounds().Width()-2, Bounds().Height()-2 ) );
   }

  if ( (mode == PEEK_IMAGE_SCALED_NICELY) )
  {
    float width  = target_view->Bounds().Width();
    float height = target_view->Bounds().Height();

    float  pic_width  = thePic->Bounds().Width();
    float  pic_height = thePic->Bounds().Height();
    
    float ratio = 1;
    
    if ( (width - pic_width) < (height - pic_height) )
    {
       ratio = width / pic_width;
    }
    else 
    {
       ratio = height / pic_height;
    }

    BRect new_rect(0,0,pic_width * ratio, pic_height * ratio );
    
    float x = Bounds().Width() / 2 - new_rect.Width() / 2;
    float y = Bounds().Height() / 2 - new_rect.Height() / 2;
    
    new_rect.left   += x;
    new_rect.top    += y;
    new_rect.right  += x;
    new_rect.bottom += y;
    
    target_view->DrawBitmap( thePic, new_rect );
  }

  DrawClipping();
  Sync();
}

// ResizeToImage --  Resizes the window to fit snugly around the image.
//                   Should be called ONLY when there is 1 image loaded
//                   and no more. & only if the window isn't FullScreen
void PictureViewer::ResizeToImage() {
   if (setup->FullScreen()) return;
   if (thePic == NULL) return;
  
  // the window needs to be adjusted by byX and byY points
   float byX = Bounds().Width() - thePic->Bounds().Width() - 3;
   float byY = Bounds().Height() - thePic->Bounds().Height() - 3;
  
  // Now we're going to make sure it's still on the screen.
    // get the coordinates
   BScreen *theScreen = new BScreen();
    float screen_right  = theScreen->Frame().Width();
    float screen_bottom = theScreen->Frame().Height();
   delete theScreen;

  // determine where the new position should be
   float newX = Window()->Frame().right - byX;
   float newY = Window()->Frame().bottom - byY;

  // is the new position greater than the screen width? hmm... fix if so
   if (newX > screen_right)   byX = -(screen_right - Window()->Frame().right) + 5;
   if (newY > screen_bottom)  byY = -(screen_bottom - Window()->Frame().bottom) + 5;

  // finalize
   Window()->ResizeBy( - byX, - byY );
}


// This will just get rid of the clipping thingy
//
void PictureViewer::ClearClipping()
{
  int32 mode = setup->ViewingMode();
    if (  (mode == PEEK_IMAGE_SCALED_NICELY)   ||
          (mode == PEEK_IMAGE_SCALE_TO_WINDOW) ||
          (mode == PEEK_IMAGE_SCALE_TO_WINDOW) ||
          (mode == PEEK_IMAGE_TILE)
       ) 
    {
      clipping = false;
      busyClipping = false;
    }
}

// Refresh --  Should anything change within the View, this is the method
//             to call. it does everything you need to.
//             It also handles the refreshing of only the refresh-needing parts
//             so it does smooth scrolling, etc
void PictureViewer::Refresh() {

  Window()->Lock();
  int32 mode = setup->ViewingMode();

  if (  thePic != NULL && 
        ( 
          mode == PEEK_IMAGE_NORMAL ||
          mode == PEEK_IMAGE_WINDOW_TO_IMAGE
        )
      ) 
  {
   float pointx = Bounds().Width()/2 - thePic->Bounds().Width()/2 ;
   float pointy = Bounds().Height()/2 - thePic->Bounds().Height()/2 ;
   if (pointy < 1) pointy = 1;
   if (pointx < 1) pointx = 1;

   BRegion viewRegion( Bounds() );
   viewRegion.Exclude(  BRect(pointx+1,pointy+1, pointx + thePic->Bounds().Width(), pointy + thePic->Bounds().Height()) );
   Invalidate(&viewRegion);
  }
  
  if ( (thePic != NULL) && (mode == PEEK_IMAGE_SCALED_NICELY) )
  {
    float width  = Bounds().Width();
    float height = Bounds().Height();

    float  pic_width  = thePic->Bounds().Width();
    float  pic_height = thePic->Bounds().Height();
    
    float ratio = 1;
    
    if ( (width - pic_width) < (height - pic_height) )
    {
       ratio = width / pic_width;
    }
    else 
    {
       ratio = height / pic_height;
    }

    BRect new_rect(0,0,pic_width * ratio, pic_height * ratio );
    
    float x = Bounds().Width() / 2 - new_rect.Width() / 2;
    float y = Bounds().Height() / 2 - new_rect.Height() / 2;
    
    new_rect.left   += x;
    new_rect.top    += y;
    new_rect.right  += x;
    new_rect.bottom += y;
    
    BRegion viewRegion( Bounds() );
    viewRegion.Exclude(  new_rect );
    Invalidate(&viewRegion);
  }
  
  
 
 if ( thePic != NULL  )   
 {
       DrawSingleImage(this);
 }
 else 
 {
  Invalidate();
 }

 AdjustScrollBars();
 Window()->Unlock();
}

// AdjustScrollBars --  Merely keep all our stuff within a nice view
//                      range
//

void PictureViewer::AdjustScrollBars() {
  if ( (thePic == NULL) || (setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW) ||
       (setup->ViewingMode() == PEEK_IMAGE_SCALED_NICELY)
     ) {
                         ScrollBar( B_VERTICAL ) -> SetRange( 0, 0 );
                         ScrollBar( B_HORIZONTAL ) -> SetRange( 0, 0 );
                         return;
                       }

  float x = 0;
  float y = 0;
  

                // we do a [if (thePic != NULL) ] because I can't handle the Multitasking
                // aspect of the Slideshow. Sometimes the program gets here, from the
                // top of the method and the picture has already been deleted. I've
                // tried everything to synchronize it all. Maybe I've missed something.
                if (thePic != NULL) x = thePic->Bounds().Width();
                if (thePic != NULL) y = thePic->Bounds().Height();
  
  if ( Bounds().Width()  > x ) x = 0;
                          else x = x - Bounds().Width();
  if ( Bounds().Height() > y ) y = 0;
                          else y = y - Bounds().Height();  
  
  
  ScrollBar( B_VERTICAL ) -> SetRange( 0, y );
  ScrollBar( B_HORIZONTAL ) -> SetRange( 0, x );
}


// -------------------------------------------------------------------
//                           HOOK FUNCTIONS
// -------------------------------------------------------------------

void PictureViewer::FrameResized( float x, float y ) {
  AdjustScrollBars();
  Refresh();
}

void PictureViewer::MouseUp(BPoint a) {
  mouseDown = false;
  if (clippingRegion.Width() < 10  && clippingRegion.Height() < 20) {
          DrawClipping();
          clipping = false;
  }
 if (busyClipping == true) 
   if (thePic != NULL)
                           {
                             float distx = ( Bounds().Width() - thePic->Bounds().Width() ) / 2;
                             float disty = ( Bounds().Height() - thePic->Bounds().Height() ) / 2;
                             if (disty < 0) disty = 0;
                             if (distx < 0) distx = 0;
                             clippingRegion.left -= distx;
                             clippingRegion.top  -= disty;
                             clippingRegion.right -= distx;
                             clippingRegion.bottom -= disty;
                             busyClipping = false;
                           }
}

void PictureViewer::MouseDown(BPoint a) {
  mouseDown = true;  
  oldMousePos = a;  
  SetMouseEventMask(1);


  uint32 buttons;
  int32 action = 0;
  BPoint ad;
  GetMouse(&ad, &buttons );
  
  if ( buttons == B_PRIMARY_MOUSE_BUTTON )  action = setup->LeftMouseAction();
  if ( buttons == B_SECONDARY_MOUSE_BUTTON )  action = setup->RightMouseAction();
  if ( buttons == B_TERTIARY_MOUSE_BUTTON )  action = setup->MiddleMouseAction();

  if (action == P_OPENNEW) {
       if (thePic != NULL) {
          if (theRef != NULL)  be_roster->Launch(theRef);
          mouseDown = false;
          return;
       }
  }

  if (action == P_CLIPPING) 
  {
    if (  (setup->ViewingMode() == PEEK_IMAGE_SCALED_NICELY)   ||
          (setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW) ||
          (setup->ViewingMode() == PEEK_IMAGE_SCALE_TO_WINDOW)
       ) return;
       
       if (thePic != NULL) 
       {
          if (clipping) DrawClipping();
          busyClipping = true;
          clipping = true;
          clippingRegion.Set( ad.x, ad.y, ad.x, ad.y );
       }
  }

  if (action == P_FULLSCREEN) {
      ((WindowPeek*)Window())->SetFullScreen( ! setup->FullScreen() );    
      return;
  }

  if (action == P_HIDEFILES) {
      ((WindowPeek*)Window())->SetCompactMode( ! setup->CompactMode()   );
      return;
  }


}



// MouseMoved -- Handles a lot of the important stuff
//               and user input.
void PictureViewer::MouseMoved(BPoint a, uint32 m, const BMessage* msg) {
   if (mouseDown == false) return;

   uint32 buttons;
   int32 action = 0;
   BPoint ad;
   GetMouse(&ad, &buttons );
  
   if ( buttons == B_PRIMARY_MOUSE_BUTTON )  action = setup->LeftMouseAction();
   if ( buttons == B_SECONDARY_MOUSE_BUTTON )  action = setup->RightMouseAction();
   if ( buttons == B_TERTIARY_MOUSE_BUTTON )  action = setup->MiddleMouseAction();
   
   if ( action == P_SCROLLING ) {
         float toScrollX = -(ad.x - oldMousePos.x);
         float toScrollY = -(ad.y - oldMousePos.y);
         float newX = ScrollBar( B_HORIZONTAL ) -> Value() + toScrollX;
         float newY = ScrollBar( B_VERTICAL   ) -> Value() + toScrollY;
         float maxX, minX;
         float maxY, minY;
         ScrollBar( B_HORIZONTAL ) ->GetRange( &minX, &maxX );
         ScrollBar( B_VERTICAL  ) ->GetRange( &minY, &maxY );
         if ( newX < 0 ) newX = 0;
         if ( newY < 0 ) newY = 0;
         if ( newX > maxX) newX = maxX;
         if ( newY > maxY) newY = maxY;
         ScrollTo( newX, newY );
         return;
   }
   
   if ( action == P_CLIPPING ) {
         DrawClipping();
         clippingRegion.right = ad.x;
         clippingRegion.bottom = ad.y;
         DrawClipping();
         return;
   }

   if ( action == P_DRAGGING ) 
      if (thePic != NULL) {
          BBitmap *dragImage;
          BMessage *msg;
          bool negotiated = setup->DragMode();
           
          // here comes the CTRL keyboard modifier
          if ( (modifiers() & B_CONTROL_KEY) == B_CONTROL_KEY ) negotiated = ! negotiated;
          //  and now it's finished.
       
          if (negotiated) {
            setup->SetNegotiationMessage(   ((PeekApp*)be_app)->SetupNegotiationMessage(setup->DragAction())      );
            msg = new BMessage (*setup->NegotiationMessage());
            BMimeType k("image");          
            dragImage = new BBitmap(BRect(0,0,31,31),B_CMAP8);
            msg->AddString("source","Peek");
            if (k.GetIcon(dragImage,B_LARGE_ICON) == B_OK) DragMessage(msg,dragImage, B_OP_ALPHA, BPoint(16,16));
            delete msg;
          } else {
             msg = new BMessage(B_SIMPLE_DATA);
             msg->AddRef("refs",theRef);
      
             if (setup->DragAction() != 0) msg->AddInt32("be:actions",setup->DragAction() );
           
             BMimeType k("image");          
             dragImage = new BBitmap(BRect(0,0,31,31),B_CMAP8);
             msg->AddString("source","Peek");
             if (k.GetIcon(dragImage,B_LARGE_ICON) == B_OK) DragMessage(msg,dragImage, B_OP_ALPHA, BPoint(16,16));
            delete msg;
          }  
   }
   
   
}

// -----------------------------------------------------------------
//            SOME IMPORTANT DRAG & DROP STUFF HERE
//      Just the received stuff so that a person can drop
//                   stuff into the viewer
//            (mainly copied from the old PictureViewer.cpp)
// -----------------------------------------------------------------


bool PictureViewer::SupportedFormat(const char *s) {
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



void PictureViewer::HandleMimeData(BMessage *e) {
    ssize_t numBytes;
    type_code type;
    const void *data;
    const char *m_type;
    if (e->GetInfo( B_MIME_TYPE, 0, (char**)&m_type, &type) != B_OK) return;

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
    ((WindowPeek*)Window())->LoadImage(bmp);
    return;  
}

void PictureViewer::HandleSimpleData(BMessage *e) 
{
    WindowPeek* motherWindow = ((WindowPeek*)Window());
                                           // CASE 1:  we have files
   
     entry_ref *ref = new entry_ref;
     if ( e->FindRef("refs",ref) == B_OK ) 
     {
                BEntry *file = new BEntry(ref, true);
                if (motherWindow->IsType( file, "image" ))  motherWindow->LoadImage(file);
                delete file;
     }
     
    delete ref;


    // CASE 2:   we don't have a file.

      // make sure we're not wasting our time
//     if (  strcmp(e->FindString("source"),"Peek") == 0 ) {
//             return;
//     }    

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


void PictureViewer::HandleCopyTarget(BMessage *e) {
  entry_ref targetRef;
   
  BBitmap *toSend;
  if (clipping == true) toSend = GetClipping();
                   else toSend = thePic;
  


  // memory or file transfer ?
  if (e->FindRef("directory",&targetRef) == B_OK)  {

      BDirectory *targetDir = new BDirectory( &targetRef );
      if (targetDir->InitCheck() == B_OK) {
         BFile *targetFile = new BFile(targetDir,e->FindString("name"),B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
         if (targetFile->InitCheck() == B_OK) {

          BBitmap *dragImage = NULL;
          BBitmapStream *strm;
          strm = new BBitmapStream(toSend);

          translator_id trans_id;
          uint32 type;

          GetTranslatorInfo(e->FindString("be:filetypes"),&trans_id,&type);
          BTranslatorRoster::Default()->Translate(trans_id,strm,NULL,targetFile,type);
          BNodeInfo *ni = new BNodeInfo(targetFile);
          ni->SetType(e->FindString("be:filetypes"));
          if (dragImage == NULL) strm->DetachBitmap(&toSend);
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
     imageSrc = new BBitmapStream(toSend);

     BMessage *package = new BMessage(B_MIME_DATA);
     translator_id trans_id;
     uint32 type;

     GetTranslatorInfo(e->FindString("be:types"),&trans_id,&type);
     if (BTranslatorRoster::Default()->Translate( trans_id, imageSrc, NULL, data, type ) != B_OK) return;
     package->AddData( e->FindString("be:types"),  B_MIME_DATA, data->Buffer(), data->BufferLength());
     e->SendReply(package);        

     imageSrc->DetachBitmap(&toSend);

     delete data;
     delete imageSrc;
     delete package;
  } 

 if (clipping) delete toSend;

}

void PictureViewer::GetTranslatorInfo(const char *m, translator_id *t, uint32 *t2) {
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



void PictureViewer::HandleTrashTarget(BMessage *e) { }



// ------------------------------------------------------------------------------------
//                                   MessageReceived 
//                      Merely for the drag & drop abilities here
// ------------------------------------------------------------------------------------
void PictureViewer::MessageReceived(BMessage* e) {
   switch (e->what) {
     case B_SIMPLE_DATA: HandleSimpleData(e); break;                          
     case B_MIME_DATA: HandleMimeData(e); break;                          
     case B_TRASH_TARGET: HandleTrashTarget(e); break;                          
     case B_COPY_TARGET: HandleCopyTarget(e); break;                          
     default: BView::MessageReceived(e); break;
   }
}
       
