#include "AllInc.h"
#include "CT.h"
#include "MainFrame.h"
#include "wxIncludes.h"
#include "GLCanvas.h"
#include "output.h"
#include "MySplitterWindow.h"
#include "Camera.h"
#include "DicomReader.h"
#include "CPU_VD.h"
#include "seg_points.h"
#include "app_state.h"

const int PROFILE_VERSION = 75;


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(MYACT_EXIT, MainFrame::OnExit)
	EVT_MENU(MYACT_ANIMATION, MainFrame::OnAnimation)
	EVT_MENU(MYACT_RM_PIC_VIEW, MainFrame::OnRMPicView)
	EVT_MENU(MYACT_FULL_SCREEN, MainFrame::OnFullScreen)

	EVT_MENU(MYACT_LOAD_DICOMDIR, MainFrame::OnLoadDicomDir)
	EVT_MENU(MYACT_LOAD_BINDIR, MainFrame::OnLoadBinDir)
	EVT_MENU(MYACT_LOAD_TO_GPU, MainFrame::OnLoadToGPU)
	EVT_MENU(MYACT_SAVE_PROFILE, MainFrame::OnSaveProfile)
	EVT_MENU(MYACT_LOAD_PROFILE, MainFrame::OnLoadProfile)
	
	EVT_MENU(MYACT_MOUSE_ARROW, MainFrame::OnMouseArrow)
	EVT_MENU(MYACT_MOUSE_ROTATE, MainFrame::OnMouseRotate)
	EVT_MENU(MYACT_MOUSE_ZOOM, MainFrame::OnMouseZoom)
	EVT_MENU(MYACT_MOUSE_BC, MainFrame::OnMouseBC)
	EVT_MENU(MYACT_MOUSE_TRANSLATE, MainFrame::OnMouseTranslate)
	EVT_MENU(MYACT_MOUSE_MEASURE_LENGTH, MainFrame::OnMouseMeasureLength)
	EVT_MENU(MYACT_BGCOLOR, MainFrame::OnBGColor)
	

	EVT_MENU(MYACT_SHOW_ALL_PANES, MainFrame::OnShowAllPanes)
	EVT_MENU(MYACT_SHOW_ALL_TOOLS, MainFrame::OnShowAllTools)
	EVT_MENU(MYACT_LOAD_DEF_PER, MainFrame::OnLoadDefaultPerspective)
	EVT_MENU(MYACT_APPLY_MEDIAN_FILTER, MainFrame::OnApplyMedianFilter)
	EVT_MENU(MYACT_APPLY_GAUSS_FILTER, MainFrame::OnApplyGaussFilter)
	EVT_MENU(MYACT_APPLY_RESAMPLING, MainFrame::OnApplyResampling)
	EVT_MENU(MYACT_SAVE_VD, MainFrame::OnSaveVD)
	EVT_MENU(MYACT_LOAD_VD, MainFrame::OnLoadVD)
	EVT_MENU(MYACT_USE_BOUNDING_MESH, MainFrame::OnUseBoundingMesh)
	EVT_MENU(MYACT_AUTO_GPU_UPLOAD, MainFrame::OnAutoGPUUpload)
	
	
	EVT_MENU(MYACT_LIGHT_TO_CAMERA, MainFrame::OnLightToCamera)
	EVT_MENU(MYACT_DOTLIGHT_TO_CAMERA, MainFrame::OnDotLightToCamera)
	EVT_MENU(MYACT_DROP_SHADOWS, MainFrame::OnDropShadows)

	
	
	EVT_MENU(MYACT_SWITCH_PROJECTION, MainFrame::OnSwitchProjection)

	EVT_MENU(MYACT_ABOUT, MainFrame::OnAbout)

	EVT_TIMER(wxID_ANY, MainFrame::OnTimer)
END_EVENT_TABLE()

int gl_attrib[20] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 24, WX_GL_STENCIL_SIZE, 8,
		WX_GL_MIN_ACCUM_RED,8,   WX_GL_MIN_ACCUM_GREEN,8,   WX_GL_MIN_ACCUM_BLUE,8,  WX_GL_LEVEL,0, WX_GL_STEREO };

