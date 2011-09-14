#include "MyPanel.h"
#include "MyApp.h"
#include "TF_window.h"
/*
BEGIN_EVENT_TABLE(MyPanel, wxPanel)
	
	EVT_SLIDER(1, MyPanel::OnSliderZ)
	EVT_SLIDER(2, MyPanel::OnSliderBox)

	
	EVT_CHOICE(3, MyPanel::OnListStereo)
	EVT_CHECKBOX(4, MyPanel::OnCheckBB)

	EVT_BUTTON(12, MyPanel::OnBtnColor1)
	EVT_BUTTON(13, MyPanel::OnBtnColor2)
	EVT_BUTTON(14, MyPanel::OnBtnDir)

	
END_EVENT_TABLE()

MyPanel*MyPanel::inst=0;

wxString cur_src_dir = "./dt/example1";

MyPanel::MyPanel(wxWindow *frame) : wxPanel(frame)
{
	wxWindow* params=this;
	inst=this;
	wxPoint pt(10,10);

	btn_dir = new wxButton(params,14,"...",pt,wxSize(20,20));
	static_dir = new wxStaticText(params,wxID_ANY,cur_src_dir,pt+wxPoint(40,0),wxSize(200,20));
	pt.y+=30;
	new wxStaticText(params,wxID_ANY,"Изо-значения изоповерхности",pt);
	pt.y+=30;
	new wxStaticText(params,wxID_ANY,"от",pt);
	m_slider_z[0] = new wxSlider(params,1,300,0,1000,pt+wxPoint(40,0),wxSize(256,30));
	pt.y+=30;
	//btn_c1 = new wxButton(params,12,"Цвет...",pt,wxSize(50,20));

	//new wxStaticText(params,wxID_ANY,"до",wxPoint(20,90));
	//m_slider_z[1] = new wxSlider(params,1,100,0,1000,wxPoint(40,80),wxSize(256,30));

	//btn_c2 = new wxButton(params,13,"Цвет...",pt+wxPoint(256,0),wxSize(50,20));
	
	
	
	wxPoint bb(10,150);
	new wxStaticText(params,wxID_ANY,"Коробка",bb);
	new wxStaticText(params,wxID_ANY,"от",bb+wxPoint(0,20));
	new wxStaticText(params,wxID_ANY,"до",bb+wxPoint(0,50));
	for(int i=0;i<3;i++)
	{
		m_slider_z[2+i] = new wxSlider(params,2,0,0,1000,bb+wxPoint(20,20)+wxPoint(100*i,0),wxSize(90,30));
		m_slider_z[5+i] = new wxSlider(params,2,1000,0,1000,bb+wxPoint(20,50)+wxPoint(100*i,0),wxSize(90,30));
	}
	

	wxPoint ster(10,270);
	new wxStaticText(params,wxID_ANY,"Стерео",ster);
	m_box = new wxChoice(params,3,ster+wxPoint(0,20),wxSize(140,30));
	m_box->Append("Моно-изображение");
	m_box->Append("Правильный анаглиф");
	m_box->Append("Серый анаглиф");
	m_box->Append("Цветной анаглиф");
	m_box->Append("Полуцветной анаглиф");
	m_box->Append("Оптимизированный анаглиф");
	m_box->Append("Для проектора");
	m_box->Append("Черезстрочный рендеринг");
	m_box->Append("3D Vision");
	m_box->Select(0);
	new wxStaticText(params,wxID_ANY,"Расстояние между глаз",ster+wxPoint(200,0));
	m_slider_eye = new wxSlider(params,1,100,-1000,1000,ster+wxPoint(170,20),wxSize(170,30));
	m_check_box_BB = new wxCheckBox(params,4,"Показать рамку",ster+wxPoint(170,50));

	ster.y+=100;
	m_slider_my_z = new wxSlider(params,1,700,0,1000,ster,wxSize(100,30));
	new wxStaticText(params,wxID_ANY,"Качество рендеринга",ster+wxPoint(200,0));
	m_slider_z[8] = new wxSlider(params,1,1000,1,2000,ster+wxPoint(170,20),wxSize(170,30));

	m_check_box_autofps = new wxCheckBox(params,4,"Авто-регулировка",ster+wxPoint(170,50));
	m_check_use_accel_struct = new wxCheckBox(params,4,"Использовать ускоряющую структуру",ster+wxPoint(0,80));


	
}


void MyPanel::OnSliderZ(wxCommandEvent& WXUNUSED(event))
{
	float zz1 = m_slider_z[0]->GetValue()*0.001f;
	//float zz2 = m_slider_z[1]->GetValue()*0.0001f;
	CT::iso->SetLevel(zz1);
	//CT::iso->SetMaxAdd(zz2);

	CT::stereo_step = m_slider_eye->GetValue()*0.0003f;
	CT::ct_data.scale.z = m_slider_my_z->GetValue()*0.001f;
	CT::iso->SetStepLength(2.0f/(100+m_slider_z[8]->GetValue()));
}

void MyPanel::OnSliderBox(wxCommandEvent& WXUNUSED(event))
{
	vec3 b1( m_slider_z[2]->GetValue(),m_slider_z[3]->GetValue(),m_slider_z[4]->GetValue());
	vec3 b2( m_slider_z[5]->GetValue(),m_slider_z[6]->GetValue(),m_slider_z[7]->GetValue());

	vec3 b11 = vec3::Min(b1,b2);
	vec3 b22 = vec3::Max(b1,b2);
	CT::iso->SetBoundingBox(b11*0.001f,b22*0.001f);
}
	
void MyPanel::OnListStereo(wxCommandEvent& WXUNUSED(event))
{
	int sm=1,sel = m_box->GetSelection();
	CT::anag = sel;
	if(!sel) sm=0;
	if(sel>=6) sm=sel-4;

	CT::SetStereoMode(sm);
}
void MyPanel::OnCheckBB(wxCommandEvent& event)
{
	CT::iso->draw_frame_is = m_check_box_BB->GetValue();
	CT::auto_fps = m_check_box_autofps->GetValue();
	m_slider_z[8]->Enable(!CT::auto_fps);

	bool bbb = CT::iso->use_accel_struct,bbb1 =  m_check_use_accel_struct->GetValue();
	CT::iso->use_accel_struct =  bbb1;
	if( bbb1!= bbb)CT::iso->ReLoadShader();
	
}

void MyPanel::OnBtnColor1(wxCommandEvent& event)
{
	
	wxColor c1 = MyApp::frame->SelectColour();
	btn_c1->SetBackgroundColour(c1);
	CT::SetColor1( vec3(c1.Red(),c1.Green(),c1.Blue())/255.0f );
}



void MyPanel::OnBtnColor2(wxCommandEvent& event)
{
	
	wxColor c1 = MyApp::frame->SelectColour();
	btn_c2->SetBackgroundColour(c1);
	CT::SetColor2( vec3(c1.Red(),c1.Green(),c1.Blue())/255.0f );
}

void MyPanel::OnBtnDir(wxCommandEvent& event)
{
	wxDirDialog *d = new wxDirDialog( this, _("Choose a source directory"), cur_src_dir, 0, wxDefaultPosition );
	
 	if ( d->ShowModal() ==  wxID_OK )
	{
		cur_src_dir = d->GetPath(); 
		static_dir->SetLabel(cur_src_dir);
		
		CT::LoadFolder(cur_src_dir);

	}
}
void MyPanel::Update_(bool self)
{
	if(self)
	{
		m_slider_z[0]->SetValue(CT::iso->GetMinLevel()*1000);
		m_slider_eye->SetValue(CT::stereo_step/0.0003f);
		m_slider_my_z->SetValue(CT::ct_data.scale.z*1000);
		m_slider_z[8]->SetValue(2/CT::iso->GetStepLength()-100);
		
		vec3 bb1 = CT::iso->GetMinBox()*1000;
		vec3 bb2 = CT::iso->GetMaxBox()*1000;
		m_slider_z[2]->SetValue(bb1.x);
		m_slider_z[3]->SetValue(bb1.y);
		m_slider_z[4]->SetValue(bb1.z);
		m_slider_z[5]->SetValue(bb2.x);
		m_slider_z[6]->SetValue(bb2.y);
		m_slider_z[7]->SetValue(bb2.z);
		
		m_check_box_BB->SetValue(CT::iso->draw_frame_is);
		m_check_box_autofps->SetValue(CT::auto_fps);
		m_check_use_accel_struct->SetValue(CT::iso->use_accel_struct);
	}else
	{
		float zz1 = m_slider_z[0]->GetValue()*0.001f;
		CT::iso->SetLevel(zz1);
		CT::stereo_step = m_slider_eye->GetValue()*0.0003f;
		CT::ct_data.scale.z = m_slider_my_z->GetValue()*0.001f;
		CT::iso->SetStepLength(2.0f/(100+m_slider_z[8]->GetValue()));
		
		vec3 b1( m_slider_z[2]->GetValue(),m_slider_z[3]->GetValue(),m_slider_z[4]->GetValue());
		vec3 b2( m_slider_z[5]->GetValue(),m_slider_z[6]->GetValue(),m_slider_z[7]->GetValue());
		vec3 b11 = vec3::Min(b1,b2);
		vec3 b22 = vec3::Max(b1,b2);
		CT::iso->SetBoundingBox(b11*0.001f,b22*0.001f);

		CT::iso->draw_frame_is = m_check_box_BB->GetValue();
		CT::auto_fps = m_check_box_autofps->GetValue();

		bool bbb = CT::iso->use_accel_struct,bbb1 =  m_check_use_accel_struct->GetValue();
		CT::iso->use_accel_struct =  bbb1;
		if( bbb1!= bbb)CT::iso->ReLoadShader();
	
	}
	m_slider_z[8]->Enable(!CT::auto_fps);
}
*/