#ifndef PERF_PANEL_INC
#define PERF_PANEL_INC

#include "AllDef.h"
#include "MyApp.h"
#include "TF_window.h"
#include "AllInc.h"
#include "wxIncludes.h"

class PerfPanel: public wxPanel
{
public:
    PerfPanel(wxWindow *frame);
	~PerfPanel()
	{
	}
	void OnBtnUpdate(wxCommandEvent& event);

	void OnSliderZ(wxCommandEvent& WXUNUSED(event));
	void OnSliderFastView(wxCommandEvent& WXUNUSED(event));
	void OnSliderIQ(wxCommandEvent& WXUNUSED(event));//interaction quality
	void OnCheckBB(wxCommandEvent& event);
	void Update_(bool self);
	void OnListRenderingType(wxCommandEvent& WXUNUSED(event));
	void OnListRenderingMode(wxCommandEvent& WXUNUSED(event));
	void OnListShadingMode(wxCommandEvent& WXUNUSED(event));
	void OnListTF(wxCommandEvent& WXUNUSED(event));


private:
	wxChoice *m_box_rmode,*m_box_rtype,*m_box_shading_type,*m_box_tf;
	wxSlider *m_slider_z[3];
	wxCheckBox *m_check_box_autofps,*m_check_use_accel_struct,*m_check_use_cubic_filt,*m_check_bicubic_filtering;
	
	 DECLARE_EVENT_TABLE()
};



#endif