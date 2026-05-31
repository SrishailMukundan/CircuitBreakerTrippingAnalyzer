#ifndef MICRO_PROJECT_H
#define MICRO_PROJECT_H

#include <U8g2lib.h>

class MicroProject {
public:
    // Virtual destructor ensures proper cleanup when switching projects
    virtual ~MicroProject() {}

    // Every project must implement its own setup logic (sensors, pins, variables)
    virtual void projectSetup() = 0;

    // Every project must implement its own background looping logic
    virtual void projectLoop(String serverUrl) = 0;

    // Every project must know how to render its own graphics onto the display glass
    virtual void projectRender(U8G2& u8g2) = 0;
};

#endif // MICRO_PROJECT_H