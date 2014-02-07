#include "PeekApp.h"

#include "ConfigureClass.h"

int main(void) {
    new PeekApp();
    be_app->Run();
    delete be_app;
    return 0;
} 