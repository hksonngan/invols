#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"


//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "tf2D.h"
#include "drawtext.h"
#include "Img3DProc.h"



void TF2D_window::OnMouseEvent(wxMouseEvent& event)
{
	
    static int last_x, last_y;
	static int state;

	int height1 = height-TF2D_MARGIN*2;
	
	this->SetFocus();
	float real_x = (event.GetX())/(scale)+center;
	float real_y = 1.0f-(event.GetY()-TF2D_MARGIN)/float(height1);
	mouse_left_is_down=event.LeftIsDown();
	if(event.GetX()!=last_x || event.GetY()!=last_y)
	{
		wxLogStatus(MyApp::frame, _T("%g"), (event.GetX()/scale+center)*(256*128)+data_stat.MinValue);

		if(event.MiddleIsDown() || event.RightIsDown() || (event.LeftIsDown() && cur_action == TF2D_ACTION_TRANSLATE))
		{
			center -= (event.GetX()-last_x)/scale;
			MyRefresh();
		}

		
		if(event.LeftIsDown())
		{
			if(cur_action == TF2D_ACTION_ZOOM)
			{
				float ps = last_x/scale+center;
				scale*=((event.GetY()>last_y)?1.1f:0.9f);
				center = ps-last_x/scale;
				MyRefresh();
			}
			if(cur_action == TF2D_ACTION_BC)
			{
				
			}
			
		}
		
	}
	
	
	if(event.m_wheelRotation)
	{
		float ps = last_x/scale+center;
		scale*=(event.m_wheelRotation>0?1.1f:0.9f);
		center = ps-last_x/scale;
		MyRefresh();
	}
	

	last_x = event.GetX();
	last_y = event.GetY();
	UpdateCursor();

}
