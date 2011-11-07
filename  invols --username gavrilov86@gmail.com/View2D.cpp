#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"


//#include <GL/gl.h>
//#include <GL/glu.h>
#include <wx/stattext.h>
#include "AllDef.h"
#include "View2D.h"
#include "drawtext.h"
#include "Img3DProc.h"
#include "CPU_VD.h"
#include "str.h"
#include "GLCanvas.h"
#include "Progress.h"

BEGIN_EVENT_TABLE(View2D, wxGLCanvas)
    EVT_SIZE(View2D::OnSize)
    EVT_PAINT(View2D::OnPaint)
    EVT_KEY_DOWN(View2D::OnKeyDown)
	EVT_KEY_UP(View2D::OnKeyUp)
    EVT_MOUSE_EVENTS(View2D::OnMouseEvent)
    EVT_SIZE(View2D::OnSize)
	
END_EVENT_TABLE()

vec2 View2D::bc(0,0.1);

View2D::View2D(wxWindow *parent, wxWindowID id,const wxPoint& pos, const wxSize& size, long style,    const wxString& name, int* gl_attrib)    : 
//wxGLCanvas(parent, id, pos, size, style, name, gl_attrib)
//wxGLCanvas(parent, MyApp::GLContext, id ,  pos, size ,  style, name,MyApp::gl_attributes)
  wxGLCanvas(parent, id,MyApp::gl_attributes, pos, size, style, name)
{
	mirror.set(1,1);
	txt2 = 0;
//	txt_reg = 0;

	to_show=0;
	gl_inited=0;
	scale = 1;
	cur_slice = 156;
	center = vec2(0);
	use_bicubic_filt=1;
	cur_action=V2D_ACTION_ARROW;
	sel_obj=-1;
	selected_obj=-1;

	if(parent)
		parent->Show(true);
	SetCurrent(*MyApp::GLContext);
	
	MyRefresh();

	
	GetClientSize(&width, &height);
}
void View2D::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    GetClientSize(&width, &height);

