#include "frames.h"
#include "MyApp.h"
#include "CT.h"
#include "CPU_VD.h"
#include "AllDef.h"
#include "MainFrame.h"
#include "str.h"

BEGIN_EVENT_TABLE(FramesPanel, wxPanel)

	EVT_SLIDER(1, FramesPanel::OnSliderZ)

	EVT_BUTTON(2, FramesPanel::OnBtnDir)
	EVT_BUTTON(3, FramesPanel::OnBtnUp)
	EVT_BUTTON(4, FramesPanel::OnBtnMakePictures)
END_EVENT_TABLE()

FramesPanel::FramesPanel(wxWindow *frame) : wxPanel(frame)
{
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)
	wxButton*btn1=new wxBitmapButton(this,2,IMG("load"));
	btn1->SetToolTip(MY_TXT("Choose source directory","Выбрать папку с данными"));
	wxButton*btn2=new wxBitmapButton(this,3,IMG("update_log"));
	btn2->SetToolTip(MY_TXT("Load frame","Загрузить кадр"));
	wxButton*btn3=new wxBitmapButton(this,4,IMG("segment"));
	btn3->SetToolTip(MY_TXT("Make pictures","Сделать кадры"));

	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->Add(btn1,0,wxEXPAND);
	sz1->Add(btn2,0,wxEXPAND);
	sz1->Add(btn3,0,wxEXPAND);

	statt = new wxStaticText(this,wxID_ANY,MY_TXT("Frame","Кадр"));
	m_slider_my_z = new wxSlider(this,1,0,0,0);

	sz->Add(sz1,0,wxEXPAND);
	sz->Add(statt,0,wxSTRETCH_NOT);
	sz->Add(m_slider_my_z,0,wxGROW);

	SetSizer(sz);
}

void FramesPanel::OnBtnDir(wxCommandEvent& event)
{
	MyApp::frame->OnLoadBinDir(event);
}

void FramesPanel::OnBtnUp(wxCommandEvent& event)
{
	if(CT::cur_frame!=m_slider_my_z->GetValue())
	{
		CT::SetFrame(m_slider_my_z->GetValue());
	}
}

void FramesPanel::OnBtnMakePictures(wxCommandEvent& event)
{
	CT::MakeMovie();
}
void FramesPanel::OnSliderZ(wxCommandEvent& WXUNUSED(event))
{
	statt->SetLabel(str::ToString(m_slider_my_z->GetValue()));
}


void FramesPanel::Update_(bool self)
{
	if(self)
	{
		m_slider_my_z->SetValue(CT::cur_frame);
		m_slider_my_z->SetMax(CT::FramesNum()-1);
		
	}else
	{
		

	}
	
}