bool auto_gpu_upload=1;
// My frame constructor
MainFrame::MainFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,   const wxSize& size, long style)    : 
		wxFrame(frame, wxID_ANY, title, pos, size, style),
			timer(this)
{
	
	m_canvas[0]=0;
	v2d[0]=0;
	v2d[1]=0;
	v2d[2]=0;
	m_mgr.SetManagedWindow(this);

	MyApp::gl_attributes = gl_attrib;
	
    //wxAuiToolBarItemArray prepend_items;
    //wxAuiToolBarItemArray append_items;
  //  wxAuiToolBarItem item;
  //  item.SetKind(wxITEM_SEPARATOR);
  //  append_items.Add(item);
  //  item.SetKind(wxITEM_NORMAL);
 //   item.SetId(ID_CustomizeToolbar);
//    item.SetLabel(_("Customize..."));
 //   append_items.Add(item);
	
#define IMG(fn) wxBitmap(wxString("./Images/")+wxString(fn)+wxString(".bmp"),wxBITMAP_TYPE_ANY)
	wxSize icon_size(32,32);

	tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
    tb1->SetToolBitmapSize(icon_size);
	tb1->AddTool(MYACT_SAVE_PROFILE, "", IMG("save"),_T(MY_TXT("Save Workspace","Сохранить профиль")));
	tb1->AddTool(MYACT_LOAD_PROFILE, "", IMG("load"),_T(MY_TXT("Load Workspace","???")));
    tb1->Realize();
    m_mgr.AddPane(tb1, wxAuiPaneInfo().Name(wxT("tb1")).Caption(wxT(MY_TXT("Workspace","???"))).ToolbarPane().Top());

	tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );
    tb1->SetToolBitmapSize(icon_size);
	tb1->AddTool(MYACT_LOAD_DICOMDIR, "", IMG("load_dcm"),wxT(MY_TXT("Load DICOM data from a directory","Загрузить данные из папки")));
	tb1->AddTool(MYACT_LOAD_TO_GPU, "", IMG("load_to_gpu"),wxT(MY_TXT("Load selected volume data to GPU","Загрузить выделенные объёмные данные в GPU")));
	tb1->AddTool(MYACT_AUTO_GPU_UPLOAD, "", IMG("auto_gpu_upload"),wxT(MY_TXT("Upload to GPU automaticly","Автозагрузка данных в GPU")),wxITEM_CHECK);
	tb1->ToggleTool(MYACT_AUTO_GPU_UPLOAD, 1);
    tb1->Realize();
    m_mgr.AddPane(tb1, wxAuiPaneInfo().Name(wxT("tb1_1")).Caption(wxT(MY_TXT("Volume Data","Объёмные данные"))).ToolbarPane().Top());


	tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb2->SetToolBitmapSize(icon_size);
	//tb2->AddTool(MYACT_SWITCH_PROJECTION, "", IMG("proj"),wxT(MY_TXT("Perspective projection","Перспективная проекция")),wxITEM_CHECK);
	//tb2->AddTool(MYACT_ANIMATION, "", IMG("anim"),wxT(MY_TXT("Animation","Анимация")),wxITEM_CHECK);
	tb2->AddTool(MYACT_RM_PIC_VIEW, "", IMG("RM_pic_view"),wxT(MY_TXT("Rendering methods view","Обзор методов рендеринга")),wxITEM_CHECK);
	tb2->ToggleTool(MYACT_RM_PIC_VIEW, CT::RM_pic_man);
	tb2->AddTool(MYACT_FULL_SCREEN, "", IMG("fullscreen"),wxT(MY_TXT("Fullscreen mode","Во весь экран")));
    tb2->Realize();
	m_mgr.AddPane(tb2, wxAuiPaneInfo().Name(wxT("tb2")).Caption(wxT(MY_TXT("View","Вид"))).ToolbarPane().Top());

	tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb3->SetToolBitmapSize(icon_size);
	tb3->AddTool(MYACT_MOUSE_ARROW, "", IMG("arrow"),wxT(MY_TXT("Object edition","Редактирование объектов")),wxITEM_CHECK);
	tb3->AddTool(MYACT_MOUSE_ROTATE, "", IMG("ml_rotate"),wxT(MY_TXT("Camera rotation","Вращение камеры")),wxITEM_CHECK);
	tb3->AddTool(MYACT_MOUSE_TRANSLATE, "", IMG("ml_translate"),wxT(MY_TXT("Camera translation","Перенос камеры")),wxITEM_CHECK);
	tb3->AddTool(MYACT_MOUSE_BC, "", IMG("mr_bc"),wxT(MY_TXT("Window edition","Редактирование окна")),wxITEM_CHECK);
	tb3->AddTool(MYACT_MOUSE_ZOOM, "", IMG("mr_zoom"),wxT(MY_TXT("Zoom in/out","Приблизить/Удалить")),wxITEM_CHECK);
