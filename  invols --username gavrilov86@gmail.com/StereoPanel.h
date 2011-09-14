#ifndef STEREO_PANEL_INC
#define STEREO_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"

class StereoPanel: public wxPanel
{
public:
    StereoPanel(wxWindow *frame);
	~StereoPanel()
	{
	}

	
	void OnSliderZ(wxCommandEvent& WXUNUSED(event));
	void OnListStereo(wxCommandEvent& WXUNUSED(event));

	void Update_(bool self);


private:
	wxSlider *m_slider_eye;

	wxChoice *m_box;

	 DECLARE_EVENT_TABLE()
};


#endif