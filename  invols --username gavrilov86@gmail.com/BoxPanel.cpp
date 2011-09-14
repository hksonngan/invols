#include "BoxPanel.h"
#include "MyApp.h"
#include "CT.h"
#include "CPU_VD.h"
#include "AllDef.h"

BEGIN_EVENT_TABLE(BoxPanel, wxPanel)

	EVT_SLIDER(1, BoxPanel::OnSliderZ)
//	EVT_SLIDER(2, BoxPanel::OnSliderBox)

	EVT_CHECKBOX(4, BoxPanel::OnCheckBB)

END_EVENT_TABLE()

float opac_kf = 0.001f;

BoxPanel::BoxPanel(wxWindow *frame) : wxPanel(frame)
{
	int panel_border=10;
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	
	sz->AddSpacer(panel_border);


	/*
	wxPoint bb(10,10);
	new wxStaticText(this,wxID_ANY,MY_TXT("from","от"),bb+wxPoint(0,20));
	new wxStaticText(this,wxID_ANY,MY_TXT("to","до"),bb+wxPoint(0,50));
	for(int i=0;i<3;i++)
	{
		m_slider_z[i] = new wxSlider(this,2,0,0,1000,bb+wxPoint(20,20)+wxPoint(100*i,0),wxSize(90,30));
		m_slider_z[3+i] = new wxSlider(this,2,1000,0,1000,bb+wxPoint(20,50)+wxPoint(100*i,0),wxSize(90,30));
	}
	*/
	m_check_box_BB = new wxCheckBox(this,4,MY_TXT("Geometry","Геометрия"));
	m_check_box_BB->SetValue(CT::draw_frame_is);

	m_check_box[0] = new wxCheckBox(this,4,MY_TXT("YZ Plane","Плоскость YZ"));
	m_check_box[1] = new wxCheckBox(this,4,MY_TXT("XZ Plane","Плоскость XZ"));
	m_check_box[2] = new wxCheckBox(this,4,MY_TXT("XY Plane","Плоскость XY"));
	m_check_box[3] = new wxCheckBox(this,4,MY_TXT("Loaded to GPU box","Загруженный в GPU объём"));
	m_check_box[4] = new wxCheckBox(this,4,MY_TXT("Box to load to GPU","Объём для загрузки в GPU"));

	m_check_box[0]->Show(0);
	m_check_box[1]->Show(0);
	m_check_box[2]->Show(0);
	m_check_box[3]->Show(0);
	m_check_box[4]->Show(0);

	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	wxSizer* sz2 = new wxBoxSizer(wxVERTICAL);
	wxStaticText* statt = new wxStaticText(this,wxID_ANY,MY_TXT("Global opacity","Глобальная непрозрачность"));
	m_slider_my_z = new wxSlider(this,1,500,0,1000);

	sz2->Add(statt,0,wxSTRETCH_NOT);
	sz2->Add(m_slider_my_z,1,wxGROW);
	sz2->Add(m_check_box_BB,0,wxSTRETCH_NOT);
	sz1->Add(sz2,1,wxGROW);
	sz->Add(sz1,0,wxGROW);
	for(int i=0;i<5;i++)
	{
		sz->Add(m_check_box[i],0,wxSTRETCH_NOT);
	}

	SetSizer(sz);
}


void BoxPanel::OnSliderZ(wxCommandEvent& WXUNUSED(event))
{
	//CPU_VD::scale.z = m_slider_my_z->GetValue()*0.001f;
	//CPU_VD::CalcReal();

	CT::iso->SetOpacity( m_slider_my_z->GetValue()*opac_kf );
	CT::need_rerender=1;
}
/*
void BoxPanel::OnSliderBox(wxCommandEvent& WXUNUSED(event))
{
	vec3 b1( m_slider_z[0]->GetValue(),m_slider_z[1]->GetValue(),m_slider_z[2]->GetValue());
	vec3 b2( m_slider_z[3]->GetValue(),m_slider_z[4]->GetValue(),m_slider_z[5]->GetValue());

	vec3 b11 = vec3::Min(b1,b2);
	vec3 b22 = vec3::Max(b1,b2);
	CT::iso->SetBoundingBox(b11*0.001f,b22*0.001f);
}
	*/
void BoxPanel::OnCheckBB(wxCommandEvent& event)
{
	CT::draw_frame_is = m_check_box_BB->GetValue();
	for(int i=0;i<3;i++)
	CT::draw_MRP[i] = m_check_box[i]->GetValue();
	CT::draw_gpu_box = m_check_box[3]->GetValue();
	CT::draw_gpu_dummy = m_check_box[4]->GetValue();

	CT::need_rerender=1;
	CT::need_rerender2d=1;
}


void BoxPanel::Update_(bool self)
{
	if(self)
	{
		
		
/*		vec3 bb1 = CT::iso->GetBoundingBox(0)*1000;
		vec3 bb2 = CT::iso->GetBoundingBox(1)*1000;
		m_slider_z[0]->SetValue(bb1.x);
		m_slider_z[1]->SetValue(bb1.y);
		m_slider_z[2]->SetValue(bb1.z);
		m_slider_z[3]->SetValue(bb2.x);
		m_slider_z[4]->SetValue(bb2.y);
		m_slider_z[5]->SetValue(bb2.z);
*/		
		m_check_box_BB->SetValue(CT::draw_frame_is);

		for(int i=0;i<3;i++)
			m_check_box[i]->SetValue(CT::draw_MRP[i]);
		m_check_box[3]->SetValue(CT::draw_gpu_box);
		m_check_box[4]->SetValue(CT::draw_gpu_dummy);
		m_slider_my_z->SetValue(CT::iso->GetOpacity()/opac_kf);
	
	}else
	{
		CT::iso->SetOpacity( m_slider_my_z->GetValue()*opac_kf );
		CT::need_rerender=1;

		CPU_VD::CalcReal();
/*		vec3 b1( m_slider_z[0]->GetValue(),m_slider_z[1]->GetValue(),m_slider_z[2]->GetValue());
		vec3 b2( m_slider_z[3]->GetValue(),m_slider_z[4]->GetValue(),m_slider_z[5]->GetValue());
		vec3 b11 = vec3::Min(b1,b2);
		vec3 b22 = vec3::Max(b1,b2);
		CT::iso->SetBoundingBox(b11*0.001f,b22*0.001f);
*/
		CT::draw_frame_is = m_check_box_BB->GetValue();
		for(int i=0;i<3;i++)
		CT::draw_MRP[i] = m_check_box[i]->GetValue();
		CT::draw_gpu_box = m_check_box[3]->GetValue();
		CT::draw_gpu_dummy = m_check_box[4]->GetValue();

	}
	
}