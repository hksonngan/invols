#ifndef VD_PANEL_INC
#define VD_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"

class VDPanel: public wxPanel
{
public:
    VDPanel(wxWindow *frame);

	void OnList(wxCommandEvent& WXUNUSED(event));
	void OnList1(wxCommandEvent& WXUNUSED(event));
	void OnBtnLoadToGPU(wxCommandEvent& event);
	void OnBtnSegment(wxCommandEvent& event);
	void OnBtnDelete(wxCommandEvent& event);

private:
	wxChoice *m_box;
	wxChoice *m_box1;

	DECLARE_EVENT_TABLE()
};


#endif