//	tb3->AddTool(MYACT_MOUSE_MEASURE_LENGTH, "", IMG("measure_length"),wxT(MY_TXT("Length measurement","Измерение расстояний")),wxITEM_CHECK);
    tb3->Realize();
	m_mgr.AddPane(tb3, wxAuiPaneInfo().Name(wxT("tb3")).Caption(wxT(MY_TXT("Left mouse button functions","Функции левой кнопки мыши"))).ToolbarPane().Top());

	/*
	tb4 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb4->SetToolBitmapSize(icon_size);
	tb4->AddTool(MYACT_MOUSE_MEASURE_LENGTH, "", IMG("measure_length"),wxT(MY_TXT("Length measurement","Измерение расстояний")),wxITEM_CHECK);
    tb4->Realize();
//	m_mgr.AddPane(tb4, wxAuiPaneInfo().Name(wxT("tb4")).Caption(wxT(MY_TXT("Camera positioning","Позиционирование камеры"))).ToolbarPane().Top());
*/
	tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb5->SetToolBitmapSize(icon_size);
	tb5->AddTool(MYACT_LIGHT_TO_CAMERA, "", IMG("light_to_camera"),wxT(MY_TXT("Set light to camera","Задать направленный свет (по направлению камеры)")),wxITEM_CHECK);
	tb5->ToggleTool(MYACT_LIGHT_TO_CAMERA, 1);
//	tb5->AddTool(MYACT_DOTLIGHT_TO_CAMERA, "", IMG("dotlight_to_camera"),wxT(MY_TXT("Move light2 position to camera's","Перенести точечный источник света в позицию камеры")));
//	tb5->AddTool(MYACT_DROP_SHADOWS, "", IMG("drop_shadows"),wxT(MY_TXT("Drop shadows","Отбрасывать тени")),wxITEM_CHECK);
    tb5->Realize();
	m_mgr.AddPane(tb5, wxAuiPaneInfo().Name(wxT("tb5")).Caption(wxT(MY_TXT("Illumination","Освещение"))).ToolbarPane().Top());

	tb6 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    tb6->SetToolBitmapSize(icon_size);
	tb6->AddTool(MYACT_USE_BOUNDING_MESH, "", IMG("use_bounding_mesh"),wxT(MY_TXT("Use bounding mesh","Использовать ограничивающую ролигональную модель")),wxITEM_CHECK);
    tb6->Realize();
	m_mgr.AddPane(tb6, wxAuiPaneInfo().Name(wxT("tb6")).Caption(wxT(MY_TXT("Bounding mesh","Огранич.модель"))).ToolbarPane().Top());

	wxPanel *gl_p[4];

	gl_p[0] = new wxPanel(this);
	m_canvas[0] = new GLCanvas(gl_p[0], wxID_ANY, wxDefaultPosition, wxSize(400,400), 0, _T("GLCanvas"), gl_attrib );
	wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
	sz->Add(m_canvas[0],1,wxGROW);
	gl_p[0]->SetSizer(sz);

	//MyApp::GLContext = m_canvas[0] ->GetContext();

	m_mgr.AddPane(gl_p[0], wxAuiPaneInfo().Name(wxT("m_canvas0")).Caption(wxT("GL_view0")).BestSize(wxSize(400,400)).Top().Left().MaximizeButton(true).CenterPane());
