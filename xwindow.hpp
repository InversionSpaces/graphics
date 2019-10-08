#pragma once

#include <stdexcept>
#include <vector>
#include <cassert>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
}

#include "linalg.hpp"

typedef vec<uint8_t, 4> bgracolor_t;
typedef vec<uint16_t, 2> resolution_t;
typedef vec<uint16_t, 2> pixelcoords_t;

class XWindow
{
public:
    ~XWindow();
    XWindow();
    XWindow(XWindow const &) = delete;
    XWindow(XWindow &&) = delete;

    int width() const noexcept;
    int height() const noexcept;

    void update() noexcept;
    void clear() noexcept;
    
    bgracolor_t& operator[](const pixelcoords_t& coords);

private:
    Display *display;
    int     screen;
    Window  window;
    GC      gc;
    XImage  *image;
    
    resolution_t resolution;
    std::vector<bgracolor_t> pixels;
};

inline int      XWindow::width() const noexcept {return resolution.w;}

inline int      XWindow::height() const noexcept {return resolution.h;}

inline void     XWindow::clear() noexcept 
{
    wchar_t* buffer = reinterpret_cast<wchar_t *>(pixels.data());
    std::wmemset(buffer, 0xff000000, resolution.w * resolution.h);
}

inline void     XWindow::update() noexcept 
{
    XPutImage
    (
        display, 
        window, 
        gc, 
        image, 
        0, 
        0, 
        0, 
        0, 
        resolution.w, 
        resolution.h
    );
}

bgracolor_t& XWindow::operator[](const pixelcoords_t& coords)
{
    assert(coords.x < resolution.w);
    assert(coords.y < resolution.h);
    
    return pixels[resolution.w * coords.y + coords.x];
}

inline XWindow::XWindow()
{
    display = XOpenDisplay(getenv("DISPLAY"));
    if(display == NULL)
        throw std::runtime_error("could not open x11 display");
        
    screen = DefaultScreen(display);
    
    window = XCreateSimpleWindow
    (
        display,
        DefaultRootWindow(display),
        0,
        0,
        640,
        480,
        0,
        WhitePixel(display, screen),
        BlackPixel(display, screen)
    );
    
    Atom wm_state      = 
        XInternAtom(display, "_NET_WM_STATE", true);
    Atom wm_fullscreen = 
        XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);
        
    XChangeProperty(
        display, 
        window, 
        wm_state, 
        XA_ATOM, 
        32, 
        PropModeReplace, 
        (unsigned char *)&wm_fullscreen, 
        1
    );

    resolution.w = XWidthOfScreen (ScreenOfDisplay(display, screen));
    resolution.h = XHeightOfScreen(ScreenOfDisplay(display, screen));

    pixels.resize(resolution.w * resolution.h);
    pixels.shrink_to_fit();

    gc = XCreateGC(display, window, 0, 0);

    XSelectInput
    (
        display,
        window,
        ExposureMask | ButtonPressMask | KeyPressMask
    );
    
    image = XCreateImage
    (
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
        ZPixmap,
        0,
        reinterpret_cast<char *>(pixels.data()),
        resolution.w,
        resolution.h,
        32,
        0
    );
    
    XClearWindow(display, window);
    XMapRaised(display, window);
    
    update();
}

inline XWindow::~XWindow()
{
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
