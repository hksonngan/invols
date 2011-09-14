#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"


//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "TF_window.h"
#include "drawtext.h"
#include "Img3DProc.h"
#include "CPU_VD.h"

int TF_VERSION=23;

#define TF_MARGIN 15

TF_window*TF_window::inst=0;

BEGIN_EVENT_TABLE(TF_window, wxGLCanvas)
    EVT_SIZE(TF_window::OnSize)
    EVT_PAINT(TF_window::OnPaint)
    EVT_KEY_DOWN(TF_window::OnKeyDown)
	EVT_KEY_UP(TF_window::OnKeyUp)
    EVT_MOUSE_EVENTS(TF_window::OnMouseEvent)
    EVT_SIZE(TF_window::OnSize)
END_EVENT_TABLE()


TF_window::TF_window(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size, long style,    const wxString& name, int* gl_attrib)    : 
//		wxGLCanvas(parent, id, pos, size, style, name, gl_attrib)
		wxGLCanvas(parent, MyApp::GLContext, id ,  pos, size ,  style, name,MyApp::gl_attributes)

		
{
	inst=this;
	hist2d_br=1;
	scale = 500;
	center = 0;
	sel_obj=-1;
	selected_obj=-1;

	cur_action=TF_ACTION_ARROW;
	cur_creation=-1;
	
	if(parent)
		parent->Show(true);
	SetCurrent();
	
	MyRefresh();


	
}

