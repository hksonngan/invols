#ifndef _WX_TF_2D_WINDOW_H_
#define _WX_TF_2D_WINDOW_H_
#include "MyApp.h"

#include "CT/CT.h"

#include "wxIncludes.h"
#include "wx/notebook.h"

#include "wx/things/spinctld.h"
#include "wx/tglbtn.h"

class tf2dInfoWindow;

#define TF2D_MARGIN 15

enum
{
	SELECTION2D_OBJECT_LEVEL
};
enum
{
	TF2D_ACTION_ARROW=1,
	TF2D_ACTION_TRANSLATE,
	TF2D_ACTION_ZOOM,
	TF2D_ACTION_BC,
	TF2D_ACTION_ADD_ISO,
	TF2D_ACTION_ADD_QUAD,
	TF2D_ACTION_DELETE,
	TF2D_ACTION_COLOR,
	TF2D_SHOW_HISTOGRAM

};

class TF2D_window: public wxGLCanvas
{
public:
    TF2D_window(wxWindow *parent, wxWindowID id = wxID_ANY,        const wxPoint& pos = wxDefaultPosition,        const wxSize& size = wxDefaultSize, long style = 0,        const wxString& name = _T("GLCanvas"), int *gl_attrib = NULL);

   ~TF2D_window()
   {
   };

   void InitGL();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
	void MyRefresh();
	
	tf2dInfoWindow *par;

	void UpdateCursor();

//	std::vector<TF_QUAD2> *tf_quads2;


	double scale,center;
	
	short start_view,end_view;

    int width, height;

	bool mouse_left_is_down;
//selection
	int sel_obj,sel_id,sel_part;	//только наведение курсора

	int selected_obj,selected_id,selected_part;

	int cur_action,cur_creation;
private:

	//vec4 tf[1024];
	

    DECLARE_EVENT_TABLE()
};



class tf2dInfoWindow: public wxPanel
{
public:
    tf2dInfoWindow(wxWindow* parent);

    virtual ~tf2dInfoWindow()
	{
	};

		void OnToggleArrow(wxCommandEvent& event);
		void OnToggleTranslate(wxCommandEvent& event);
		void OnToggleZoom(wxCommandEvent& event);
		void OnToggleAddIso(wxCommandEvent& event);
		void OnToggleAddQuad(wxCommandEvent& event);
		void OnToggleBC(wxCommandEvent& event);
		void OnBtnDelete(wxCommandEvent& event);
		void OnBtnColor(wxCommandEvent& event);
		void OnBtnShowHistogram(wxCommandEvent& event);

		void Save(std::ofstream& fs);
		void Load(std::ifstream& fs);

		void SetButtonValue(wxBitmapButton *b,bool v);
		bool GetButtonValue(wxBitmapButton *b);

		TF2D_window * tf;
		wxBitmapButton *tbtn_arrow,*tbtn_translate,*tbtn_zoom,*tbtn_add_iso,*tbtn_add_quad,*tbtn_show_histogram;


    private:
		
		//wxSizer *m_sizerBook;
		//wxScrolledWindow *sw;
		

		DECLARE_EVENT_TABLE()
};

#endif 

