#include "CT/CT.h"

#include "wxIncludes.h"

//#include <GL/gl.h>
//#include <GL/glu.h>

#include "MainFrame.h"
#include "GLCanvas.h"
#include "str.h"
#include "Mouse.h"

#include "event_proc.h"


BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
    EVT_SIZE(GLCanvas::OnSize)
    EVT_PAINT(GLCanvas::OnPaint)
    EVT_KEY_DOWN(GLCanvas::OnKeyDown)
	EVT_KEY_UP(GLCanvas::OnKeyUp)
    EVT_MOUSE_EVENTS(GLCanvas::OnMouseEvent)
END_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size, long style,    const wxString& name, int* gl_attrib)    : 
//		wxGLCanvas(parent, id, pos, size, style, name, gl_attrib)
wxGLCanvas(parent, id,gl_attrib, pos, size, style, name)
		
		
{
	MyApp::GLContext = new wxGLContext(this);
	if(parent)		parent->Show(true);
	SetCurrent(*MyApp::GLContext);
	CT::initIVP();
	MyRefresh();

	glDrawBuffer(CT::gl_back_buffer_target);
	glReadBuffer(CT::gl_back_buffer_target);

	par = parent;

   
}
void GLCanvas::MyRefresh()
{
	Refresh(false);
	


}

void GLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    //if (!GetContext()) return;
    SetCurrent(*MyApp::GLContext);
	static int times_accum = 0;

//OnSize(wxSizeEvent());
	CT::DoRegular();
	int RenderingType = CT::RenderingType; //0-permanent  1-once changed  2-permanent accum  3-draw small when changed

	static int rendered_times = 0;
	if(RenderingType==0)//wxLogStatus(MyApp::frame, _T("fps %2.1f"),  CT::fps);
	{
		MyApp::frame->SetStatusText("fps "+str::ToString(CT::fps),1);
	}
	else
	{
		if(CT::need_rerender || (RenderingType==2 && times_accum<CT::frames_accum_limit))rendered_times++;
		MyApp::frame->SetStatusText("frame "+str::ToString(rendered_times),1);
	}

	if(RenderingType==0)
	{
		CT::FullDraw();
		glFlush();
		SwapBuffers();
	}else
	if(RenderingType==1)
	{
		
		if(CT::need_rerender)
		{
			CT::FullDraw();
			glFlush();
			SwapBuffers();
		}else{}
		
	}else
	if(RenderingType==2)
	{
		if(CT::need_rerender)
		{
			glClear(GL_ACCUM_BUFFER_BIT);
			times_accum=0;
		}
		
		
		times_accum++;
		if(times_accum<CT::frames_accum_limit)
		{
		CT::FullDraw();
		glFlush();
		glAccum(GL_MULT, (times_accum-1)/float(times_accum));
		glAccum(GL_ACCUM, 1.0/times_accum);
		if(CT::stereo_on==1 || CT::stereo_on==2)
		{
			CT::FullDraw();
			glFlush();
			glAccum(GL_ACCUM, 1.0/times_accum);
		}
		glAccum(GL_RETURN, 1.0);
		
		SwapBuffers();
		}else
		{
			glAccum(GL_RETURN, 1.0);
			SwapBuffers();
		}
	}else
	if(RenderingType==3)
	{
		static int free_times=20;
		free_times--;
		if(CT::need_rerender)
		{
			free_times=10;
			CT::draw_small_data=0;
			CT::FullDraw();			glFlush();			SwapBuffers();
		}else
		{
			if(!free_times)
			{
				CT::draw_small_data=0;
				CT::FullDraw();			glFlush();			SwapBuffers();
				CT::draw_small_data=1;
			}//else if(!is_timer_call){		SwapBuffers();SwapBuffers();}
		}
			
	}
	CT::need_rerender=0;

	
}

void GLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    int w, h;
    GetClientSize(&w, &h);
//	wxGLContext *c = GetContext();
  //  if (c)
    {
	
        SetCurrent(*MyApp::GLContext);
		CT::Resize(w,h);
		MyRefresh();
    }
	
}
void GLCanvas::OnFullScreen()
{
		static wxFrame *ff=0;
		static bool bb=1;
		static wxSize old_sz;
		if(bb)
		{
			old_sz = GetSize();
			wxSize sz = wxGetDisplaySize();
			ff  = new wxFrame(MyApp::frame,0,"",wxPoint(0,0), sz,wxFULLSCREEN_ALL);
			par->RemoveChild(this);
			Reparent(ff);
			ff->Show();
			this->SetSize(sz);
		}else
		{
			ff->RemoveChild(this);
			Reparent(par);
			this->SetSize(old_sz);
			
			ff->Close();
		}
		bb=!bb;
		CT::need_rerender=1;
}
void GLCanvas::OnKeyDown(wxKeyEvent& event)
{
	CT::KeyButton(event.GetKeyCode(), 1);
    switch( event.GetKeyCode() )
    {
	case WXK_HOME:
		MyApp::frame->OnGoHome(wxCommandEvent());
		break;
    case WXK_ESCAPE:
      //  wxTheApp->ExitMainLoop();
		MyApp::frame->OnFullScreen(wxCommandEvent());

        return;
	case 32:
		MyApp::frame->OnRMPicView(wxCommandEvent());
		
		break;
	}


}

void GLCanvas::SetDefaultCursor()
{
	wxCursor c = wxCursor(wxCURSOR_ARROW);
	switch(CT::MouseButtonFunction)
	{
		case CT::MouseBC:			c = wxCursor(wxImage("Images/BC.png",wxBITMAP_TYPE_PNG));break;
		case CT::MouseRotate:		c = wxCursor(wxImage("Images/rotate.png",wxBITMAP_TYPE_PNG));break;
		case CT::MouseTranslate:	c = wxCursor(wxImage("Images/translate.png",wxBITMAP_TYPE_PNG));break;
		case CT::MouseZoom:			c = wxCursor(wxImage("Images/zoom.png",wxBITMAP_TYPE_PNG));break;
	}
	SetCursor(c);
}
void GLCanvas::OnKeyUp(wxKeyEvent& event)
{
	CT::KeyButton(event.GetKeyCode(), 0);
}

void GLCanvas::OnMouseEvent(wxMouseEvent& event)
{
	
    static float last_x, last_y;
	this->SetFocus();

	if(event.GetX()!=last_x || event.GetY()!=last_y)
	{
		CT::MouseMove(event.GetX(),event.GetY());

		
	}
	

	if(event.IsButton())
	{
		int bt = 0;
		int state = event.LeftDown() || event.RightDown()||event.MiddleDown();
		//this->SetFocus();
		if(event.RightDown() || event.RightUp()) bt=1;
		if(event.MiddleDown()||event.MiddleUp()) bt=2;
		
		CT::MouseButton(bt,state);
	}
	
	if(event.m_wheelRotation)
	{
		//this->SetFocus();
		CT::MouseWheel(event.m_wheelRotation);
	}
		
			if(!event.LeftIsDown() && CT::mouse.btn&1)
			CT::MouseButton(0,0);
		if(!event.RightIsDown() && CT::mouse.btn&2)
			CT::MouseButton(1,0);
		if(!event.MiddleIsDown() && CT::mouse.btn&4)
			CT::MouseButton(2,0);


}