//	wxGLContext *c = GetContext();
  //  if (c)
    {
	
		SetCurrent(*MyApp::GLContext);
		glViewport(0, 0, (GLint) width, (GLint) height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, width/(height+0.0001f), 1,0);
		glMatrixMode(GL_MODELVIEW);

		MyRefresh();
    }

	
}
void View2D::InitGL()
{
//	glClearColor(0,0,0,0);
    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glLineStipple(1,15+(15<<8));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void View2D::MyRefresh()
{
	//CT::need_rerender2d=1;
	Refresh(false);
}


void View2D::OnKeyDown(wxKeyEvent& event)
{
	CT::KeyButton(event.GetKeyCode(), 1);
    switch( event.GetKeyCode() )
    {
    case WXK_ESCAPE:
//		segmentation::UnMarkAll();
		break;
	case 32:
//		segmentation::MarkBB(255);
		break;
	}
}

void View2D::OnKeyUp(wxKeyEvent& event)
{
}
View2D::~View2D()
{
	if(txt2)delete txt2;	
	mip.Clear();
	sum.Clear();
//	if(txt_reg)delete txt_reg;	
}

vec2 View2D::Get2DBoxMin()
{
	vec3 b1_3d = CT::iso->GetBoundingBox(0);
	return vec2(b1_3d.GetByID(axis_x),b1_3d.GetByID(axis_y));
}
vec2 View2D::Get2DBoxMax()
{
	vec3 b2_3d = CT::iso->GetBoundingBox(1);
	return vec2(b2_3d.GetByID(axis_x),b2_3d.GetByID(axis_y));
}
vec3 View2D::Get3DPoint(vec2 p,float level)
{
	vec3 res;
	res.SetByID(axis_x,p.x);
	res.SetByID(axis_y,p.y);
	res.SetByID(axis  ,level);
	return res;

}
vec2 View2D::Get2DPoint(vec3 p)
{
	return vec2(p.GetByID(axis_x),p.GetByID(axis_y));

}
void View2D::LoadTexture()
{

	if(!CPU_VD::full_data.GetSlices())return;
	need_texture_reload=0;
	ivec3 size = CPU_VD::full_data.GetSize();
	if(cur_slice<0 || cur_slice>=size.GetByID(axis))return;
	if(txt2)delete txt2;
	int ww = size.GetByID(axis_x),hh = size.GetByID(axis_y);
	
	VData data(ivec3(ww,hh,1),CPU_VD::full_data.GetValueFormat());
	void *dt;
	
	if(to_show==0)
	{
	if(axis==2)
		dt = CPU_VD::full_data.GetSlice(cur_slice);
	else
	{
		dt = data.GetSlice(0);
		
		if(axis==0)
		{//ww-y//hh-z
			for(int j=0;j<hh;j++)
			for(int i=0;i<ww;i++)
			{
				//data[i+ww*j] = *(CPU_VD::full_data.g + cur_slice + size.x*(i+j*size.y));
				data.SetValue(CPU_VD::full_data.GetValue(cur_slice,i,j), i,j,0);
			}
		}else
			for(int j=0;j<size.z;j++)
			for(int i=0;i<size.x;i++)
			{
				//data[i+ww*j] = *(CPU_VD::full_data + i + size.x*(cur_slice+j*size.y));
				data.SetValue(CPU_VD::full_data.GetValue(i,cur_slice,j), i,j,0);
			}
	}
	}else
	if(to_show==1)
	{
		if(!sum.GetSlices())UpdateSumMip();
		dt = sum.GetSlice(0);
	}else
	if(to_show==2)
	{
		if(!mip.GetSlices())UpdateSumMip();
		dt = mip.GetSlice(0);
	}

	txt2 = new SimText3D(2,ww,hh,1,1,dt,0,1,data.GetGLFormat());

	data.Clear();
}
/*void View2D::LoadTextureReg()
{
	need_texture_reg_reload=0;
	if(!segmentation::region)return;
	ivec3 size = CPU_VD::full_data.GetSize();
	if(cur_slice<0 || cur_slice>=size.GetByID(axis))return;
//	if(txt_reg)delete txt_reg;
	int ww = size.GetByID(axis_x),hh = size.GetByID(axis_y);
	BYTE* data;

	if(axis==2)
		data = segmentation::region +size.x*size.y*cur_slice;
	else
	{
		data = new BYTE[ww*hh];
		
		if(axis==0)
		{//ww-y//hh-z
			for(int j=0;j<hh;j++)
			for(int i=0;i<ww;i++)
			{
				data[i+ww*j] = *(segmentation::region + cur_slice + size.x*(i+j*size.y));
			}
		}else
			for(int j=0;j<size.z;j++)
			for(int i=0;i<size.x;i++)
			{
				data[i+ww*j] = *(segmentation::region + i + size.x*(cur_slice+j*size.y));
			}
	}
	

	
	txt_reg = new SimText3D(2,ww,hh,1,1,data,0,1,GL_UNSIGNED_BYTE);
	
	if(axis!=2)delete[]data;
}*/

void View2D::UpdateSumMip()
{
	if(!CPU_VD::full_data.GetSlices())return;
	Progress progress(wxT("Generating 2D MIP,SUM..."));
	Progress::inst->AddText(wxT("init"));
	ivec3 size = CPU_VD::full_data.GetSize();

	int ww = size.GetByID(axis_x),hh = size.GetByID(axis_y),dd = size.GetByID(axis);
	short* data;
	sum.Allocate(ivec3(ww,hh,1),CPU_VD::full_data.GetValueFormat());
	mip.Allocate(ivec3(ww,hh,1),CPU_VD::full_data.GetValueFormat());
	int* sum1 = new int[ww*hh];
	for(int i=0;i<ww*hh;i++)
	{
		sum1[i]=0;
	}
	mip.SetAllValues(0);
	ivec3 bb1 = ivec3(0);//CT::GetBoundingBox(0);
	ivec3 bb2 = CPU_VD::full_data.GetSize()-ivec3(1);//CT::GetBoundingBox(1);
	ivec3 id;
	//short min_val = short(bc.x*256*128);
	//short max_val = short(bc.y*256*128);
	int*idx=&id.x+axis_x;
	int*idy=&id.x+axis_y;
	for(id.z=bb1.z;id.z<=bb2.z;id.z++)
	{
	for(id.y=bb1.y;id.y<=bb2.y;id.y++)
	for(id.x=bb1.x;id.x<=bb2.x;id.x++)
	{
		short val = CPU_VD::full_data.GetValue(id);
		//if(val>=min_val && val<=max_val)
		{
			int idd = *idx+ww**idy;
			sum1[idd] += val;
			short tmp=mip.GetValue(*idx,*idy,0);
			if(tmp<val)mip.SetValue(val,*idx,*idy,0);
		}
		
	}
	Progress::inst->SetPercent((id.z-bb1.z)/(float)(bb2.z-bb1.z));
	}
	dd/=2;
	for(int i=0;i<ww*hh;i++)
	{
		sum.SetValue(sum1[i]/dd,i%ww,i/ww,0);
	}
	delete[]sum1;
}

BEGIN_EVENT_TABLE(View2DWindow, wxPanel)
//	EVT_TOGGLEBUTTON(V2D_ACTION_ARROW, View2DWindow::OnToggleArrow)
//	EVT_TOGGLEBUTTON(V2D_ACTION_TRANSLATE, View2DWindow::OnToggleTranslate)
//	EVT_TOGGLEBUTTON(V2D_ACTION_ZOOM, View2DWindow::OnToggleZoom)
	EVT_COMMAND_SCROLL(V2D_ACTION_SCROLL,View2DWindow::OnScroll)
	EVT_CHOICE(V2D_ACTION_CHOISE, View2DWindow::OnList)
	EVT_BUTTON(V2D_ACTION_MIRROR_OX, View2DWindow::OnBtnMirrorOx)
	EVT_BUTTON(V2D_ACTION_MIRROR_OY, View2DWindow::OnBtnMirrorOy)
	EVT_BUTTON(V2D_ACTION_UP, View2DWindow::OnBtnUp)
	EVT_CHECKBOX(V2D_ACTION_CHECKBOX, View2DWindow::OnCheckBox)

END_EVENT_TABLE()

View2DWindow::View2DWindow(wxWindow* parent,int axis_)
        : wxPanel(parent)
{
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)

	v2d = new View2D(this);
	sc = new wxScrollBar(this,V2D_ACTION_SCROLL,wxDefaultPosition,wxDefaultSize,wxSB_VERTICAL);
	v2d->axis = axis_;
	v2d->axis_x = !axis_;
	v2d->axis_y = 2-(axis_==2);

	info = new wxStaticText(this,wxID_ANY,"qwe");
//	tbtn_arrow = new wxToggleButton(this,V2D_ACTION_ARROW,"Arrow");
//	tbtn_translate = new wxToggleButton(this,V2D_ACTION_TRANSLATE,"Move");
//	tbtn_zoom = new wxToggleButton(this,V2D_ACTION_ZOOM,"Zoom");

	m_box = new wxChoice(this,V2D_ACTION_CHOISE);
	m_box->Append(MY_TXT("Slice","Слой"));
	m_box->Append(MY_TXT("Simple average","Среднее"));
	m_box->Append(MY_TXT("Maximum","Максимум"));
	m_box->Select(0);

	wxButton*btn_mirror_Ox = new wxBitmapButton(this,V2D_ACTION_MIRROR_OX,IMG("mirror_Ox"));
	btn_mirror_Ox->Show(0);
	btn_mirror_Ox->SetToolTip(MY_TXT("Mirror(V)","Отразить по вертикали"));
	wxButton*btn_mirror_Oy = new wxBitmapButton(this,V2D_ACTION_MIRROR_OY,IMG("mirror_Oy"));
	btn_mirror_Oy->Show(0);
	btn_mirror_Oy->SetToolTip(MY_TXT("Mirror(H)","Отразить по горизонтали"));

	wxButton*btn_up = new wxBitmapButton(this,V2D_ACTION_UP,IMG("update_texture_2d"));
	btn_up->SetToolTip(MY_TXT("Update","Обновить"));
	m_check_box_use_bicubic_filt = new wxCheckBox(this,V2D_ACTION_CHECKBOX,MY_TXT("Bicubic filtering","Бикубическая фильтрация"));
	m_check_box_use_bicubic_filt->SetValue(1);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz2 = new wxBoxSizer(wxHORIZONTAL);

//	sz1->Add(tbtn_arrow,0,wxSTRETCH_NOT);
//	sz1->Add(tbtn_translate,0,wxSTRETCH_NOT);
//	sz1->Add(tbtn_zoom,0,wxSTRETCH_NOT);
	


	v2d->par = this;
	sz1->Add(v2d,1,wxEXPAND);
	sz1->Add(sc,0,wxEXPAND);

	sz2->Add(m_box,1,wxSTRETCH_NOT);
	sz2->AddSpacer(10);
	sz2->Add(btn_mirror_Ox,0,wxSTRETCH_NOT);
	sz2->Add(btn_mirror_Oy,0,wxSTRETCH_NOT);
	sz2->Add(btn_up,0,wxSTRETCH_NOT);
	sz2->AddSpacer(20);
	sz2->Add(m_check_box_use_bicubic_filt,0,wxSTRETCH_NOT);
	sz2->AddSpacer(20);
	sz2->Add(info,0,wxSTRETCH_NOT);


	sz->Add(sz1,1,wxEXPAND);
	sz->Add(sz2,0,wxSTRETCH_NOT);


	SetSizer(sz);
}
void View2DWindow::OnCheckBox(wxCommandEvent& event)
{
	v2d->use_bicubic_filt = m_check_box_use_bicubic_filt->GetValue();

	delete CT::view2d_sp;
	CT::view2d_sp = new ShaderProgram("common_shaders//srf.vs", "common_shaders//view2d.fs");


	v2d->MyRefresh();
}

