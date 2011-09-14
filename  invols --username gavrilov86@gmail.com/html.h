
#ifndef WX_HTML_FRAME_H
#define WX_HTML_FRAME_H

#include "wxIncludes.h"




class HtmlFrame : public wxFrame
{
    public:
        // ctor and dtor

        HtmlFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
        virtual ~HtmlFrame();

        // event handlers (these functions should _not_ be virtual)
        void OnPageSetup(wxCommandEvent& event);
        void OnPrint(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);

		bool LoadPage(wxString& fn);
    private:
		wxString m_Name;
        wxHtmlWindow *m_Html;
        wxHtmlEasyPrinting *m_Prn;
        
		DECLARE_EVENT_TABLE()
};



#endif