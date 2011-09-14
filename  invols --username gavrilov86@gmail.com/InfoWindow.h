#ifndef _WX_INFO_WINDOW_H_
#define _WX_INFO_WINDOW_H_

#include "CT/CT.h"
#include "wxIncludes.h"
#include "wx/notebook.h"

#include "wx/things/spinctld.h"

class InfoWindow: public wxScrolledWindow
{
public:
    InfoWindow(wxWindow* parent);
    virtual ~InfoWindow(){};


	void OnPageChanged(wxNotebookEvent& event);

	void UpdateHtml();
	
	bool nead_html_reload;
    private:
		
        wxHtmlWindow *m_Html;
		wxNotebook * notebook;
		wxSizer *m_sizerBook;

		DECLARE_EVENT_TABLE()
};


#endif