void View2DWindow::OnList(wxCommandEvent& WXUNUSED(event))
{
	int sel = m_box->GetSelection();
	v2d->to_show = sel;
	v2d->need_texture_reload=1;
	v2d->MyRefresh();
	
}
void View2DWindow::OnToggleArrow(wxCommandEvent& event)
{
	/*
	tbtn_arrow->SetValue(1);
	tbtn_translate->SetValue(0);
	tbtn_zoom->SetValue(0);
	*/
	v2d->cur_action = V2D_ACTION_ARROW;
	
}
void View2DWindow::OnToggleTranslate(wxCommandEvent& event)
{
	/*
	tbtn_arrow->SetValue(0);
	tbtn_translate->SetValue(1);
	tbtn_zoom->SetValue(0);
	*/
	v2d->cur_action = V2D_ACTION_TRANSLATE;

}

void View2DWindow::OnToggleZoom(wxCommandEvent& event)
{
	/*
	tbtn_arrow->SetValue(0);
	tbtn_translate->SetValue(0);
	tbtn_zoom->SetValue(1);
	*/
	v2d->cur_action = V2D_ACTION_ZOOM;

}

void View2DWindow::OnToggleBC(wxCommandEvent& event)
{
	v2d->cur_action = V2D_ACTION_BC;

}

