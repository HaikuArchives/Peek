#ifndef PEEK_APP_H
#define PEEK_APP_H

#include <Application.h>
#include "WindowPeek.h"
#include "Setup.h"
#include "constants.h"
#include <string.h>
#include <Entry.h>
#include <TranslatorRoster.h>

class PeekApp : public BApplication {

  public:
    PeekApp();
    BMessage* SetupNegotiationMessage(int32);
    void RefsReceived(BMessage* );
    void ReadyToRun();
    void SaveSetup();
    bool LoadSetup();
  private:
    void SetDefaults();
    void LoadLanguage();
    void ShowHTML();
    virtual void AboutRequested();
    virtual void MessageReceived(BMessage*);
    virtual void Quit();

    WindowPeek *mainWindow;
    Setup *setup;
    Language *words;
};


#endif
