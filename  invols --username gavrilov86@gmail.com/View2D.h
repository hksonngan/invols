#ifndef _WX_VIEW2D_WINDOW_H_
#define _WX_VIEW2D_WINDOW_H_
#include "MyApp.h"

#include "CT/CT.h"

#include "wxIncludes.h"
#include "wx/notebook.h"

#include "wx/things/spinctld.h"
#include "wx/tglbtn.h"
#include <wx/scrolbar.h>

class View2DWindow;

enum
{
	V2D_ACTION_ARROW=1,
	V2D_ACTION_TRANSLATE,
	V2D_ACTION_ZOOM,
	V2D_ACTION_BC,
	V2D_ACTION_MEASURE_LENGTH,
	V2D_ACTION_SCROLL,
	V2D_ACTION_CHOISE,
	V2D_ACTION_MIRROR_OX,
	V2D_ACTION_MIRROR_OY,
	V2D_ACTION_UP,
	V2D_ACTION_CHECKBOX
};
enum
{
	V2D_SELECTION_OBJECT_BB=1,
	V2D_SELECTION_OBJECT_CENTER,

	V2D_SELECTION_PART_POINT,
	V2D_SELECTION_PART_EDGE,
	V2D_SELECTION_PART_FACE,
	V2D_SELECTION_PART_X,
	V2D_SELECTION_PART_Y,
	V2D_SELECTION_PART_CENTER
};

class View2D: public wxGLCanvas
{
public:
    View2D(wxWindow *parent, wxWindowID id = wxID_ANY,        const wxPoint& pos = wxDefaultPosition,        const wxSize& size = wxDefaultSize, long style = 0,        const wxString& name = _T("GLCanvas"), int *gl_attrib = NULL);

   ~View2D();
   void InitGL();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
	void MyRefresh();
	View2DWindow *par;
	void UpdateCursor();

	void UpdateSumMip();

	vec2 Get2DBoxMin();
	vec2 Get2DBoxMax();
	vec3 Get3DPoint(vec2 p,float level);
	vec2 Get2DPoint(vec3 p);

	float scale;
	vec2 center,mirror;
	
    int width, height;

	//selection
	int sel_obj,sel_id,sel_part;	//только наведение курсора

	int selected_obj,selected_id,selected_part;

	int cur_action;
	bool gl_inited, mouse_left_is_down, need_texture_reload;

	bool use_bicubic_filt;

	int axis,axis_x,axis_y,cur_slice;
	int to_show;//0-slice  1-sum  2-mip

	static vec2 bc;

	SimText3D*txt2;//, *txt_reg;
private:
	VData sum,mip;
	void LoadTexture();
	//void LoadTextureReg();
char old_cur;
    DECLARE_EVENT_TABLE()
};



class View2DWindow: public wxPanel
{
public:
    View2DWindow(wxWindow* parent,int axis_);

    virtual ~View2DWindow(){};

		void OnToggleArrow(wxCommandEvent& event);
		void OnToggleTranslate(wxCommandEvent& event);
		void OnToggleZoom(wxCommandEvent& event);
		void OnToggleBC(wxCommandEvent& event);
		void OnToggleMeasureLength(wxCommandEvent&event);
		void OnScroll(wxScrollEvent& event);
	void OnList(wxCommandEvent& WXUNUSED(event));
	void OnBtnMirrorOx(wxCommandEvent& event);
	void OnBtnMirrorOy(wxCommandEvent& event);
	void OnBtnUp(wxCommandEvent& event);
	void OnCheckBox(wxCommandEvent& event);
		void Save(wxFile& fs);
		void Load(wxFile& fs);

		void Update1(bool self=1);

		View2D * v2d;
		wxToggleButton *tbtn_arrow,*tbtn_translate,*tbtn_zoom;
wxStaticText* info;
		wxScrollBar* sc;
	wxChoice *m_box;
wxCheckBox *m_check_box_use_bicubic_filt;

bool need_Update1;

    private:
		
		DECLARE_EVENT_TABLE()
};

#endif 

