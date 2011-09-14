#ifndef _WX_PROG_H_
#define _WX_PROG_H_

#include "wxIncludes.h"


class ProgressFrame: public wxDialog
{
public:
    ProgressFrame(wxWindow *frame, const wxString& title);

	virtual ~ProgressFrame();

	ProgressFrame **par;

	wxGauge *m_gauge;
	wxStaticText*m_time;

	void UpdateHtml(wxString text);
private :
	wxHtmlWindow *m_Html;


DECLARE_EVENT_TABLE()
};


class Progress
{
public:
	
	Progress(wxString title);
	~Progress();

	void SetText(wxString text);
	void AddText(wxString text);
	void SetPercent(float p);
	float GetPercent(){return 0.001f*pf->m_gauge->GetValue();}
	void Close();
	wxString GetText(){return pr_text;}
	static Progress *inst;
private:
	ProgressFrame*pf;
	wxString pr_text;
};

#endif