void View2DWindow::OnToggleMeasureLength(wxCommandEvent& event)
{
	v2d->cur_action = V2D_ACTION_MEASURE_LENGTH;

}
void View2D::UpdateCursor()
{
	
	char cur=0;

	if(cur_action==V2D_ACTION_BC)cur=10;
	if(cur_action==V2D_ACTION_ZOOM)cur=2;
	if(cur_action==V2D_ACTION_TRANSLATE)cur=1;
	if(sel_obj==V2D_SELECTION_OBJECT_BB)
	{
		if(sel_part==V2D_SELECTION_PART_FACE)cur=7;
		if(sel_part==V2D_SELECTION_PART_EDGE)
		{
			cur = sel_id<=1?4:5;
		}
		if(sel_part==V2D_SELECTION_PART_POINT)
		{
			cur = (sel_id==1 || sel_id==2)?8:9;
			if(mirror.x*mirror.y<0)cur = 8+9-cur;
		}
	}
	if(sel_obj==V2D_SELECTION_OBJECT_CENTER)
	{
		if(V2D_SELECTION_PART_X==sel_part)cur=4;
		if(V2D_SELECTION_PART_Y==sel_part)cur=5;
		if(V2D_SELECTION_PART_CENTER==sel_part)cur=7;
	}
	/*
	if(mouse_left_is_down&&selected_obj==V2D_SELECTION_OBJECT_BB&&(selected_part==V2D_SELECTION_PART_FACE||selected_part==V2D_SELECTION_PART_POINT))cur=7;else
	if(mouse_left_is_down&&selected_obj==V2D_SELECTION_OBJECT_BB&&(selected_part==V2D_SELECTION_PART_FACE||selected_part==V2D_SELECTION_PART_POINT))cur=7;else
	if(mouse_left_is_down&&selected_obj==V2D_SELECTION_OBJECT_BB&&selected_part==V2D_SELECTION_PART_EDGE&&(selected_id<=1))cur=4;else
	if(mouse_left_is_down&&selected_obj==V2D_SELECTION_OBJECT_BB&&selected_part==V2D_SELECTION_PART_EDGE)cur=5;else
*/
	
	



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
		case 7:		c = wxCursor(wxCURSOR_SIZING);break;
		case 8:		c = wxCursor(wxCURSOR_SIZENESW);break;
		case 9:		c = wxCursor(wxCURSOR_SIZENWSE);break;
		case 10:	c = wxCursor(wxImage("Images/BC.png",wxBITMAP_TYPE_PNG));break;

	}
	SetCursor(c);
}
void View2DWindow::Save(wxFile& fs)
{
	SaveItem(fs,v2d->scale);
	SaveItem(fs,v2d->center);
	SaveItem(fs,v2d->mirror);
	SaveItem(fs, View2D::bc);
	
}
void View2DWindow::Load(wxFile& fs)
{
	OpenItem(fs,v2d->scale);
	OpenItem(fs,v2d->center);
	OpenItem(fs,v2d->mirror);
	OpenItem(fs, View2D::bc);
}
void View2DWindow::OnScroll(wxScrollEvent& event)
{
	int slice = sc->GetThumbPosition();
	if(v2d->cur_slice!=slice)
	{
		v2d->cur_slice=slice;
		vec3 center = CT::GetCenter();
		center.SetByID(v2d->axis,slice/(float)CPU_VD::full_data.GetSize().GetByID(v2d->axis));
		CT::SetCenter(center);
		v2d->need_texture_reload=1;
		info->SetLabel(str::ToString(slice));
		CT::need_rerender2d=1;
	}
}

