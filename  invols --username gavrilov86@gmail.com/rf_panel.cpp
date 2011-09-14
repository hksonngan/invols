#include "rf_panel.h"
#include "MyApp.h"
#include "CT.h"
#include "AllDef.h"
#include "CPU_VD.h"
#include "MainFrame.h"
#include "app_state.h"


BEGIN_EVENT_TABLE(RFPanel, wxPanel)
	EVT_BUTTON(1, RFPanel::OnBtnOK)
	EVT_BUTTON(4, RFPanel::OnBtnDir)
END_EVENT_TABLE()

extern bool auto_gpu_upload;

RFPanel::RFPanel(wxWindow *frame) : wxPanel(frame)
{
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)
int panel_border=10;
	wxSize left_size(50,20);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	sz->AddSpacer(panel_border);
	wxSizer* sz1;

//	wxSpinCtrl *spin_data_size[3],*spin_offset;
//	wxSpinCtrlDbl *spin_spacing[3];

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Offset","???"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	spin_offset = new wxSpinCtrl(this, 2,"1",wxDefaultPosition,wxSize(50,20),4096,0,32000,0);
	spin_offset->SetValue(0);
	sz1->Add(spin_offset,1,wxEXPAND);
	//m_check_box = new wxCheckBox(this,5,MY_TXT("from end","???"));
	sz1->AddStretchSpacer(2);
	sz1->AddSpacer(panel_border);


	sz->Add(sz1,0,wxEXPAND);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Size","???"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	for(int i=0;i<3;i++)
	{
		spin_data_size[i] = new wxSpinCtrl(this, 2,"1",wxDefaultPosition,wxSize(50,20),4096,64,512,256);
		spin_data_size[i]->SetValue(256);
		sz1->Add(spin_data_size[i],1,wxEXPAND);
	}
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);
	sz1->Add(new wxStaticText(this,wxID_ANY,MY_TXT("Spacing","???"),wxDefaultPosition,left_size),0,wxSTRETCH_NOT);
	for(int i=0;i<3;i++)
	{
		spin_spacing[i] = new wxSpinCtrlDbl(this, 3,"1",wxDefaultPosition,wxSize(50,20),4096,0,100,1);
		spin_spacing[i]->SetValue(1,0.01,100.0, 0.01);
		sz1->Add(spin_spacing[i],1,wxEXPAND);
	}
	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
	sz->AddSpacer(10);

	sz1 = new wxBoxSizer(wxHORIZONTAL);
	sz1->AddSpacer(panel_border);

	wxButton*btn11=new wxBitmapButton(this,4,IMG("load"),wxDefaultPosition,wxSize(40,40));
	btn11->SetToolTip(MY_TXT("Set source file","???"));
	sz1->Add(btn11,0,wxSTRETCH_NOT);
	wxButton*btn_up = new wxBitmapButton(this,1,IMG("update_log"),wxDefaultPosition,wxSize(40,40));
	btn_up->SetToolTip(MY_TXT("Reload file","???"));
	sz1->Add(btn_up,0,wxSTRETCH_NOT);

	sz1->AddSpacer(panel_border);
	sz->Add(sz1,0,wxEXPAND);
	SetSizer(sz);
}


void RFPanel::OnBtnOK(wxCommandEvent& event)
{
	if(CT::cur_loaded_file_type!=2)
	{
		OnBtnDir(event);
		return;
	}
	
	Update_(0);
	int offset = spin_offset->GetValue();

	CT::LoadRawFile(CT::cur_loaded_file.c_str(),CPU_VD::full_data.GetSize(),CPU_VD::full_data.spacing,offset);
	if(auto_gpu_upload)	MyApp::frame->OnLoadToGPU(event);
	MyApp::frame->Update_(1);
	
}

void RFPanel::OnBtnDir(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Load volume data from raw file","Загрузить объёмные данные"),"./Datasets","","*.raw",wxOPEN);
	if(openFileDialog->ShowModal() == wxID_OK)
	{
		CT::cur_loaded_file = openFileDialog->GetPath();
		CT::cur_loaded_file_type = 2;
		OnBtnOK(event);
	}
	delete openFileDialog;
}

void RFPanel::Update_(bool self)
{
	if(self)
	{
		for(int i=0;i<3;i++)
		{
			spin_data_size[i]->SetValue(CPU_VD::full_data.GetSize().GetByID(i));
			spin_spacing[i]->SetValue(CPU_VD::full_data.spacing.GetByID(i));
		}
		//spin_offset=SetValue(0);
	}else
	{
		CPU_VD::full_data.GetSize().set(spin_data_size[0]->GetValue(),spin_data_size[1]->GetValue(),spin_data_size[2]->GetValue());
		CPU_VD::full_data.spacing.set(spin_spacing[0]->GetValue(),spin_spacing[1]->GetValue(),spin_spacing[2]->GetValue());
	
	}
	
}

void RFPanel::Save(wxFile& fs)
{
	int tmpi = spin_offset->GetValue();
	SaveItem(fs,CPU_VD::full_data.GetSize());
	SaveItem(fs,CPU_VD::full_data.spacing);
	SaveItem(fs,tmpi);
}

void RFPanel::Load(wxFile& fs)
{
	int tmpi;
	OpenItem(fs,CPU_VD::full_data.GetSize());
	OpenItem(fs,CPU_VD::full_data.spacing);
	OpenItem(fs,tmpi);
	spin_offset->SetValue(tmpi);
	Update_(1);
}