//	m_mgr.AddPane(gl_p[1], wxAuiPaneInfo().Name(wxT("m_canvas1")).Caption(wxT("GL_view1")).BestSize(wxSize(400,400)).Top().Right().CenterPane());
//	m_mgr.AddPane(gl_p[2], wxAuiPaneInfo().Name(wxT("m_canvas2")).Caption(wxT("GL_view2")).BestSize(wxSize(400,400)).Bottom().Left().CenterPane());
//	m_mgr.AddPane(gl_p[3], wxAuiPaneInfo().Name(wxT("m_canvas3")).Caption(wxT("GL_view3")).BestSize(wxSize(400,400)).Bottom().Right().CenterPane());
	

	for(int i=0;i<3;i++)
	{
		v2d[i] = new View2DWindow(this,i);
	}
	m_mgr.AddPane(v2d[0], wxAuiPaneInfo().Name(wxT("v2d0")).Caption(wxT("YZ")).BestSize(wxSize(520,520)).Top().MaximizeButton(true));
	m_mgr.AddPane(v2d[1], wxAuiPaneInfo().Name(wxT("v2d1")).Caption(wxT("XZ")).BestSize(wxSize(520,520)).Top().MaximizeButton(true));
	m_mgr.AddPane(v2d[2], wxAuiPaneInfo().Name(wxT("v2d2")).Caption(wxT("XY")).BestSize(wxSize(520,520)).Top().MaximizeButton(true));

	
	log = new LogPanel(this);
	stereo_panel = new StereoPanel(this);
	box_panel = new BoxPanel(this);
	
	perf_panel = new PerfPanel(this);
	vd = new VDPanel(this);
	//rf = new RFPanel(this);


	fp = new FramesPanel(this);

    m_mgr.AddPane(perf_panel, wxAuiPaneInfo().
                  Name(wxT("perf_panel")).Caption(wxT(MY_TXT("Rendering","Редеринг"))).
                  Right().CloseButton(true).MaximizeButton(true).BestSize(wxSize(200,200)));
    m_mgr.AddPane(log, wxAuiPaneInfo().
                  Name(wxT("Log")).Caption(wxT(MY_TXT("Log","Лог"))).BestSize(wxSize(200,200)).
                  Left().CloseButton(true).MaximizeButton(true));
    m_mgr.AddPane(stereo_panel, wxAuiPaneInfo().
                  Name(wxT("stereo_panel")).Caption(wxT(MY_TXT("Stereo","Стерео"))).
                  Left().CloseButton(true).MaximizeButton(true).BestSize(wxSize(200,200)));
    m_mgr.AddPane(box_panel, wxAuiPaneInfo().
                  Name(wxT("box_panel")).Caption(wxT(MY_TXT("Show/hide options","Скрыть/показать"))).
                  Left().CloseButton(true).MaximizeButton(true).MinimizeButton(true).MaxSize(wxSize(300,250)).BestSize(wxSize(200,200)));
    m_mgr.AddPane(vd, wxAuiPaneInfo().
                  Name(wxT("vd")).Caption(wxT(MY_TXT("Volume data","Объёмные данные"))).
                  Left().CloseButton(true).MaximizeButton(true).MinimizeButton(true).MaxSize(wxSize(300,250)).BestSize(wxSize(200,200)));
/*    m_mgr.AddPane(rf, wxAuiPaneInfo().
                  Name(wxT("rf")).Caption(wxT(MY_TXT("Loading binary file","???"))).
                  Left().CloseButton(true).MaximizeButton(true).MinimizeButton(true).MaxSize(wxSize(300,250)));
*/
				   
    m_mgr.AddPane(fp, wxAuiPaneInfo().
                  Name(wxT("fp")).Caption(wxT(MY_TXT("Frames","Кадры"))).
                  Right().CloseButton(true).MaximizeButton(true).MinimizeButton(true).BestSize(wxSize(200,200)));


	tf = new tfInfoWindow(this);
    m_mgr.AddPane(tf, wxAuiPaneInfo().
                  Name(wxT("tf")).Caption(wxT(MY_TXT("Transfer function","???"))).
                  Bottom().CloseButton(true).MaximizeButton(true).BestSize(wxSize(200,200)));



	/*
	int i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (!all_panes.Item(i).IsToolbar())
            all_panes.Item(i).Hide();
    m_mgr.GetPane(wxT("m_canvas")).Show().CenterPane().BestSize(wxSize(400,400));
    m_mgr.GetPane(wxT("params")).Show().Right().BestSize(wxSize(300,300));
    m_mgr.GetPane(wxT("stereo_panel")).Show().Right().BestSize(wxSize(300,300));
    m_mgr.GetPane(wxT("box_panel")).Show().Right().BestSize(wxSize(300,300));
    m_mgr.GetPane(wxT("m_Html")).Show().Right().BestSize(wxSize(300,300));
    m_mgr.GetPane(wxT("tf")).Show().Bottom().BestSize(wxSize(500,300));
*/
    m_mgr.GetPane(wxT("stereo_panel")).Hide();
    m_mgr.GetPane(wxT("Log")).Hide();
    m_mgr.GetPane(wxT("vd")).Hide();

	m_mgr.Update();
	default_perspective = m_mgr.SavePerspective();
