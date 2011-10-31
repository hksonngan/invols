#ifndef _WX_MAIN_FRAME_H_
#define _WX_MAIN_FRAME_H_

#include "wxIncludes.h"
#include "wx/aui/aui.h"
#include "PerfPanel.h"
#include "StereoPanel.h"
#include "BoxPanel.h"
#include "TF_window.h"
#include "View2D.h"
#include "log.h"
#include "VD_panel.h"
//#include "rf_panel.h"
#include "frames.h"

class GLCanvas;
enum
{
	
	MYACT_EXIT =1,
 MYACT_ANIMATION ,
 MYACT_ABOUT ,
 MYACT_TIMER ,
 MYACT_FULL_SCREEN ,
 MYACT_SAVE_PROFILE ,
 MYACT_LOAD_PROFILE ,
 MYACT_SETDEFAULT_PROFILE ,

 MYACT_MOUSE_ARROW,
 MYACT_MOUSE_ROTATE ,
 MYACT_MOUSE_BC ,
 MYACT_MOUSE_TRANSLATE ,
 MYACT_MOUSE_ZOOM ,
 MYACT_MOUSE_MEASURE_LENGTH,
 MYACT_BGCOLOR,

 MYACT_SHOW_ALL_PANES ,
 MYACT_SHOW_ALL_TOOLS ,
 MYACT_LOAD_DEF_PER,
 MYACT_LOAD_DICOMDIR,
 MYACT_LOAD_BINDIR,
 MYACT_LOAD_TO_GPU,
 MYACT_SWITCH_PROJECTION,
 MYACT_APPLY_MEDIAN_FILTER,
 MYACT_APPLY_GAUSS_FILTER,
 MYACT_APPLY_RESAMPLING,

 MYACT_LIGHT_TO_CAMERA,
 MYACT_DOTLIGHT_TO_CAMERA,
 MYACT_DROP_SHADOWS,
 MYACT_SAVE_VD,
 MYACT_LOAD_VD,
 MYACT_RM_PIC_VIEW,
 MYACT_USE_BOUNDING_MESH,
 MYACT_AUTO_GPU_UPLOAD,
 MYACT_SET_DATA1,
 MYACT_SET_DATA2
};
class MainFrame: public wxFrame
{
public:
    MainFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
    void OnTimer(wxTimerEvent& event);

	virtual ~MainFrame();

	wxColour SelectColour();

	///void OnBtnColor1(wxCommandEvent& WXUNUSED(event));
	//void OnBtnColor2(wxCommandEvent& WXUNUSED(event));


    GLCanvas *m_canvas[4];
	wxTimer timer;

	///////////
	void OnLoadDicomDir(wxCommandEvent& event);
	void OnLoadBinDir(wxCommandEvent& event);
	void OnLoadToGPU(wxCommandEvent& event);

    void OnLoadProfile(wxCommandEvent& event);
    void OnSaveProfile(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void OnSwitchProjection(wxCommandEvent& event);

	void OnFullScreen(wxCommandEvent& event);
	void OnAnimation(wxCommandEvent& event);
	void OnRMPicView(wxCommandEvent& event);

	void OnMouseArrow(wxCommandEvent& event);
	void OnMouseRotate(wxCommandEvent& event);
	void OnMouseZoom(wxCommandEvent& event);
	void OnMouseBC(wxCommandEvent& event);
	void OnMouseTranslate(wxCommandEvent& event);
	void OnMouseMeasureLength(wxCommandEvent& event);

	void OnLightToCamera(wxCommandEvent& event);
	void OnDotLightToCamera(wxCommandEvent& event);
	void OnBGColor(wxCommandEvent& event);
	
	void OnDropShadows(wxCommandEvent& event);
	void OnApplyMedianFilter(wxCommandEvent& event);
	void OnApplyGaussFilter(wxCommandEvent& event);
	void OnApplyResampling(wxCommandEvent& event);
	void OnGoHome(wxCommandEvent& event);
	void OnSaveVD(wxCommandEvent& event);
	void SaveVD(wxString fileName);
	void OnLoadVD(wxCommandEvent& event);
	void OnUseBoundingMesh(wxCommandEvent& event);
	void OnAutoGPUUpload(wxCommandEvent& event);

	void OnSetData1(wxCommandEvent& event);
	void OnSetData2(wxCommandEvent& event);

	void OnShowAllPanes(wxCommandEvent& event);
	void OnShowAllTools(wxCommandEvent& event);
	void OnLoadDefaultPerspective(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
	void UntoggleMouseToolbars();

	void LoadProfile(wxString fname);
	void SaveProfile(wxString fname);
	void SetDefaultProfile(wxCommandEvent& event);

	void MyMessageBox(wxString mess);
	
	PerfPanel *perf_panel;
	LogPanel *log;
	StereoPanel* stereo_panel;
	BoxPanel*box_panel;
	tfInfoWindow*tf;
	VDPanel*vd;
//	RFPanel*rf;
	FramesPanel*fp;

	View2DWindow* v2d[3];

	void Update_(bool self);
	
//////////
	wxAuiManager m_mgr;
private :
	wxString default_perspective;

	wxAuiToolBar *tb1,*tb2,*tb3,*tb4,*tb5,*tb6,*tb7;
    long m_notebook_style;
    long m_notebook_theme;



DECLARE_EVENT_TABLE()
};

#endif