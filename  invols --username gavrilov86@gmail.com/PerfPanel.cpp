#include "PerfPanel.h"
#include "Render.h"
#include "vec.h"

BEGIN_EVENT_TABLE(PerfPanel, wxPanel)
	
	EVT_SLIDER(1, PerfPanel::OnSliderZ)
	EVT_CHECKBOX(4, PerfPanel::OnCheckBB)
	EVT_CHOICE(2, PerfPanel::OnListRenderingType)
	EVT_CHOICE(3, PerfPanel::OnListRenderingMode)
	EVT_CHOICE(6, PerfPanel::OnListShadingMode)
	EVT_CHOICE(7, PerfPanel::OnListTF)
	EVT_BUTTON(5, PerfPanel::OnBtnUpdate)
	EVT_SLIDER(6, PerfPanel::OnSliderFastView)
	EVT_SLIDER(8, PerfPanel::OnSliderIQ)
END_EVENT_TABLE()

float koef_rs=0.1f; //ray step
float IQ_koef=0.04f; //interactive quality
float fast_res_kf=0.05f;//fast resolution

svec rm_files;
void UpdateFS_src()
{
	RenderingMethod* tmp;
	std::string ss;
	CT::iso->rendering_methods.clear();
	rm_files.clear();
	wxString f = wxFindFirstFile("RM/*.fs");

	
	int i=0;
	while ( !f.empty() )
	{
		tmp = new RenderingMethod();
		ss = f;
		tmp->size.set(1,1);
		tmp->caption = ss;
		tmp->fs_filename = ss;
		tmp->pos = tmp->size * vec2(i%3,i/3) * (1.2);
		

		CT::iso->rendering_methods.push_back(tmp);
		rm_files.push_back(ss);
		f = wxFindNextFile();
		i++;
	}
	CT::iso->SetCurRM(0);
	CT::iso->ReLoadShader();
	/*
	if(!rendering_methods.size())
	{
		wxMessageBox("Шейдеры не загружены: в папке RM нет файлов *.fs");
	}
	*/
}
PerfPanel::PerfPanel(wxWindow *frame) : wxPanel(frame)
{
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)
int panel_border=10;
	UpdateFS_src();
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	sz->AddSpacer(panel_border);
	wxSize left_size(100,30);
	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);

	wxButton*btn_up = new wxBitmapButton(this,5,IMG("small_add"));
	btn_up->SetToolTip(MY_TXT("Add selected rendering method","Добавить выбранный метод рендеринга"));
	btn_up->Show(0);


	m_box_rmode = new wxChoice(this,3);
	for(int i=0;i<CT::iso->rendering_methods.size();i++)
	{
		m_box_rmode->Append(CT::iso->rendering_methods[i]->caption);
	}
	m_box_rmode->Select(0);
	//sz->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Rendering method","Метод рендеринга"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_box_rmode,1,wxGROW);
	sz1->Add(btn_up,0,wxSTRETCH_NOT);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
	sz->AddSpacer(panel_border);

	//sz->Add(m_box_rmode,0,wxEXPAND);
//
	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_box_rtype = new wxChoice(this,2);
	m_box_rtype->Append(MY_TXT("Permanent","Постоянный"));
	m_box_rtype->Append(MY_TXT("When something has changed","При изменениях"));
	m_box_rtype->Append(MY_TXT("Frames accumulation","Накопление кадров"));
	//m_box_rtype->Append(MY_TXT("Use LoD when changing","Использовать LoD при изменении"));
	m_box_rtype->Select(CT::RenderingType);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Rendering type","Тип рендеринга"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_box_rtype,1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
//
	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_slider_z[0] = new wxSlider(this,1,100,1,500);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Rendering quality","Качество рендеринга"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_slider_z[0],1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_slider_z[2] = new wxSlider(this,8,100,0,400);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Interaction quality","???"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_slider_z[2],1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_slider_z[1] = new wxSlider(this,6,50,0,100);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Fast resolution","Быстрое разрешение"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_slider_z[1],1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);


	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_box_shading_type = new wxChoice(this,6);
	m_box_shading_type->Append(MY_TXT("No","Нет"));
	m_box_shading_type->Append(MY_TXT("Phong","Фонг"));
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Shading model","Модель затенения"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_box_shading_type,1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
/*
	m_box_shading_type = new wxChoice(this,6);
	m_box_shading_type->Append(MY_TXT("No","Нет"));
	m_box_shading_type->Append(MY_TXT("Phong","Фонг"));
	sz->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Shading model","Модель затенения"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz->Add(m_box_shading_type,1,wxGROW);
*/
	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	m_box_tf = new wxChoice(this,7);
	m_box_tf->Append(MY_TXT("1D Gray","1D Серая"));
	m_box_tf->Append(MY_TXT("1D Colored","1D цветная"));
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Transfer function","Раскраска"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_box_tf,1,wxGROW);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);

//	m_check_box_autofps = new wxCheckBox(this,4,MY_TXT("Permanent rendering","Постоянный рендеринг"),ster+wxPoint(0,100));
	m_check_use_accel_struct = new wxCheckBox(this,4,MY_TXT("Use acceleration structure","Использовать ускоряющую структуру"));
	m_check_use_accel_struct->Show(0);
	m_check_use_cubic_filt = new wxCheckBox(this,4,MY_TXT("Tricubic filtering","Трикубическая фильтрация"));
	m_check_bicubic_filtering = new wxCheckBox(this,4,MY_TXT("Bicubic filtering","Бикубическая фильтрация"));

	sz->Add(m_check_use_accel_struct,0,wxSTRETCH_NOT);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	sz1->Add(m_check_use_cubic_filt,0,wxSTRETCH_NOT);
	sz1->Add(m_check_bicubic_filtering,0,wxSTRETCH_NOT);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
	
	SetSizer(sz);
	
}

