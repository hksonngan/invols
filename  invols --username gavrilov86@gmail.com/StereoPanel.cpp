#include "StereoPanel.h"
#include "MyApp.h"
#include "CT.h"
#include "AllDef.h"

BEGIN_EVENT_TABLE(StereoPanel, wxPanel)
	
	EVT_SLIDER(1, StereoPanel::OnSliderZ)
	EVT_CHOICE(3, StereoPanel::OnListStereo)


	
END_EVENT_TABLE()

StereoPanel::StereoPanel(wxWindow *frame) : wxPanel(frame)
{
	
	wxPoint ster(10,10);
	m_box = new wxChoice(this,3,ster,wxSize(140,30));
	m_box->Append(MY_TXT("Mono","Моно-изображение"));
	m_box->Append(MY_TXT("True anaglyph","Правильный анаглиф"));
	m_box->Append(MY_TXT("Gray anaglyph","Серый анаглиф"));
	m_box->Append(MY_TXT("Color anaglyph","Цветной анаглиф"));
	m_box->Append(MY_TXT("Half-color anaglyph","Полуцветной анаглиф"));
	m_box->Append(MY_TXT("Optimized anaglyph","Оптимизированный анаглиф"));
	m_box->Append(MY_TXT("Two screens","Для проектора"));
	m_box->Append(MY_TXT("Interlaced","Черезстрочный рендеринг"));
	m_box->Append("3D Vision");
	m_box->Select(0);
	new wxStaticText(this,wxID_ANY,MY_TXT("Parallax","Расстояние между глаз"),ster+wxPoint(0,40));
	m_slider_eye = new wxSlider(this,1,100,-1000,1000,ster+wxPoint(0,60),wxSize(140,30));

}


void StereoPanel::OnSliderZ(wxCommandEvent& WXUNUSED(event))
{
	CT::stereo_step = m_slider_eye->GetValue()*0.0003f;
	CT::need_rerender=1;
}

	
void StereoPanel::OnListStereo(wxCommandEvent& WXUNUSED(event))
{
	int sm=1,sel = m_box->GetSelection();
	CT::anag = sel;
	if(!sel) sm=0;
	if(sel>=6) sm=sel-4;

	CT::SetStereoMode(sm);
}
void StereoPanel::Update_(bool self)
{
	if(self)
	{
		m_slider_eye->SetValue(CT::stereo_step/0.0003f);

	}else
	{
		CT::stereo_step = m_slider_eye->GetValue()*0.0003f;
	}
}