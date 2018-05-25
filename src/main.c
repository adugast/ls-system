// https://www.x.org/releases/X11R7.6/doc/libX11/specs/libX11/libX11.html
// http://neuron-ai.tuke.sk/hudecm/Tutorials/C/special/xlib-programming/xlib-programming-2.html

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get_info_api.h"


#define WIN_NAME        "Show System Info (X11)"
#define WIN_NAME_MIN    "Show System Info"

/* get the geometry of the default screen for our display. */
/*screen_num = DefaultScreen(display);
display_width = DisplayWidth(display, screen_num);
display_height = DisplayHeight(display, screen_num);
*/
// XSetWMProperties(cdisDisplay, Handle, &xtpWinName, &xtpIconName, ArgC, ArgV, xshSize, xwmhHints, xchClass)

//struct X11_ctx {
Display *display;
Window win;
GC gc;
//};


// open xserver connection
static int open_Xserver(const char *display_name)
{
    display = XOpenDisplay(display_name);
    if (display == NULL) {
        fprintf(stderr, "%s: cannot connect to X server '%s'\n", "toto", display_name);
        exit(1);
    }

    return 0;
}


//close xserver connection
static int close_Xserver()
{
    XCloseDisplay(display);
    return 0;
}


/////////////////////////////////////////////////////////////////////


static int set_window_icon()
{
//    #include "icon.bmp"
    /* include the definition of the bitmap in our program. */

    /* pointer to the WM hints structure. */
  //  XWMHints* win_hints;

    /* load the given bitmap data and create an X pixmap containing it. */
    /*Pixmap icon_pixmap = XCreateBitmapFromData(display,
            win,
            icon_bitmap_bits,
            icon_bitmap_width,
            icon_bitmap_height);
    if (!icon_pixmap) {
        fprintf(stderr, "XCreateBitmapFromData - error creating pixmap\n");
        exit(1);
    }
*/
    /* allocate a WM hints structure. */
/*    win_hints = XAllocWMHints();
    if (!win_hints) {
        fprintf(stderr, "XAllocWMHints - out of memory\n");
        exit(1);
    }
*
    /* initialize the structure appropriately. */
    /* first, specify which size hints we want to fill in. */
    /* in our case - setting the icon's pixmap. */
 //   win_hints->flags = IconPixmapHint;
    /* next, specify the desired hints data.           */
    /* in our case - supply the icon's desired pixmap. */
   // win_hints->icon_pixmap = icon_pixmap;

    /* pass the hints to the window manager. */
   // XSetWMHints(display, win, win_hints);

    /* finally, we can free the WM hints structure. */
   // XFree(win_hints);
}

struct x11_handle {
    int win_x, win_y; // Location of the window, relative to its parent window.
    unsigned int width, height; // width and height of the window (in pixels).
    unsigned int border_width; // width of the window's border
    unsigned long border_color, background_color; // colors
};


static Window create_window(struct x11_handle *hdl)
{
    return XCreateSimpleWindow(display,
            RootWindow(display, 0),
            hdl->win_x,
            hdl->win_y,
            hdl->width,
            hdl->height,
            hdl->border_width,
            hdl->border_color,
            hdl->background_color);
}


static int set_window_properties(const char *window_name, const char *window_name_minimized)
{
    XSetStandardProperties(display, win, window_name, window_name_minimized, None, NULL, 0, NULL);
    return 0;
}

static int set_window_events(Window w, long event_mask)
{
    return XSelectInput(display, win, event_mask);
}

static int draw_string(const char *string, int pos_x, int pos_y)
{
    return XDrawString(display, win, gc, pos_x, pos_y, string, strlen(string));
}


/////////////////////////////////////////////////////////////////////


static int initialize()
{
    // launch X server connection
    open_Xserver(NULL);

    // set window properties
    struct x11_handle hdl = {
        .win_x = 1500,
        .win_y = 1500,
        .width = 300,
        .height = 300,
        .border_width = 5,
        .border_color = 0x000000,
        .background_color = 0x696969
    };

    // create window context
    // should I do win = create_window();
    win = create_window(&hdl);
    if (win == -1) {
        fprintf(stderr, "cannot create window\n");
        return -1;
    }

    gc = XCreateGC(display, win, 0, 0);
//    XSetBackground(display, gc, 0xFFFFFF);
    XSetForeground(display, gc, 0xDCDCDC); // set police color

    set_window_events(win, ExposureMask|ButtonPressMask|KeyPressMask|StructureNotifyMask);
    set_window_properties(WIN_NAME, WIN_NAME_MIN);

/*
    XTextProperty ppt;
    XSetWMName(display, win, &ppt);

    XTextProperty icon;
    XSetWMIconName(display, win, &icon);
*/
    return 0;
}


static int terminate()
{
    printf("cleanup done\n");
    XUnmapWindow(display, win);
    XFreeGC(display, gc);
    XDestroyWindow(display, win);
    // close X server connection
    close_Xserver();
    exit(0);
}


static int loop_event()
{
    // put window on sceen
    XMapWindow(display, win);
    XSync(display, False);

    int info_pos_x = 5;

    KeySym key;
    char text[255];
    XEvent event;

    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, win, &wmDeleteMessage, 1);

    int display_width = DisplayWidth(display, 0) / 2;
    int display_height = DisplayHeight(display, 0) / 2;
    XMoveWindow(display, win, display_width, display_height);

    while (1) {

        XNextEvent(display, &event);

        switch (event.type) {
            case ConfigureNotify:
                printf("Window moved or resized!\n");
                break;
            case ButtonPress:
                printf("Mouse button %d pressed!\n", event.xbutton.button);
                break;
            case KeyPress:
                if (XLookupString(&event.xkey,text,255,&key,0) == 1) {
                    if (text[0]=='q') {
                        terminate();
                    }
                                                                }
                printf("You pressed the %c key!\n",text[0]);
                break;
            case DestroyNotify:
                printf("Window killed!\n");
            case ClientMessage:
                if (event.xclient.data.l[0] == wmDeleteMessage) {
                    printf("yolo\n");
                    terminate();
                }
                break;
            default:
                printf("Unknown event: %d (discarding)\n", event.type);
                break;
        }

        draw_string("System Info:", info_pos_x, 15);

        char username[256] = {0};
        if (usr_get_name(username, sizeof(username)) != -1) {
            draw_string("Username: ", info_pos_x, 30);
            draw_string(username, info_pos_x + 60, 30);
        }


        char *ptr;
        uts_get_sysname(&ptr);
        draw_string("System name:", info_pos_x, 45);
        draw_string(ptr, info_pos_x + 76, 45);
        free(ptr);

        uts_get_release(&ptr);
        draw_string("Release:", info_pos_x, 60);
        draw_string(ptr, info_pos_x + 54, 60);
        free(ptr);

    }

    return 0;
}


int main()
{
    initialize();
    loop_event();
    terminate();
    return 0;
}