void PerfPanel::OnBtnUpdate(wxCommandEvent& event)
{
	int sel = m_box_rmode->GetSelection();
	CT::iso->SetCurRM(CT::iso->CloneRM(CT::iso->GetCurRMID()));
	RenderingMethod* tmp = CT::iso->GetCurRM();

	tmp->caption = m_box_rmode->GetString(sel);
	tmp->fs_filename = rm_files[sel];
	tmp->pos += tmp->size * 0.6;
	
}

void PerfPanel::OnListRenderingMode(wxCommandEvent& WXUNUSED(event))
{
	int sel = m_box_rmode->GetSelection();
	for(int i=0;i<CT::iso->rendering_methods.size();i++)
	{
		if(m_box_rmode->GetString(sel) == CT::iso->rendering_methods[i]->caption)
			CT::iso->SetCurRM(i);
	}
	CT::need_rerender=1;
	CT::need_rerender_tf=1;
}
void PerfPanel::OnListShadingMode(wxCommandEvent& WXUNUSED(event))
{
	RenderingMethod* rm = CT::iso->GetCurRM();
	char ii = rm->shade_mode,ii1 =  m_box_shading_type->GetSelection();
	rm->shade_mode =  ii1;
	if( ii!= ii1){rm->need_reload_shader=1;CT::need_rerender=1;}

	

}
void PerfPanel::OnListTF(wxCommandEvent& WXUNUSED(event))
{
	RenderingMethod* rm = CT::iso->GetCurRM();
	char ii = rm->tf_type,ii1 =  m_box_tf->GetSelection();
	rm->tf_type =  ii1;
	if( ii!= ii1){rm->need_reload_shader=1;CT::need_rerender=1;}

	

}
void PerfPanel::OnListRenderingType(wxCommandEvent& WXUNUSED(event))
{
	CT::RenderingType = m_box_rtype->GetSelection();
	CT::need_rerender=1;
}

void PerfPanel::OnSliderZ(wxCommandEvent& WXUNUSED(event))
{
	CT::iso->SetStepLength(koef_rs/m_slider_z[0]->GetValue());
	CT::need_rerender=1;
}


void PerfPanel::OnSliderFastView(wxCommandEvent& WXUNUSED(event))
{
	CT::fast_res = m_slider_z[1]->GetValue()*fast_res_kf+1.0f;
	CT::Update_rtt();
	CT::need_rerender=1;
	CT::is_changing=1;
}
void PerfPanel::OnSliderIQ(wxCommandEvent& WXUNUSED(event))
{
	CT::interaction_quality = m_slider_z[2]->GetValue()*IQ_koef+1.0f;
	CT::need_rerender=1;
	CT::is_changing=1;
}

void PerfPanel::OnCheckBB(wxCommandEvent& event)
{
	RenderingMethod* rm = CT::iso->GetCurRM();
//	CT::auto_fps = m_check_box_autofps->GetValue();
	bool bbb = rm->use_accel_struct,bbb1 =  m_check_use_accel_struct->GetValue();
	rm->use_accel_struct =  bbb1;
	if( bbb1!= bbb)rm->need_reload_shader=1;

	bbb = rm->use_cubic_filt;bbb1 =  m_check_use_cubic_filt->GetValue();
	rm->use_cubic_filt =  bbb1;
	if( bbb1!= bbb)rm->need_reload_shader=1;
	
	CT::use_bicubic_filtering =  m_check_bicubic_filtering->GetValue();

	CT::need_rerender=1;
}

void PerfPanel::Update_(bool self)
{
	RenderingMethod* rm = CT::iso->GetCurRM();

	if(self)
	{

		m_slider_z[0]->SetValue(koef_rs/CT::iso->GetStepLength());
		m_slider_z[1]->SetValue((CT::fast_res-1)/fast_res_kf);
		m_slider_z[2]->SetValue((CT::interaction_quality-1)/IQ_koef);
		

//		m_check_box_autofps->SetValue(CT::auto_fps);
		m_check_use_accel_struct->SetValue(rm->use_accel_struct);
		m_check_use_cubic_filt->SetValue(rm->use_cubic_filt);
		m_check_bicubic_filtering->SetValue(CT::use_bicubic_filtering);


		for(int i=0;i<m_box_rmode->GetCount();i++)
		{
			if(m_box_rmode->GetString(i) == rm->caption)
				m_box_rmode->Select(i);
		}
		m_box_shading_type->Select(rm->shade_mode);
		m_box_tf->Select(rm->tf_type);
		m_box_rtype->Select(CT::RenderingType);

	}else
	{
		CT::iso->SetStepLength(koef_rs/m_slider_z[0]->GetValue());
		
		CT::fast_res = m_slider_z[1]->GetValue()*fast_res_kf+1.0f;
		CT::interaction_quality = m_slider_z[2]->GetValue()*IQ_koef+1.0f;
		
		rm->use_accel_struct = m_check_use_accel_struct->GetValue();
		rm->use_cubic_filt = m_check_use_cubic_filt->GetValue();
		CT::use_bicubic_filtering = m_check_bicubic_filtering->GetValue();
		

		CT::RenderingType = m_box_rtype->GetSelection();
		int sel = m_box_rmode->GetSelection();
		CT::iso->SetCurRM(sel);

		//CT::need_rerender=1;
	
	}
	//m_slider_z[0]->Enable(!CT::auto_fps);
}

