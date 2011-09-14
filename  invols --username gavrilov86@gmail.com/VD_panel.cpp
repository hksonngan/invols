#include "VD_panel.h"
#include "MyApp.h"
#include "CT.h"
#include "AllDef.h"
#include "CPU_VD.h"
#include "MainFrame.h"
#include "Img3DProc.h"

BEGIN_EVENT_TABLE(VDPanel, wxPanel)
	EVT_BUTTON(1, VDPanel::OnBtnSegment)
	EVT_BUTTON(2, VDPanel::OnBtnLoadToGPU)
	EVT_CHOICE(3, VDPanel::OnList)
	EVT_BUTTON(4, VDPanel::OnBtnDelete)
	EVT_CHOICE(5, VDPanel::OnList1)
	
END_EVENT_TABLE()

VDPanel::VDPanel(wxWindow *frame) : wxPanel(frame)
{
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)

	int panel_border=10;
	wxSize left_size(100,30);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	sz->AddSpacer(panel_border);
	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);

	m_box = new wxChoice(this,3);
	m_box->Append("64");
	m_box->Append("128");
	m_box->Append("256");
	m_box->Append("512");
	m_box->Select(3);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Size for GPU","???"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	sz1->Add(m_box,0,wxEXPAND);
	sz1->AddSpacer(panel_border);
	sz->Add(sz1);

	m_box1 = new wxChoice(this,5);
	m_box1->Append("Volume 1");
	m_box1->Append("Volume 2");
	m_box1->Append("Volume 3");
	m_box1->Select(0);
	sz->Add(m_box1);
	//m_box1->Show(0);


	wxButton*btn_=new wxBitmapButton(this,2,IMG("load_to_gpu"));
	//wxButton*btn_segment=new wxBitmapButton(this,1,IMG("segment"));
	//wxButton*btn_del=new wxBitmapButton(this,4,IMG("delete"));
	btn_->SetToolTip(MY_TXT("Upload to GPU","Загрузить в GPU"));
	btn_->Show(0);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	//sz1->Add(btn_segment,0,wxEXPAND);
	sz1->Add(btn_,0,wxEXPAND);
	//sz1->Add(btn_del,0,wxEXPAND);
	sz->Add(sz1,0);
	
	SetSizer(sz);
}


void VDPanel::OnList(wxCommandEvent& WXUNUSED(event))
{
	int sel = m_box->GetSelection();
	if(sel==0)CPU_VD::gpu_size_dummy.set(64,64,64);
	if(sel==1)CPU_VD::gpu_size_dummy.set(128,128,128);
	if(sel==2)CPU_VD::gpu_size_dummy.set(256,256,256);
	if(sel==3)CPU_VD::gpu_size_dummy.set(512,512,512);
	CT::need_rerender=1;
	CT::need_rerender2d=1;
}

void VDPanel::OnList1(wxCommandEvent& WXUNUSED(event))
{
	int sel = m_box1->GetSelection();
	CT::SetCurDataID(sel);
}
void VDPanel::OnBtnDelete(wxCommandEvent& event)
{
	//CT::GetCurData()->Clear();
	
}
void VDPanel::OnBtnLoadToGPU(wxCommandEvent& event)
{
	MyApp::frame->OnLoadToGPU(event);
	
}
void VDPanel::OnBtnSegment(wxCommandEvent& event)
{
	/*
	if(CPU_VD::full_data)
	{
		segmentation::MakeSegmentation();
		MyApp::frame->Update_(1);
	}
	else
		MyApp::frame->MyMessageBox(MY_TXT("There is no data for segmentation","Нет данных для сегментации"));
		*/
	
}