 #include "AllInc.h"
#include "CT/CT.h"
#include "Progress.h"
#include "wxIncludes.h"
#include "output.h"
#include "MyApp.h"



BEGIN_EVENT_TABLE(ProgressFrame, wxDialog)
	
END_EVENT_TABLE()
 


// My frame constructor
ProgressFrame::ProgressFrame(wxWindow *frame, const wxString& title)    : 
		wxDialog(frame,  title, true)
{
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);
	m_Html = new wxHtmlWindow(this);
	m_time = new wxStaticText(this,wxID_ANY,"???");
	m_gauge = new wxGauge(this,0,1000);
	sz->Add(m_Html,12,wxGROW);
	sz->Add(m_time,0,wxEXPAND);
	sz->Add(m_gauge,0,wxEXPAND);
	this->SetSizer(sz);

	

}
void ProgressFrame::UpdateHtml(wxString text)
{
	std::ofstream fs("tmp.html",std::ios::out | std::ios::binary);
	fs << text;
	fs.close();
	m_Html -> LoadPage("tmp.html");
	
}
ProgressFrame::~ProgressFrame()
{
	*par=0;
}

///////////////////
Progress*Progress::inst;

Progress::Progress(wxString title)
{
	inst=this;
	pf = new ProgressFrame((wxWindow *)MyApp::frame,title);
	pf->par = &pf;

	pf->Show();
	pf->Update();
}
void Progress::SetText(wxString text)
{
	pr_text = text;
	if(pf)	
	{
		pf->UpdateHtml(pr_text);
		pf->Update();
	}
}
void Progress::AddText(wxString text)
{
	pr_text += text;
	SetText(pr_text);
}
void Progress::SetPercent(float p)
{
	static double start_time;
	if(p==0)start_time=CT::glfwGetTime();
	if(pf)	
	{
		int	el_sec = (1-p)*(CT::glfwGetTime ( )-start_time)/(p+0.0001f);

		pf->m_time->SetLabel( str::ToString(int(100*p)) + "%   "+
			str::ToString(el_sec/3600)+":"+str::ToString((el_sec/60)%60)+":"+str::ToString(el_sec%60)+"  elapsed");

		pf->m_gauge->SetValue(1000*p);
		pf->Update();
	}

}

void Progress::Close()
{
	if(pf)	
	{
		pf->Close();
		delete pf;
		pf=0;
	}
}
Progress::~Progress()
{
	if(pf)pf->Close();
	delete pf;
}