void TF_window::OnSize(wxSizeEvent& event)
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
void TF_window::InitGL()
{
//	glClearColor(0,0,0,0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineStipple(1,15+(15<<8));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void TF_window::MyRefresh()
{

	Refresh(false);
}


void TF_window::OnKeyDown(wxKeyEvent& event)
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

void TF_window::OnKeyUp(wxKeyEvent& event)
{
	//CT::KeyButton(event.GetKeyCode(), 0);
}
void TF_window::AddQuad(TF_QUAD qq)
{
	tf_quads->push_back(qq);
}
/////

BEGIN_EVENT_TABLE(tfInfoWindow, wxPanel)
	EVT_BUTTON(TF_ACTION_ARROW, tfInfoWindow::OnToggleArrow)
	EVT_BUTTON(TF_ACTION_TRANSLATE, tfInfoWindow::OnToggleTranslate)
	EVT_BUTTON(TF_ACTION_ZOOM, tfInfoWindow::OnToggleZoom)
	EVT_BUTTON(TF_ACTION_ADD_ISO, tfInfoWindow::OnToggleAddIso)
	EVT_BUTTON(TF_ACTION_ADD_QUAD, tfInfoWindow::OnToggleAddQuad)
	EVT_BUTTON(TF_ACTION_DELETE, tfInfoWindow::OnBtnDelete)
	EVT_BUTTON(TF_ACTION_COLOR, tfInfoWindow::OnBtnColor)
	EVT_BUTTON(TF_SHOW_HISTOGRAM, tfInfoWindow::OnBtnShowHistogram)
	EVT_BUTTON(TF_ACTION_SAVE_TF, tfInfoWindow::OnBtnSaveTF)
	EVT_BUTTON(TF_ACTION_LOAD_TF, tfInfoWindow::OnBtnLoadTF)


END_EVENT_TABLE()

tfInfoWindow::tfInfoWindow(wxWindow* parent)
        : wxPanel(parent)
{

	tf = new TF_window(this,wxID_ANY);//, wxSize(200,600), 0);
	
//#define IMG(fn) wxBitmap(wxString("Images/")+wxString(fn)+wxString(".png"),wxBITMAP_TYPE_PNG)
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)



//	tbtn_arrow = new wxBitmapButton(this,TF_ACTION_ARROW,IMG("arrow"));
//	tbtn_translate = new wxBitmapButton(this,TF_ACTION_TRANSLATE,IMG("ml_translate"));
//	tbtn_zoom = new wxBitmapButton(this,TF_ACTION_ZOOM,IMG("mr_zoom"));
	tbtn_add_iso = new wxBitmapButton(this,TF_ACTION_ADD_ISO,IMG("add_iso"));
	tbtn_add_quad = new wxBitmapButton(this,TF_ACTION_ADD_QUAD,IMG("add_quad"));
	tbtn_show_histogram = new wxBitmapButton(this,TF_SHOW_HISTOGRAM,IMG("show_hist"));
	wxButton *btn_del = new wxBitmapButton(this,TF_ACTION_DELETE,IMG("delete"));
	wxButton *btn_color = new wxBitmapButton(this,TF_ACTION_COLOR,IMG("color"));

	wxButton *btn_save_tf = new wxBitmapButton(this,TF_ACTION_SAVE_TF,IMG("save"));
	wxButton *btn_load_tf = new wxBitmapButton(this,TF_ACTION_LOAD_TF,IMG("load"));

//	tbtn_arrow->SetToolTip(MY_TXT("Objects' edition","Редактирование объектов"));
//	tbtn_translate->SetToolTip(MY_TXT("Translation","Перенос"));
//	tbtn_zoom->SetToolTip(MY_TXT("Zoom","Масштабирование"));
	
	btn_save_tf->SetToolTip(MY_TXT("Save transfer function","???"));
	btn_load_tf->SetToolTip(MY_TXT("Load transfer function","???"));
	
	tbtn_add_iso->SetToolTip(MY_TXT("Add isosurface","Добавить изоповерхность"));
	tbtn_add_quad->SetToolTip(MY_TXT("Add quad","Добавить прямоугольник"));
	btn_del->SetToolTip(MY_TXT("Delete","Удалить"));
	btn_color->SetToolTip(MY_TXT("Color","Задать цвет"));
	tbtn_show_histogram->SetToolTip(MY_TXT("Build 2D Histogram","Построить 2D гистограмму"));

	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz1 = new wxBoxSizer(wxVERTICAL);
	wxSizer* sz11 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz12 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz13 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz14 = new wxBoxSizer(wxHORIZONTAL);

	sz11->Add(btn_save_tf,0,wxSTRETCH_NOT);
	sz11->Add(btn_load_tf,0,wxSTRETCH_NOT);
	sz12->Add(tbtn_add_iso,0,wxSTRETCH_NOT);
	sz12->Add(tbtn_add_quad,0,wxSTRETCH_NOT);
	sz13->Add(btn_color,0,wxSTRETCH_NOT);
	sz13->Add(btn_del,0,wxSTRETCH_NOT);
	sz14->Add(tbtn_show_histogram,0,wxSTRETCH_NOT);
	sz1->Add(sz12);
	sz1->Add(sz13);
	sz1->Add(sz14);
	sz1->Add(sz11);


	tf->par = this;
	sz->Add(tf,1,wxGROW);
	sz->Add(sz1,0,wxSTRETCH_NOT);
	SetSizer(sz);

}
const wxColor wxColor0(255,255,255);
const wxColor wxColor1(111,111,0);
void tfInfoWindow::SetButtonValue(wxBitmapButton *b,bool v)
{
	b->SetBackgroundColour(v ? wxColor1 : wxColor0);
}
bool tfInfoWindow::GetButtonValue(wxBitmapButton *b)
{
	return (b->GetBackgroundColour() == wxColor1);
}