CreateStatusBar(2);
	
	timer.Start(20);

	OnMouseArrow(wxCommandEvent());
	for(int i=0;i<1;i++)m_canvas[i]->SetDefaultCursor();


	//

}
void MainFrame::Update_(bool self)
{
	
	stereo_panel->Update_(self);
	box_panel->Update_(self);
	perf_panel->Update_(self);
//	rf->Update_(self);
	fp->Update_(self);

	for(int i=0;i<3;i++)
		v2d[i]->Update1(self);
		
}
MainFrame::~MainFrame()
{
	//SaveProfile("default.invols");
	CT::KillCT();
    //delete m_canvas;
	m_mgr.UnInit();
	delete MyApp::GLContext;
}

// Intercept menu commands
void MainFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
	timer.Stop();
    Close(true);
}


void MainFrame::OnTimer(wxTimerEvent& event)
{
	for(int i=0;i<1;i++)
	{
		m_canvas[i]->is_timer_call=1;
		m_canvas[i]->MyRefresh();
		m_canvas[i]->is_timer_call=0;
	}
	for(int i=0;i<3;i++)
	{
		if(v2d[i]->need_Update1)
			v2d[i]->Update1();
		if(CT::need_rerender2d)
			v2d[i]->v2d->MyRefresh();
	}
	CT::need_rerender2d=0;

	if(CT::need_rerender_tf)		tf->tf->MyRefresh();	CT::need_rerender_tf=0;

/*
static bool rrr=1;
if(!rrr){rrr=1;
	CT::cur_src_dir = "./dt2/"; 
	CT::UpdateBinSrc();
	CT::iso->ReLoadShader();
	//CT::SetCurDataID(1);
//	tf->tf->ApplyQuads();
	
	
	CT::need_rerender_tf=1;
	CT::need_rerender=1;
	Update_(1);
}
*/
}





void MainFrame::OnAbout(wxCommandEvent& event)
{
	/*
	wxFrame *ab = new wxFrame(this,wxID_ANY,MY_TXT("About","О программе"));
	wxHtmlWindow*m_Html = new wxHtmlWindow(ab);
	ab->Maximize();
	m_Html->LoadPage("readme.html");
	ab->Show();
	*/

	    wxBoxSizer *topsizer;
    wxHtmlWindow *html;
    wxDialog dlg(this, wxID_ANY, wxString(_("About InVols")));
	dlg.SetBackgroundColour(wxColor(0xFFFFFFFF));

    topsizer = new wxBoxSizer(wxVERTICAL);

    html = new wxHtmlWindow(&dlg, wxID_ANY, wxDefaultPosition, wxSize(380, 160), wxHW_SCROLLBAR_NEVER);
    html -> SetBorders(0);
    html -> LoadPage(wxT("doc/about.html"));
    html -> SetSize(html -> GetInternalRepresentation() -> GetWidth(),
                    html -> GetInternalRepresentation() -> GetHeight());

	
    topsizer -> Add(html, 1, wxALL, 10);

//    topsizer -> Add(new wxStaticLine(&dlg, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);


    wxButton *bu1 = new wxButton(&dlg, wxID_OK, _("OK"));
    bu1 -> SetDefault();

    topsizer -> Add(bu1, 0, wxALL | wxALIGN_RIGHT, 15);

    dlg.SetSizer(topsizer);
    topsizer -> Fit(&dlg);

    dlg.ShowModal();
	CT::need_rerender=1;
}
void MainFrame::OnSaveProfile(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Save Workspace","Сохранить профиль"),"./Workspaces","","*.invols",wxSAVE);

	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		SaveProfile(fileName.c_str());
		
	}
	delete openFileDialog;
}
void MainFrame::OnSwitchProjection(wxCommandEvent& event)
{
	CT::SetProjection(!CT::cam.GetProjection());
}

void MainFrame::OnLoadDicomDir(wxCommandEvent& event)
{
	wxDirDialog *d = new wxDirDialog( this, _(MY_TXT("Choose folder with the DICOM-files","Выберите папку с dicom-файлами")), DicomReader::cur_dicom_dir, 0, wxDefaultPosition );
	
 	if ( d->ShowModal() ==  wxID_OK )
	{
		CT::cur_loaded_file = d->GetPath(); 
		CT::cur_loaded_file_type=1;
		CT::LoadDicomDir(CT::cur_loaded_file);
		if(auto_gpu_upload)	OnLoadToGPU(event);


		Update_(1);

	}
	delete d;
}

