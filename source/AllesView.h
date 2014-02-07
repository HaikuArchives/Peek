#ifndef ALLES_VIEW_H
#define ALLES_VIEW_H


#include <View.h>

class AllesView : public BView {

  public:
    AllesView( BRect, char* );
    virtual void MessageReceived(BMessage *);
    
  private:
    virtual void KeyUp(const char*, int32);
    
    // Handlers
    void HandleSpacebar();
    void HandleEscape();
    void HandleMouseWheel( int32 );
    
    
};

#endif