#ifndef _WX_ISOSURF_H_
#define _WX_ISOSURF_H_
#include "MyApp.h"

class GLCanvas: public wxGLCanvas
{
public:
    GLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,        const wxPoint& pos = wxDefaultPosition,        const wxSize& size = wxDefaultSize, long style = 0,        const wxString& name = _T("GLCanvas"), int *gl_attrib = NULL);

   ~GLCanvas(){};

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
	void MyRefresh();
	void SetDefaultCursor();
	void OnFullScreen();


	
	wxWindow *par;
	bool is_timer_call;
private:
	
    DECLARE_EVENT_TABLE()
};



#endif 

