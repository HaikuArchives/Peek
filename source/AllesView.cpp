#include "AllesView.h"

// AllesView
//
//   The purpose of AllesView is to capture any keystrokes before they
//   go to their intended views and then send them in the corret directions
//   according to the values in the Setup object.
//
//   It also captures:  B_MOUSE_WHEEL_CHANGED
//
//
//
//
//

#include "WindowPeek.h"


AllesView::AllesView(BRect R, char* name)
: BView(R,name, B_FOLLOW_ALL_SIDES, B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS)
{
 
}



// MessageReceived -- Handle the stuff, baby. yeah.
void AllesView::MessageReceived(BMessage *e) {
   switch ( e->what ) {
      case B_KEY_UP:
             KeyUp( e->FindString("bytes"), strlen( e->FindString("bytes") ) );
             return;
      case  B_MOUSE_WHEEL_CHANGED:
            HandleMouseWheel(e->FindFloat("be:wheel_delta_y")); 
            return;
      default:  BView::MessageReceived(e); 
   }
}


// KeyUp   --  Capture all key presses before they can do any damage
//             and handle them accordingly;  This cuts down a lot on
//             source code further down the hierarchy.
//
void AllesView::KeyUp(const char* bytes, int32 numbytes) {

     // deal with simple characters and then return;
   if (numbytes == 1) {
          switch (*bytes) {
            case  B_SPACE:  HandleSpacebar(); break;
            case B_ESCAPE:  HandleEscape();   break;
          }
       return;   
   }


   BView::KeyUp( bytes, numbytes);
}


// --------------------------- HANDLERS --------------------------------



void AllesView::HandleSpacebar()  {
     WindowPeek* motherWindow = ((WindowPeek*)Window());   
     motherWindow->filePane->SelectNextImage(1);
}

//  Escape 
//       pressing escape now helps everything.
//       Hooray.  double-click on picture. esc. keep going.

void AllesView::HandleEscape()  {
     if (modifiers() != 0) return;
     
     WindowPeek* motherWindow = ((WindowPeek*)Window());
     
     if ( motherWindow->setup->Sliding() ) {
           motherWindow->SetSliding(false);
     }
//     else if ( motherWindow->setup->CompactMode() ) {
//           motherWindow->SetCompactMode( false );
//     }
//     else if ( motherWindow->setup->FullScreen() ) {
//           motherWindow->SetFullScreen( false );
//     }
     else {
            motherWindow->PostMessage( B_QUIT_REQUESTED );
           }
}


// HandleMouseWheel -- This should handle the mouse wheel according to
//                     Setup->WheelMouseAction()
//
void AllesView::HandleMouseWheel(int32 ychange) {
  if (ychange == 0) return;
  WindowPeek* motherWindow = ((WindowPeek*)Window());

  switch (motherWindow->setup->WheelMouseAction()) {
	case P_WHEEL_SCROLL_LIST:
		if ( motherWindow->filePane->SelectNextImage( ychange ) )
			motherWindow->filePane->fileList->ScrollToSelection();
		break;
	case P_WHEEL_SCROLL_IMAGE:
		if (ychange > 0)
			motherWindow->imagePane->ScrollBy( 0, 20 );
		else if (ychange < 0)
			motherWindow->imagePane->ScrollBy( 0, -20 );
		break;
	case P_WHEEL_ZOOM_IMAGE:
		float zoom = motherWindow->imagePane->GetZoom();

		if (ychange > 0)
			zoom *= 0.75;
		else
			zoom /= 0.75;

		motherWindow->imagePane->SetZoom(zoom);
		break;
  }
}
