#include "html.h"



// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Print,
    Minimal_Preview,
    Minimal_PageSetup

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(HtmlFrame, wxFrame)
    EVT_MENU(Minimal_Print, HtmlFrame::OnPrint)
    EVT_MENU(Minimal_Preview, HtmlFrame::OnPreview)
    EVT_MENU(Minimal_PageSetup, HtmlFrame::OnPageSetup)
END_EVENT_TABLE()



// ============================================================================
// implementation
// ============================================================================


// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------


// frame constructor
HtmlFrame::HtmlFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Minimal_PageSetup, _("Page Setup"));
    menuFile->Append(Minimal_Print, _("Print..."));
    menuFile->Append(Minimal_Preview, _("Preview..."));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));

    SetMenuBar(menuBar);

    m_Html = new wxHtmlWindow(this);
    m_Html -> SetRelatedFrame(this, _("HTML : %s"));
    m_Prn = new wxHtmlEasyPrinting(_("Easy Printing Demo"), this);
    m_Prn -> SetHeader(m_Name + wxT("(@PAGENUM@/@PAGESCNT@)<hr>"), wxPAGE_ALL);
	m_Prn->SetStandardFonts(12);
	

}
bool HtmlFrame::LoadPage(wxString& fn)
{
	m_Name = fn;
	
	return m_Html -> LoadPage(fn);
}
// frame destructor
HtmlFrame::~HtmlFrame()
{
    delete m_Prn;
    m_Prn = (wxHtmlEasyPrinting *) NULL;
}


// event handlers

void HtmlFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PageSetup();
}


void HtmlFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PrintFile(m_Name);
}


void HtmlFrame::OnPreview(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PreviewFile(m_Name);
}