void MainFrame::OnLoadBinDir(wxCommandEvent& event)
{
	wxDirDialog *d = new wxDirDialog( this, _(MY_TXT("Choose folder with the binary files","???")), CT::cur_src_dir[CT::GetCurDataID()], 0, wxDefaultPosition );
	
 	if ( d->ShowModal() ==  wxID_OK )
	{
		CT::cur_src_dir[CT::GetCurDataID()] = d->GetPath(); 
		CT::UpdateBinSrc();
		Update_(1);
		CT::need_rerender=1;
		CT::need_rerender2d=1;
		
	}
	delete d;
}
void MainFrame::MyMessageBox(wxString mess)
{
	wxMessageBox(mess,MY_TXT("Message","Сообщение"),4|wxCENTRE,this);
}
void MainFrame::OnLoadToGPU(wxCommandEvent& event)
{
	if(CPU_VD::full_data.GetSlices())
	{
		CT::need_reload_volume_data=1;
		CT::need_rerender=1;
	}
	else
		MyMessageBox(MY_TXT("There is no data to load","Нет данных для загрузки"));
}
void MainFrame::OnLoadProfile(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Load Workspace","Открыть профиль"),"./Workspaces","","*.invols",wxOPEN);

	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		LoadProfile(fileName.c_str());
	}
	delete openFileDialog;

}

void MainFrame::OnAnimation(wxCommandEvent& event)
{
	
	CT::action_is = !CT::action_is;
	

}
void MainFrame::OnRMPicView(wxCommandEvent& event)
{
	CT::RM_pic_man = !CT::RM_pic_man;
	tb2->ToggleTool(MYACT_RM_PIC_VIEW, CT::RM_pic_man);
	tb2->Refresh();
	CT::need_rerender=1;
}
void MainFrame::OnFullScreen(wxCommandEvent& event)
{
	static bool bb=1;
	static std::vector<int> shown_panes;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
 
	if(bb)
	{
		shown_panes.clear();
		int i, count;
		for (i = 0, count = all_panes.GetCount(); i < count; ++i)
			if(all_panes.Item(i).IsShown()){shown_panes.push_back(i); all_panes.Item(i).Hide();}
		 m_mgr.GetPane(wxT("m_canvas0")).Show();
			m_mgr.Update();
	}

	m_canvas[0]->OnFullScreen();
	
	if(!bb)
	{
		for(int i=0;i<shown_panes.size();i++)
			all_panes.Item(shown_panes[i]).Show();
			m_mgr.Update();

	}

	bb=!bb;
}

wxColour MainFrame::SelectColour()
{
    wxColour col(0,0,0);
    wxColourData data;
    wxColourDialog dialog(this, &data);

    if ( dialog.ShowModal() == wxID_OK )
    {
        col = dialog.GetColourData().GetColour();
    }

    return col;
}
void MainFrame::UntoggleMouseToolbars()
{
	tb3->ToggleTool(MYACT_MOUSE_ARROW, 0);
	tb3->ToggleTool(MYACT_MOUSE_TRANSLATE, 0);
	tb3->ToggleTool(MYACT_MOUSE_ROTATE, 0);
	tb3->ToggleTool(MYACT_MOUSE_ZOOM, 0);
	tb3->ToggleTool(MYACT_MOUSE_BC, 0);
	tb3->ToggleTool(MYACT_MOUSE_MEASURE_LENGTH, 0);

	for(int i=0;i<1;i++)m_canvas[i]->SetDefaultCursor();

}
void MainFrame::OnMouseArrow(wxCommandEvent& event)
{
	CT::MouseButtonFunction = CT::MouseArrow;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_ARROW, 1);
	tb3->Refresh();
	for(int i=0;i<3;i++)v2d[i]->OnToggleArrow(event);
	tf->OnToggleArrow(event);

}
void MainFrame::OnMouseRotate(wxCommandEvent& event)
{
	CT::MouseButtonFunction = CT::MouseRotate;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_ROTATE, 1);
	

}

