#ifndef FRAMES_PANEL_INC
#define FRAMES_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"

class FramesPanel: public wxPanel
{
public:
    FramesPanel(wxWindow *frame);
	~FramesPanel()
	{
	}

	void OnSliderZ(wxCommandEvent& WXUNUSED(event));
	
	void OnBtnDir(wxCommandEvent& event);
	void OnBtnUp(wxCommandEvent& event);
	void OnBtnMakePictures(wxCommandEvent& event);


	void Update_(bool self);

private:
	wxSlider *m_slider_my_z;
	wxStaticText* statt;

	
	
	 DECLARE_EVENT_TABLE()
};


#endif