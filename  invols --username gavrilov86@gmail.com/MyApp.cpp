#include "CT/CT.h"
#include "MyApp.h"

#include "GLCanvas.h"
#include "AllDef.h"
#include "MainFrame.h"

#include "wxIncludes.h"
#include "GLCanvas.h"

IMPLEMENT_APP(MyApp)

MainFrame* MyApp::frame=0;
wxGLContext* MyApp::GLContext=0;
int* MyApp::gl_attributes=0;

// `Main program' equivalent, creating windows and returning main app frame

bool MyApp::OnInit()
{
//    Args(argc, argv);

	
    // Create the main frame window
  frame = new MainFrame(NULL, wxT("InVols Viewer"), wxDefaultPosition, wxSize(800,500));
  frame->Maximize(true);
  wxImage::AddHandler(new wxPNGHandler);
 
    SetTopWindow(frame);
    //frame->SetIcon(wxICON(sample));

		
	//wxBitmap btm(wxString("./Images/logo16.bmp"),wxBITMAP_TYPE_ANY)
	//wxSize icon_size(16,16);
	
	frame->SetIcon(wxIcon("./Images/InVols_logo2_32.png",wxBITMAP_TYPE_PNG, 32,32));

	wxMenuBar *menuBar = new wxMenuBar;	

	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(MYACT_SAVE_PROFILE, _T(MY_TXT("Save Workspace","��������� �������")));
	fileMenu->Append(MYACT_LOAD_PROFILE, _T(MY_TXT("Load Workspace","��������� �������")));
	fileMenu->Append(MYACT_SETDEFAULT_PROFILE, _T(MY_TXT("Set as default Workspace","������� ���� ������� �� ���������")));
	fileMenu->AppendSeparator();

	fileMenu->Append(MYACT_LOAD_DICOMDIR, _T(MY_TXT("Open folder with DICOM files","������� ����� � DICOM �������")));
//ipfran 	fileMenu->Append(MYACT_LOAD_BINDIR, _T(MY_TXT("Open folder with *.bin files","������� ����� � *.bin �������")));
	fileMenu->Append(MYACT_SAVE_VD, _T(MY_TXT("Save to *.raw","???")));
	fileMenu->Append(MYACT_LOAD_VD, _T(MY_TXT("Load *.raw file","???")));

	fileMenu->Append(MYACT_EXIT, _T(MY_TXT("Exit","�����")));
    menuBar->Append(fileMenu, _T(MY_TXT("&File","&����")));
    frame->SetMenuBar(menuBar);

	
    fileMenu = new wxMenu;
	fileMenu->Append(MYACT_BGCOLOR, _T(MY_TXT("Background colour","���� ����")));
    menuBar->Append(fileMenu, _T(MY_TXT("&View","&���")));
    frame->SetMenuBar(menuBar);

	fileMenu = new wxMenu;
	fileMenu->Append(MYACT_SHOW_ALL_PANES, _T(MY_TXT("Show all panels","�������� ��� ������")));
	fileMenu->Append(MYACT_SHOW_ALL_TOOLS, _T(MY_TXT("Show all tools","�������� ��� ����������")));
	fileMenu->Append(MYACT_LOAD_DEF_PER, _T(MY_TXT("Load defallt perspective","����������� ���� �� ���������")));
    menuBar->Append(fileMenu, _T(MY_TXT("Windows","&����")));
    frame->SetMenuBar(menuBar);

	fileMenu = new wxMenu;
	fileMenu->Append(MYACT_APPLY_MEDIAN_FILTER, _T(MY_TXT("Median filter","��������� ������")));
	fileMenu->Append(MYACT_APPLY_GAUSS_FILTER, _T(MY_TXT("Gaussian filter","������ ������")));
	//fileMenu->Append(MYACT_APPLY_RESAMPLING, _T(MY_TXT("Resample","�������� ����������")));
    menuBar->Append(fileMenu, _T(MY_TXT("Data processing","��������� ������")));
    frame->SetMenuBar(menuBar);


    fileMenu = new wxMenu;
	fileMenu->Append(MYACT_ABOUT, _T(MY_TXT("About","� ���������...")));
    menuBar->Append(fileMenu, _T(MY_TXT("&Help","&������")));
    frame->SetMenuBar(menuBar);

    frame->Show(true);
	

    frame->m_canvas[0]->SetCurrent();
   	frame->LoadProfile("default.invols");

		CT::need_rerender=1;

    return true;
}