void MainFrame::OnMouseTranslate(wxCommandEvent& event)
{
	CT::MouseButtonFunction = CT::MouseTranslate;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_TRANSLATE, 1);

	for(int i=0;i<3;i++)v2d[i]->OnToggleTranslate(event);
	tf->OnToggleTranslate(event);

}
//
void MainFrame::OnMouseZoom(wxCommandEvent& event)
{	
	CT::MouseButtonFunction = CT::MouseZoom;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_ZOOM, 1);

	for(int i=0;i<3;i++)v2d[i]->OnToggleZoom(event);
	tf->OnToggleZoom(event);
}
void MainFrame::OnMouseBC(wxCommandEvent& event)
{
	CT::MouseButtonFunction = CT::MouseBC;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_BC, 1);
	for(int i=0;i<3;i++)v2d[i]->OnToggleBC(event);
	tf->OnToggleBC(event);
	
}
void MainFrame::OnMouseMeasureLength(wxCommandEvent& event)
{
	CT::MouseButtonFunction = CT::MouseMeasureLength;
	UntoggleMouseToolbars();
	tb3->ToggleTool(MYACT_MOUSE_MEASURE_LENGTH, 1);
	for(int i=0;i<3;i++)v2d[i]->OnToggleMeasureLength(event);
	
}
void MainFrame::OnShowAllPanes(wxCommandEvent& event)
{
	int i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (!all_panes.Item(i).IsToolbar())            all_panes.Item(i).Show();
	m_mgr.Update();
}
void MainFrame::OnShowAllTools(wxCommandEvent& event)
{
	int i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (all_panes.Item(i).IsToolbar())            all_panes.Item(i).Show();
	m_mgr.Update();
}
void MainFrame::OnGoHome(wxCommandEvent& event)
{
	CT::cam.SetCenter(CT::GetCenter());
	CT::cam.SetDistance(3);
	CT::need_rerender=1;
}
void MainFrame::OnLoadDefaultPerspective(wxCommandEvent& event)
{
	m_mgr.LoadPerspective(default_perspective);
}


void MainFrame::OnApplyMedianFilter(wxCommandEvent& event)
{
	segmentation::MedianFilter(CPU_VD::full_data);
	if(auto_gpu_upload)	OnLoadToGPU(event);
	Update_(1);
}
void MainFrame::OnApplyGaussFilter(wxCommandEvent& event)
{
	segmentation::GaussFilter(CPU_VD::full_data);
	if(auto_gpu_upload)	OnLoadToGPU(event);
	Update_(1);
}
void MainFrame::OnApplyResampling(wxCommandEvent& event)
{
	segmentation::Resample();
}

void MainFrame::LoadProfile(wxString fname)
{
	wxFile fs(fname,wxFile::read);
	if(fs.Error())return;

	int pv;
	OpenItem(fs,pv);
	if(pv!=PROFILE_VERSION){output::application.println("wrong Workspace version!");return;}

//	rf->Load(fs);
	CT::LoadProfile(fs);
	tf->Load(fs);
	for(int i=0;i<3;i++)v2d[i]->Load(fs);

	wxString perspective;
	OpenString(fs,perspective);
	m_mgr.LoadPerspective(perspective);

	Update_(1);
	fs.Close();
}

void MainFrame::SaveProfile(wxString fname)
{
	wxFile fs(fname,wxFile::write);

	int pv=PROFILE_VERSION;
	SaveItem(fs,pv);
		
//	rf->Save(fs);
	CT::SaveProfile(fs);
	tf->Save(fs);
	for(int i=0;i<3;i++)v2d[i]->Save(fs);

	wxString perspective = m_mgr.SavePerspective();
	SaveString(fs,perspective);
		
	fs.Close();
}


void MainFrame::OnLightToCamera(wxCommandEvent& event)
{
	CT::light_to_camera = !CT::light_to_camera;
	if(CT::light_to_camera)CT::need_rerender=1;
}
void MainFrame::OnSaveVD(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Save volume data","Сохранить объёмные данные"),"./Datasets/","","*.raw",wxSAVE);
	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		
		wxFile fs2(fileName+".txt",wxFile::write);
		std::string outs = "size: ";
		outs += str::ToString(CPU_VD::full_data.GetSize().x)+" ";
		outs += str::ToString(CPU_VD::full_data.GetSize().y)+" ";
		outs += str::ToString(CPU_VD::full_data.GetSize().z)+" spacing: ";
		outs += str::ToString(CPU_VD::full_data.spacing.x)+" ";
		outs += str::ToString(CPU_VD::full_data.spacing.y)+" ";
		outs += str::ToString(CPU_VD::full_data.spacing.z)+" ";
		//SaveString(fs2,outs);
		fs2.Write(outs.c_str(),outs.size());
		fs2.Close();

		wxFile fs3(fileName,wxFile::write);
		for(int i=0;i<CPU_VD::full_data.GetSize().z;i++)
			fs3.Write((char*)CPU_VD::full_data.GetSlice(i),2*CPU_VD::full_data.GetSize().x*CPU_VD::full_data.GetSize().y);
		fs3.Close();
	}
	delete openFileDialog;
}

