#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"


//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "tf2D.h"
#include "drawtext.h"
#include "Img3DProc.h"
#include "CPU_VD.h"


#define TF2D_MARGIN 15


BEGIN_EVENT_TABLE(TF2D_window, wxGLCanvas)
    EVT_SIZE(TF2D_window::OnSize)
    EVT_PAINT(TF2D_window::OnPaint)
    EVT_KEY_DOWN(TF2D_window::OnKeyDown)
	EVT_KEY_UP(TF2D_window::OnKeyUp)
    EVT_MOUSE_EVENTS(TF2D_window::OnMouseEvent)
    EVT_SIZE(TF2D_window::OnSize)
END_EVENT_TABLE()


TF2D_window::TF2D_window(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size, long style,    const wxString& name, int* gl_attrib)    : 
//		wxGLCanvas(parent, id, pos, size, style, name, gl_attrib)
		wxGLCanvas(parent, MyApp::GLContext, id ,  pos, size ,  style, name,MyApp::gl_attributes)

		
{
	scale = 500;
	center = 0;
	sel_obj=-1;
	selected_obj=-1;

	cur_action=TF2D_ACTION_ARROW;
	cur_creation=-1;
	
	if(parent)
		parent->Show(true);
	SetCurrent();
	
	MyRefresh();


	
}

void TF2D_window::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    GetClientSize(&width, &height);

	wxGLContext *c = GetContext();
    if (c)
    {
	
        SetCurrent();
		glViewport(0, 0, (GLint) width, (GLint) height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width, height,0);
		glMatrixMode(GL_MODELVIEW);
		MyRefresh();
    }

}
void TF2D_window::InitGL()
{
//	glClearColor(0,0,0,0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineStipple(1,15+(15<<8));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void TF2D_window::MyRefresh()
{

	Refresh(false);
}


void TF2D_window::OnKeyDown(wxKeyEvent& event)
{
	CT::KeyButton(event.GetKeyCode(), 1);
    switch( event.GetKeyCode() )
    {
    case WXK_ESCAPE:
            return;
	case 32:
		break;
	}
}

void TF2D_window::OnKeyUp(wxKeyEvent& event)
{
	//CT::KeyButton(event.GetKeyCode(), 0);
}

/////

BEGIN_EVENT_TABLE(tf2dInfoWindow, wxPanel)
	EVT_BUTTON(TF2D_ACTION_ARROW, tf2dInfoWindow::OnToggleArrow)
	EVT_BUTTON(TF2D_ACTION_TRANSLATE, tf2dInfoWindow::OnToggleTranslate)
	EVT_BUTTON(TF2D_ACTION_ZOOM, tf2dInfoWindow::OnToggleZoom)
	EVT_BUTTON(TF2D_ACTION_ADD_ISO, tf2dInfoWindow::OnToggleAddIso)
	EVT_BUTTON(TF2D_ACTION_ADD_QUAD, tf2dInfoWindow::OnToggleAddQuad)
	EVT_BUTTON(TF2D_ACTION_DELETE, tf2dInfoWindow::OnBtnDelete)
	EVT_BUTTON(TF2D_ACTION_COLOR, tf2dInfoWindow::OnBtnColor)
	EVT_BUTTON(TF2D_SHOW_HISTOGRAM, tf2dInfoWindow::OnBtnShowHistogram)


END_EVENT_TABLE()

tf2dInfoWindow::tf2dInfoWindow(wxWindow* parent)
        : wxPanel(parent)
{

	tf = new TF2D_window(this,wxID_ANY);//, wxSize(200,600), 0);
	
//#define IMG(fn) wxBitmap(wxString("Images/")+wxString(fn)+wxString(".png"),wxBITMAP_TYPE_PNG)
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)



//	tbtn_arrow = new wxBitmapButton(this,TF_ACTION_ARROW,IMG("arrow"));
//	tbtn_translate = new wxBitmapButton(this,TF_ACTION_TRANSLATE,IMG("ml_translate"));
//	tbtn_zoom = new wxBitmapButton(this,TF_ACTION_ZOOM,IMG("mr_zoom"));
	tbtn_add_iso = new wxBitmapButton(this,TF2D_ACTION_ADD_ISO,IMG("add_iso"));
	tbtn_add_quad = new wxBitmapButton(this,TF2D_ACTION_ADD_QUAD,IMG("add_quad"));
	tbtn_show_histogram = new wxBitmapButton(this,TF2D_SHOW_HISTOGRAM,IMG("show_hist"));
	wxButton *btn_del = new wxBitmapButton(this,TF2D_ACTION_DELETE,IMG("delete"));
	wxButton *btn_color = new wxBitmapButton(this,TF2D_ACTION_COLOR,IMG("color"));

//	tbtn_arrow->SetToolTip(MY_TXT("Objects' edition","Редактирование объектов"));
//	tbtn_translate->SetToolTip(MY_TXT("Translation","Перенос"));
//	tbtn_zoom->SetToolTip(MY_TXT("Zoom","Масштабирование"));

	tbtn_add_iso->SetToolTip(MY_TXT("Add isosurface","Добавить изоповерхность"));
	tbtn_add_quad->SetToolTip(MY_TXT("Add quad","Добавить прямоугольник"));
	btn_del->SetToolTip(MY_TXT("Delete","Удалить"));
	btn_color->SetToolTip(MY_TXT("Color","Задать цвет"));
	tbtn_show_histogram->SetToolTip(MY_TXT("Histogram","Гистограмма"));

	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz1 = new wxBoxSizer(wxVERTICAL);
//	wxSizer* sz11 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz12 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz13 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz14 = new wxBoxSizer(wxHORIZONTAL);

//	sz11->Add(tbtn_arrow,0,wxSTRETCH_NOT);
//	sz11->Add(tbtn_translate,0,wxSTRETCH_NOT);
//	sz11->Add(tbtn_zoom,0,wxSTRETCH_NOT);
	sz12->Add(tbtn_add_iso,0,wxSTRETCH_NOT);
	sz12->Add(tbtn_add_quad,0,wxSTRETCH_NOT);
	sz13->Add(btn_color,0,wxSTRETCH_NOT);
	sz13->Add(btn_del,0,wxSTRETCH_NOT);
	sz14->Add(tbtn_show_histogram,0,wxSTRETCH_NOT);
//	sz1->Add(sz11);
	sz1->Add(sz12);
	sz1->Add(sz13);
	sz1->Add(sz14);


	tf->par = this;
	sz->Add(tf,1,wxGROW);
	sz->Add(sz1,0,wxSTRETCH_NOT);
	SetSizer(sz);

}
const wxColor wxColor0(255,255,255);
const wxColor wxColor1(111,111,0);
void tf2dInfoWindow::SetButtonValue(wxBitmapButton *b,bool v)
{
	b->SetBackgroundColour(v ? wxColor1 : wxColor0);
}
bool tf2dInfoWindow::GetButtonValue(wxBitmapButton *b)
{
	return (b->GetBackgroundColour() == wxColor1);
}

