#ifndef WX_INCLUDES_H
#define WX_INCLUDES_H

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "wx/timer.h"
#include "wx/glcanvas.h"
#include "wx/math.h"
#include "wx/toolbar.h"
#include "wx/splitter.h"
#include "wx/image.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmprint.h"
#include "wx/spinctrl.h"
#include "wx/textctrl.h"
#include "wx/colordlg.h"
#include "wx/file.h"


#endif