#include "log.h"
#include "output.h"
#include "AllDef.h"

BEGIN_EVENT_TABLE(LogPanel, wxPanel)
	EVT_BUTTON(1, LogPanel::OnBtnUpdate)
	EVT_BUTTON(2, LogPanel::OnBtnClear)
	EVT_CHOICE(3, LogPanel::OnList)
END_EVENT_TABLE()

LogPanel::LogPanel(wxWindow *frame) : wxPanel(frame)
{
	current_log=1;
	m_Html = new wxHtmlWindow(this);
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)
	m_box = new wxChoice(this,3);
	m_box->Append(MY_TXT("General","Общий лог"));
	m_box->Append(MY_TXT("Application","Приложение"));
	m_box->Append(MY_TXT("Shaders","Шейдеры"));
	m_box->Select(current_log);
	wxButton*btn_up = new wxBitmapButton(this,1,IMG("update_log"));
	wxButton*btn_cl = new wxBitmapButton(this,2,IMG("clear_log"));
	btn_up->SetToolTip(MY_TXT("Update log","Обновить лог"));
	btn_cl->SetToolTip(MY_TXT("Clear log","Очистить лог"));

	wxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	sz1->Add(m_box,1,wxGROW);
	sz1->Add(btn_up,0,wxSTRETCH_NOT);
	sz1->Add(btn_cl,0,wxSTRETCH_NOT);
	sz->Add(m_Html,1,wxGROW);
	sz->Add(sz1,0,wxSTRETCH_NOT);
	SetSizer(sz);
}

void LogPanel::OnList(wxCommandEvent& WXUNUSED(event))
{
	current_log = m_box->GetSelection();
	OnBtnUpdate(wxCommandEvent());
	
}
Output* LogByID(int id)
{
	Output* cur_output = &output::general;
	if(id==1)cur_output = &output::application;
	if(id==2)cur_output = &output::shaders;
	return cur_output;
}
void LogPanel::OnBtnClear(wxCommandEvent& event)
{
	LogByID(current_log)->clear();
	OnBtnUpdate(wxCommandEvent());
}
void LogPanel::OnBtnUpdate(wxCommandEvent& event)
{
	static int old_current_log = -2;
	Output* cur_output = LogByID(current_log);

	if(old_current_log==current_log)
	if(!cur_output->need_update)return;

	std::ofstream fs("tmp.html",std::ios::out | std::ios::binary);
	fs << cur_output->GetOutput();
	fs.close();
	m_Html -> LoadPage("tmp.html");
	cur_output->need_update=0;

	old_current_log = current_log;
	
}