void MainFrame::OnLoadVD(wxCommandEvent& event)
{
	wxFileDialog * openFileDialog = new wxFileDialog(this,MY_TXT("Load volume data","Загрузить объёмные данные"),"./Datasets/","","*.raw",wxOPEN);
	if(openFileDialog->ShowModal() == wxID_OK)
	{
		wxString fileName = openFileDialog->GetPath();
		wxFile fs(fileName,wxFile::read);
		if(!fs.Error())
		{
			ivec3 data_size = CPU_VD::full_data.GetSize();
			vec3 data_spacing = CPU_VD::full_data.spacing;
			CT::LoadRawDataMetrics( fileName, data_size, data_spacing);
			//

			CT::LoadRawFile(fileName,data_size,data_spacing);
			if(auto_gpu_upload)	OnLoadToGPU(event);
			Update_(1);
			fs.Close();
		}
		
	}
	delete openFileDialog;

}
void MainFrame::OnUseBoundingMesh(wxCommandEvent& event)
{
	CT::use_bounding_mesh = !CT::use_bounding_mesh;
	CT::seg_BuildBox(vec3(0),vec3(1));
	CT::iso->ReLoadShader();
	CT::need_rerender=1;
}
void MainFrame::OnAutoGPUUpload(wxCommandEvent& event)
{
	auto_gpu_upload=!auto_gpu_upload;
}

void MainFrame::OnDotLightToCamera(wxCommandEvent& event)
{
//	CT::iso->SetDotLight(CT::cam.GetPosition());

}
void MainFrame::OnBGColor(wxCommandEvent& event)
{
	wxColour cc = SelectColour();
	CT::bg_color.set(cc.Red()/256.0f,cc.Green()/256.0f,cc.Blue()/256.0f);
	CT::need_rerender=1;
	CT::need_rerender2d=1;
}
/*
void MainFrame::OnDropShadows(wxCommandEvent& event)
{
	CT::iso->drop_shadows = !CT::iso->drop_shadows;
	CT::iso->ReLoadShader();
	
}*/

#include "Progress.h"
void MainFrame::OnDropShadows(wxCommandEvent& event)
{
	bool stereo=CT::keyboard['C'];
	wxString cur_dir = "./pic";
	MyApp::frame->OnFullScreen(wxCommandEvent());
	
	Progress progress(wxT("Making pictures..."));

//	if(m_checkbox_fullscreen->IsChecked())


	//MyApp::frame->timer.Stop();
	char fn[]="000000.png";
	int pcpc = (stereo?2:1);
	unsigned char *dt = new unsigned char [CT::width*CT::height*3 * pcpc];
	
	glReadBuffer(GL_BACK);
//	if (!MyApp::frame->m_canvas[0]->GetContext()) return;
	//MyApp::frame->m_canvas[0]->SetCurrent();
	//wxPaintDC dc(MyApp::frame->m_canvas[0]);
	//CT::cam.left_eye=1;
	

	for(int i=0;i<CT::FramesNum();i++)
	{
		Progress::inst->SetPercent(i/float(CT::FramesNum()));
		CT::SetFrame(i);
		
//glViewport(0,0,CT::width, CT::height);
		CT::FullDraw();
		//
		
		glFlush();
		//glFinish();
		glReadPixels((GLint)0, (GLint)0, (GLint)CT::width, (GLint)CT::height, GL_RGB, GL_UNSIGNED_BYTE, dt);
		
		if(pcpc==2)
		{
		//	CT::cam.left_eye=0;
			CT::FullDraw();
		//	CT::cam.left_eye=1;
			glFlush();
			glReadPixels((GLint)0, (GLint)0, (GLint)CT::width, (GLint)CT::height, GL_RGB, GL_UNSIGNED_BYTE, dt+CT::width*CT::height*3);
		
		}
		wxImage img(CT::width,CT::height*pcpc,dt,true);
		wxBitmap bp(img);
		fn[2]='0'+(i/1000)%10;		fn[3]='0'+(i/100)%10;		fn[4]='0'+(i/10)%10;		fn[5]='0'+i%10;

		//bp.SaveFile(fn,wxBITMAP_TYPE_BMP);
		bp.SaveFile(cur_dir+"/"+fn,wxBITMAP_TYPE_PNG);
//		if(CT::cam.eye_distance<0)break;
	}

//	delete[]dt;
	//MyApp::frame->timer.Start();
	//if(m_checkbox_fullscreen->IsChecked())
	MyApp::frame->OnFullScreen(wxCommandEvent());
}