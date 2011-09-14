#ifndef _WX_TF_WINDOW_H_
#define _WX_TF_WINDOW_H_
#include "MyApp.h"

#include "CT/CT.h"

#include "wxIncludes.h"
#include "wx/notebook.h"

#include "wx/things/spinctld.h"
#include "wx/tglbtn.h"

class tfInfoWindow;

#define TF_MARGIN 15
#define TF_QUAD_DRAG_BORDER 5

enum
{
	SELECTION_OBJECT_ISO,
	SELECTION_OBJECT_LEVEL,
	SELECTION_OBJECT_QUAD,
	SELECTION_OBJECT_TF_POINT
};
enum
{
	TF_ACTION_ARROW=1,
	TF_ACTION_TRANSLATE,
	TF_ACTION_ZOOM,
	TF_ACTION_BC,
	TF_ACTION_ADD_ISO,
	TF_ACTION_ADD_QUAD,
	TF_ACTION_DELETE,
	TF_ACTION_COLOR,
	TF_SHOW_HISTOGRAM,
	TF_ACTION_SAVE_TF,
	TF_ACTION_LOAD_TF

};

class TF_window: public wxGLCanvas
{
public:
    TF_window(wxWindow *parent, wxWindowID id = wxID_ANY,        const wxPoint& pos = wxDefaultPosition,        const wxSize& size = wxDefaultSize, long style = 0,        const wxString& name = _T("GLCanvas"), int *gl_attrib = NULL);

   ~TF_window()
   {
   };

   void InitGL();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
	void MyRefresh();
	
	tfInfoWindow *par;

	void UpdateCursor();

	//std::vector<TF_QUAD>*tf_quads{return &tf_quads[CT::GetCurDataID()];}


	void PutLine(int x1,int y1,int x2,int y2);
	void AddQuad(TF_QUAD qq);

	vec4 *tf;
	std::vector<IsoSurface> * isos;
	std::vector<TF_QUAD>* tf_quads;
	std::vector<TF_POINT>* tf_points;
	
	double scale,center;
	
	short start_view,end_view;

    int width, height;
		float hist2d_br;

	bool mouse_left_is_down;
//selection
	int sel_obj,sel_id,sel_part;	//только наведение курсора

	int selected_obj,selected_id,selected_part;

	int cur_action,cur_creation;
	static TF_window*inst;
private:

	//vec4 tf[1024];
	

    DECLARE_EVENT_TABLE()
};



class tfInfoWindow: public wxPanel
{
public:
    tfInfoWindow(wxWindow* parent);

    virtual ~tfInfoWindow()
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
		void OnBtnSaveTF(wxCommandEvent& event);
		void OnBtnLoadTF(wxCommandEvent& event);

		void Save(wxFile& fs);
		void Load(wxFile& fs);

		void SaveTF(wxFile& fs);
		void LoadTF(wxFile& fs);

		void SetButtonValue(wxBitmapButton *b,bool v);
		bool GetButtonValue(wxBitmapButton *b);

		TF_window * tf;
		wxBitmapButton *tbtn_arrow,*tbtn_translate,*tbtn_zoom,*tbtn_add_iso,*tbtn_add_quad,*tbtn_show_histogram;


    private:
		
		//wxSizer *m_sizerBook;
		//wxScrolledWindow *sw;
		

		DECLARE_EVENT_TABLE()
};

#endif 

