#pragma once

#include <stdexcept>
#include <vector>
#include <cassert>
#include <cinttypes>

extern "C"
{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
}

#include "linalg.hpp"

class XWindow
{
public:
    ~XWindow();
    XWindow();
    XWindow(XWindow const &) = delete;
    XWindow(XWindow &&) = delete;

    uint16_t width() const noexcept;
    uint16_t height() const noexcept;
    resolution_t resolution() const noexcept;

    void update() noexcept;
    void clear() noexcept;
    
    bgracolor_t& operator[](const pixelcoords_t& coords);

private:
    Display *display;
    int     screen;
    Window  window;
    GC      gc;
    XImage  *image;
    
    resolution_t res;
    bgracolor_t* pixels;
};

inline resolution_t XWindow::resolution() const noexcept 
{
    return res;
}

inline uint16_t XWindow::width() const noexcept 
{
    return res.w;
}

inline uint16_t XWindow::height() const noexcept 
{
    return res.h;
}

inline void     XWindow::clear() noexcept 
{
    wchar_t* buffer = reinterpret_cast<wchar_t *>(pixels);
    std::wmemset(buffer, 0xff000000, res.w * res.h);
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
        res.w, 
        res.h
    );
}

bgracolor_t& XWindow::operator[](const pixelcoords_t& coords)
{
    assert(coords.x < res.w);
    assert(coords.y < res.h);
    
    return pixels[res.w * (res.h - coords.y - 1) + coords.x];
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

    res.w = XWidthOfScreen (ScreenOfDisplay(display, screen));
    res.h = XHeightOfScreen(ScreenOfDisplay(display, screen));

    pixels = reinterpret_cast<bgracolor_t*>(
		calloc(res.w * res.h, sizeof(bgracolor_t))
		);

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
        reinterpret_cast<char*>(pixels),
        res.w,
        res.h,
        32,
        0
    );
    
    XClearWindow(display, window);
    XMapRaised(display, window);
}

inline XWindow::~XWindow()
{
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
