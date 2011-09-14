#include "InfoWindow.h"
#include "CT/CT.h"
#include "output.h"
#include "MyApp.h"
#include "MyPanel.h"

BEGIN_EVENT_TABLE(InfoWindow, wxScrolledWindow)
	EVT_NOTEBOOK_PAGE_CHANGED(1, InfoWindow::OnPageChanged)
END_EVENT_TABLE()

InfoWindow::InfoWindow(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE),nead_html_reload(0)
{
    //m_Html -> SetRelatedFrame(this, _("HTML : %s"));
    


	notebook = new wxNotebook(this,1);//, wxDefaultPosition, wxSize(600,600), 0);

	

	//wxWindow *params = new wxWindow(notebook, wxID_ANY,wxDefaultPosition,wxSize(100,100));
	wxWindow *params = new MyPanel(notebook);

	
	notebook->AddPage(params,"Параметры");

	//
	m_Html = new wxHtmlWindow(notebook);//,wxID_ANY,wxPoint(0,0),wxSize(300,300));
	notebook->AddPage(m_Html,"Вывод");

	

	m_sizerBook = new wxBoxSizer(wxHORIZONTAL);
	m_sizerBook->Add(notebook,1, wxEXPAND | wxALL,0);
	
	SetSizer(m_sizerBook);
	
		
}



void InfoWindow::UpdateHtml()
{
	
	if(notebook->GetSelection()==1)
	{
		std::ofstream fs("tmp.html",std::ios::out | std::ios::binary);
		fs << output::out_string;
		fs.close();
		m_Html -> LoadPage("tmp.html");
		nead_html_reload=0;
	}
	
	
}

void InfoWindow::OnPageChanged(wxNotebookEvent& event)
{
    UpdateHtml();
}

