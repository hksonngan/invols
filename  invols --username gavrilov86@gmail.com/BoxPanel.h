#ifndef BOX_PANEL_INC
#define BOX_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"

class BoxPanel: public wxPanel
{
public:
    BoxPanel(wxWindow *frame);
	~BoxPanel()
	{
	}

	void OnSliderBox(wxCommandEvent& WXUNUSED(event));
	void OnSliderZ(wxCommandEvent& WXUNUSED(event));
	
	void OnCheckBB(wxCommandEvent& event);

	void Update_(bool self);

private:
	wxSlider *m_slider_z[6],*m_slider_my_z;

	wxCheckBox *m_check_box_BB;
	wxCheckBox *m_check_box[10];
	
	 DECLARE_EVENT_TABLE()
};


#endif