void tfInfoWindow::OnToggleArrow(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,1);
	SetButtonValue(tbtn_translate,0);
	SetButtonValue(tbtn_zoom,0);
	*/

	tf->cur_action = TF_ACTION_ARROW;
	
}
void tfInfoWindow::OnToggleTranslate(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,0);
	SetButtonValue(tbtn_translate,1);
	SetButtonValue(tbtn_zoom,0);
	*/
	tf->cur_action = TF_ACTION_TRANSLATE;

}
void tfInfoWindow::OnToggleZoom(wxCommandEvent& event)
{
	/*
	SetButtonValue(tbtn_arrow,0);
	SetButtonValue(tbtn_translate,0);
	SetButtonValue(tbtn_zoom,1);
	*/
	tf->cur_action = TF_ACTION_ZOOM;


}
void tfInfoWindow::OnToggleAddIso(wxCommandEvent& event)
{
	SetButtonValue(tbtn_add_quad,0);
	bool b = GetButtonValue(tbtn_add_iso);
	SetButtonValue(tbtn_add_iso,!b);
	tf->cur_creation = !b ? TF_ACTION_ADD_ISO:-1;
	//OnToggleArrow(event);
	MyApp::frame->OnMouseArrow(event);
}
void tfInfoWindow::OnToggleAddQuad(wxCommandEvent& event)
{
	SetButtonValue(tbtn_add_iso,0);
	bool b = GetButtonValue(tbtn_add_quad);
	SetButtonValue(tbtn_add_quad,!b);
	tf->cur_creation = !b ? TF_ACTION_ADD_QUAD:-1;

	//	OnToggleArrow(event);
	MyApp::frame->OnMouseArrow(event);
}
void tfInfoWindow::OnToggleBC(wxCommandEvent& event)
{
	tf->cur_action = TF_ACTION_BC;
}
void tfInfoWindow::OnBtnDelete(wxCommandEvent& event)
{
	if(tf->selected_obj == SELECTION_OBJECT_TF_POINT)
	{
		CT::iso->GetCurRM()->DeletePoint(tf->selected_id);
	}
	if(tf->selected_obj == SELECTION_OBJECT_ISO)
	{
		CT::iso->GetCurRM()->DeleteIso(tf->selected_id);
	}
	if(tf->selected_obj == SELECTION_OBJECT_QUAD)
	{
		CT::iso->GetCurRM()->DeleteQuad(tf->selected_id);
	}
	CT::need_rerender_tf=1;
	CT::need_rerender=1;
	tf->selected_obj=-1;

}
void tfInfoWindow::OnBtnShowHistogram(wxCommandEvent& event)
{
	if(CPU_VD::full_data.GetSlices())
		CT::histogram2D[CT::GetCurDataID()].Build(CPU_VD::full_data);

	//histogram.Build(CPU_VD::full_data,CPU_VD::full_data.GetSize().x*CPU_VD::full_data.GetSize().y*CPU_VD::full_data.GetSize().z);
}
void tfInfoWindow::OnBtnColor(wxCommandEvent& event)
{
	if(tf->selected_obj==-1)return;
	wxColor c1 = MyApp::frame->SelectColour(),c0(0,0,0);
	if(c1!=c0)
	{
		int i=tf->selected_id;
		if(tf->selected_obj == SELECTION_OBJECT_TF_POINT)
		{
			(*tf->tf_points)[i].color.x = c1.Red()/256.0f;
			(*tf->tf_points)[i].color.y = c1.Green()/256.0f;
			(*tf->tf_points)[i].color.z = c1.Blue()/256.0f;
			CT::iso->GetCurRM()->ApplyPoints();
		}
		if(tf->selected_obj == SELECTION_OBJECT_ISO)
		{
			(*tf->isos)[i].color.x = c1.Red()/256.0f;
			(*tf->isos)[i].color.y = c1.Green()/256.0f;
			(*tf->isos)[i].color.z = c1.Blue()/256.0f;
			CT::iso->UpdateUniforms();
		}
		if(tf->selected_obj == SELECTION_OBJECT_QUAD)
		{
			(*tf->tf_quads)[i].color.x = c1.Red()/256.0f;
			(*tf->tf_quads)[i].color.y = c1.Green()/256.0f;
			(*tf->tf_quads)[i].color.z = c1.Blue()/256.0f;
			CT::iso->UpdateUniforms();
		}
		//CT::iso->UpdateUniforms();
		CT::need_rerender_tf=1;
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

void TF_window::UpdateCursor()
{
	static char old_cur=0;
	char cur=0;
	TF_window*tf=this;
	if(tf->cur_creation!=-1)cur=3;else

	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_TF_POINT)cur=6;else
	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_ISO&&tf->selected_part==0)cur=5;else
	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_LEVEL)cur=5;else
	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_QUAD&&(tf->selected_part==0||tf->selected_part==1))cur=5;else
	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_ISO&&tf->selected_part==1)cur=4;else
	if(mouse_left_is_down&&tf->selected_obj==SELECTION_OBJECT_QUAD&&(tf->selected_part==2||tf->selected_part==4))cur=4;else

	if(tf->sel_obj==SELECTION_OBJECT_TF_POINT)cur=6;else
	if(tf->sel_obj==SELECTION_OBJECT_ISO&&tf->sel_part==0)cur=5;else
	if(tf->sel_obj==SELECTION_OBJECT_LEVEL)cur=5;else
	if(tf->sel_obj==SELECTION_OBJECT_QUAD&&(tf->sel_part==0||tf->sel_part==1))cur=5;else
	if(tf->sel_obj==SELECTION_OBJECT_ISO&&tf->sel_part==1)cur=4;else
	if(tf->sel_obj==SELECTION_OBJECT_QUAD&&(tf->sel_part==2||tf->sel_part==4))cur=4;else


	if(tf->sel_obj!=-1)cur=6;else
	if(tf->cur_action==TF_ACTION_TRANSLATE)cur=1;else
	if(tf->cur_action==TF_ACTION_ZOOM)cur=2;else
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
		case 6:		c = wxCursor(wxCURSOR_SIZING);break;
		case 7:		c = wxCursor(wxImage("Images/BC.png",wxBITMAP_TYPE_PNG));break;
	}
	SetCursor(c);
}
void tfInfoWindow::Save(wxFile& fs)
{
	SaveItem(fs,tf->scale);
	SaveItem(fs,tf->center);
	
}
void tfInfoWindow::Load(wxFile& fs)
{
	CT::SetCurDataID(0);
	OpenItem(fs,tf->scale);
	OpenItem(fs,tf->center);
	
	
}
void tfInfoWindow::OnBtnSaveTF(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Save transfer function","???"),"./Transfer Functions","","*.tf",wxSAVE);

	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		wxFile fs(fileName,wxFile::write);
		if(!fs.Error())
		{
			SaveTF(fs);
			fs.Close();
		}
	}
	delete openFileDialog;
}
void tfInfoWindow::OnBtnLoadTF(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Load transfer function","???"),"./Transfer Functions","","*.tf",wxOPEN);

	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		wxFile fs(fileName,wxFile::read);
		if(!fs.Error())
		{
			LoadTF(fs);
			fs.Close();
		}
	}
	delete openFileDialog;
}
void tfInfoWindow::SaveTF(wxFile& fs)
{
	vec2 win = CT::iso->GetWindow();

	SaveItem(fs,TF_VERSION);
	SaveVector(fs,*tf->isos);
	SaveVector(fs,*tf->tf_quads);
	SaveVector(fs,*tf->tf_points);
	SaveItem(fs,tf->scale);
	SaveItem(fs,tf->center);
	SaveItem(fs,tf->hist2d_br);
	SaveItem(fs,win);
}
void tfInfoWindow::LoadTF(wxFile& fs)
{
	vec2 win;

	int ver;
	OpenItem(fs,ver);
	if(ver!=TF_VERSION)
	{
		MyApp::frame->MyMessageBox("Wrong file version :(");
		return;
	}
	OpenVector(fs,*tf->isos);
	OpenVector(fs,*tf->tf_quads);
	OpenVector(fs,*tf->tf_points);
	OpenItem(fs,tf->scale);
	OpenItem(fs,tf->center);
	OpenItem(fs,tf->hist2d_br);
	OpenItem(fs,win);

	CT::iso->SetWindow(win);

	CT::iso->GetCurRM()->ApplyPoints();
	CT::iso->ReLoadCurShader();
	CT::need_rerender_tf=1;
}

