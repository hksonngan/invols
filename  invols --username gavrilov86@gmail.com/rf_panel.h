#ifndef RF_PANEL_INC
#define RF_PANEL_INC

#include "AllInc.h"
#include "wxIncludes.h"
#include "wx/things/spinctld.h"

// Raw Files interpretation panel
class RFPanel: public wxPanel
{
public:
    RFPanel(wxWindow *frame);

//	void OnSpinBoxD(wxSpinEvent& event);
//	void OnSpinBoxI(wxCommandEvent& event);
	void OnBtnOK(wxCommandEvent& event);
	void OnBtnDir(wxCommandEvent& event);
	void Update_(bool self);

	void Save(wxFile& fs);
	void Load(wxFile& fs);

	//int GetOffset();

private:
	wxSpinCtrl *spin_data_size[3],*spin_offset;
	wxSpinCtrlDbl *spin_spacing[3];
	wxCheckBox *m_check_box[10];
	
	DECLARE_EVENT_TABLE()
};


#endif