void tf2dInfoWindow::OnToggleArrow(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,1);
	SetButtonValue(tbtn_translate,0);
	SetButtonValue(tbtn_zoom,0);
	*/

	tf->cur_action = TF2D_ACTION_ARROW;
	
}
void tf2dInfoWindow::OnToggleTranslate(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,0);
	SetButtonValue(tbtn_translate,1);
	SetButtonValue(tbtn_zoom,0);
	*/
	tf->cur_action = TF2D_ACTION_TRANSLATE;

}
void tf2dInfoWindow::OnToggleZoom(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,0);
	SetButtonValue(tbtn_translate,0);
	SetButtonValue(tbtn_zoom,1);
	*/
	tf->cur_action = TF2D_ACTION_ZOOM;


}
void tf2dInfoWindow::OnToggleAddIso(wxCommandEvent& event)
{
	SetButtonValue(tbtn_add_quad,0);
	bool b = GetButtonValue(tbtn_add_iso);
	SetButtonValue(tbtn_add_iso,!b);
	tf->cur_creation = !b ? TF2D_ACTION_ADD_ISO:-1;
	//OnToggleArrow(event);
	MyApp::frame->OnMouseArrow(event);
}
void tf2dInfoWindow::OnToggleAddQuad(wxCommandEvent& event)
{
	SetButtonValue(tbtn_add_iso,0);
	bool b = GetButtonValue(tbtn_add_quad);
	SetButtonValue(tbtn_add_quad,!b);
	tf->cur_creation = !b ? TF2D_ACTION_ADD_QUAD:-1;

	//	OnToggleArrow(event);
	MyApp::frame->OnMouseArrow(event);
}
void tf2dInfoWindow::OnToggleBC(wxCommandEvent& event)
{
	tf->cur_action = TF_ACTION_BC;
}
void tf2dInfoWindow::OnBtnDelete(wxCommandEvent& event)
{
	
	CT::need_rerender_tf2d=1;
	CT::need_rerender=1;
	tf->selected_obj=-1;

}
void tf2dInfoWindow::OnBtnShowHistogram(wxCommandEvent& event)
{
	//histogram.Build(CPU_VD::full_data,CPU_VD::full_size.x*CPU_VD::full_size.y*CPU_VD::full_size.z);
}
void tf2dInfoWindow::OnBtnColor(wxCommandEvent& event)
{
	if(tf->selected_obj==-1)return;
	wxColor c1 = MyApp::frame->SelectColour(),c0(0,0,0);
	if(c1!=c0)
	{
		int i=tf->selected_id;
		
		CT::need_rerender_tf2d=1;
		CT::need_rerender=1;
	}
}
/*
void tfInfoWindow::OnBtnAddIso(wxCommandEvent& event)
{
	if(tf->isos->size()>=MAX_ISO_NUM)return;

	IsoSurface iso;
	iso.color = vec4(RND01,RND01,RND01,0.9f);
	iso.value = tf->center+0.5*tf->width/tf->scale;

	tf->isos->push_back(iso);
	CT::iso->ReLoadShader();
	tf->MyRefresh();
	if(tf->isos->size()>=MAX_ISO_NUM) btn_add_iso->Enable(0);
}
void tfInfoWindow::OnBtnAddQuad(wxCommandEvent& event)
{
	TF_QUAD qq;
	qq.color = vec4(RND01,RND01,RND01,0.9f);
	qq.l1 = tf->center+0.3*tf->width/tf->scale;
	qq.l2 = qq.l1 + 0.3*tf->width/tf->scale;

	tf->AddQuad(qq);
	tf->MyRefresh();
}*/

