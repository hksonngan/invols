#ifndef LOG_PANEL_INC
#define LOG_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"

class LogPanel: public wxPanel
{
public:
    LogPanel(wxWindow *frame);
	
	void OnList(wxCommandEvent& WXUNUSED(event));
	void OnBtnUpdate(wxCommandEvent& event);
	void OnBtnClear(wxCommandEvent& event);


private:
	int current_log;
	wxChoice *m_box;
	wxHtmlWindow *m_Html;


	DECLARE_EVENT_TABLE()
};


#endif