void View2DWindow::Update1(bool self)
{
	if(self)
	{
		int axis = v2d->axis;
		int sz = CPU_VD::full_data.GetSize().GetByID(axis);
		int tr_div = 14;
		sc->SetScrollbar((CT::GetCenter().GetByID(axis)*CPU_VD::full_data.GetSize().GetByID(axis)), sz/tr_div, sz+sz/tr_div-1, sz/tr_div);
		v2d->cur_slice = CT::GetCenter().GetByID(axis)*sz;
		sc->SetThumbPosition(v2d->cur_slice);
		v2d->need_texture_reload=1;
		info->SetLabel(str::ToString(v2d->cur_slice));

		need_Update1=0;

		v2d->MyRefresh();
	}else
	{
	}
}
void View2DWindow::OnBtnMirrorOx(wxCommandEvent& event)
{
	v2d->mirror.y = -v2d->mirror.y;
	v2d->center.y = -v2d->center.y+1;
	v2d->MyRefresh();
}
void View2DWindow::OnBtnMirrorOy(wxCommandEvent& event)
{
	v2d->mirror.x = -v2d->mirror.x;
	v2d->center.x = -v2d->center.x+1;
	v2d->MyRefresh();
}

void View2DWindow::OnBtnUp(wxCommandEvent& event)
{
	v2d->UpdateSumMip();
	v2d->need_texture_reload=1;
	v2d->MyRefresh();
}