void TF2D_window::UpdateCursor()
{
	static char old_cur=0;
	char cur=0;
	TF2D_window*tf=this;
	if(tf->cur_creation!=-1)cur=3;else


	if(tf->sel_obj!=-1)cur=6;else
	if(tf->cur_action==TF2D_ACTION_TRANSLATE)cur=1;else
	if(tf->cur_action==TF2D_ACTION_ZOOM)cur=2;else
	if(tf->cur_action==V2D_ACTION_BC)cur=7;


	if(old_cur==cur)return;
	old_cur=cur;
	wxCursor c;
	switch(cur)
	{
		case 0:		c = wxCursor(wxCURSOR_ARROW);break;
		case 1:		c = wxCursor(wxImage("Images/translate.png",wxBITMAP_TYPE_PNG));break;
		case 2:		c = wxCursor(wxImage("Images/zoom.png",wxBITMAP_TYPE_PNG));break;
		case 3:		c = wxCursor(wxImage("Images/add.png",wxBITMAP_TYPE_PNG));break;
		case 4:		c = wxCursor(wxCURSOR_SIZENS);break;
		case 5:		c = wxCursor(wxCURSOR_SIZEWE);break;
		case 6:		c = wxCursor(wxCURSOR_HAND);break;
		case 7:		c = wxCursor(wxImage("Images/BC.png",wxBITMAP_TYPE_PNG));break;
	}
	SetCursor(c);
}
void tf2dInfoWindow::Save(std::ofstream& fs)
{
//	for(int i=0;i<MAX_VD_NUMBER;i++)
//		SaveVector(fs,tf->tf_quads[i]);
	SaveItem(fs,tf->scale);
	SaveItem(fs,tf->center);
	
}
void tf2dInfoWindow::Load(std::ifstream& fs)
{
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
//		OpenVector(fs,tf->tf_quads[i]);
//		CT::SetCurDataID(i);
//		tf->ApplyQuads();
	}
	CT::SetCurDataID(0);
	OpenItem(fs,tf->scale);
	OpenItem(fs,tf->center);
	
	
}
