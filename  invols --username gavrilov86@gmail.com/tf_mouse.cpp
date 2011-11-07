#include "CT/CT.h"
#include "Draw2D.h"
#include "CPU_VD.h"

#include "wxIncludes.h"
#include "MainFrame.h"


//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "TF_window.h"
#include "drawtext.h"
#include "Img3DProc.h"


void TF_window::PutLine(int x1,int y1,int x2,int y2)
{
	double scale = tf_scale[CT::GetCurDataID()];
	double center = tf_center[CT::GetCurDataID()];
	vec2 win = CT::iso->GetWindow();
	x1 = TF_WIDTH*(x1/scale+center - win.x)/(win.y-win.x);
	x2 = TF_WIDTH*(x2/scale+center - win.x)/(win.y-win.x);

//int cx = ((i/float(TF_WIDTH))*(win.y-win.x)+win.x-center)*scale;

	int height1=height-TF_MARGIN*2;
	float nn;
	y1 = clamp(0,height1-1,y1);
	y2 = clamp(0,height1-1,y2);
	if(x1>x2){swap(x1,x2);swap(y1,y2);}
	if(x1<0)x1=0;
	if(x2>=TF_WIDTH)x2=TF_WIDTH-1;
	if(x1==x2)nn=1;else nn=x2-x1;
	for(int i=x1;i<=x2;i++)
	{
		float yy = y1+(y2-y1)*(i-x1)/nn;
		float val = (height1-yy)/float(height1);
		/*
		if(check_box[0]->IsChecked()) tf[i].x = val;
		if(check_box[1]->IsChecked()) tf[i].y = val;
		if(check_box[2]->IsChecked()) tf[i].z = val;
		if(check_box[3]->IsChecked()) tf[i].w = val;
		*/
	}
	

}

void TF_window::OnMouseEvent(wxMouseEvent& event)
{
	
    static int last_x, last_y;
	static int state;
	double& scale = tf_scale[CT::GetCurDataID()];
	double& center = tf_center[CT::GetCurDataID()];

	int height1 = height-TF_MARGIN*2;
	
	this->SetFocus();
	float real_x = (event.GetX())/(scale)+center;
	float real_y = 1.0f-(event.GetY()-TF_MARGIN)/float(height1);

		if(CT::keyboard['H']){hist2d_br = max(1,real_y*50+1);}


	mouse_left_is_down = event.LeftIsDown();


	if(event.GetX()!=last_x || event.GetY()!=last_y)
	{
		wxLogStatus(MyApp::frame, _T("%g"), (event.GetX()/scale+center)*(256*(CPU_VD::full_data.GetValueFormat()==0?128:1))+data_stat.MinValue);

		if(event.MiddleIsDown() /*|| event.RightIsDown()*/ || (event.LeftIsDown() && cur_action == TF_ACTION_TRANSLATE))
		{
			center -= (event.GetX()-last_x)/scale;
			MyRefresh();
		}

		
		if(event.LeftIsDown())
		{
			if(cur_action == TF_ACTION_ZOOM)
			{
				float ps = last_x/scale+center;
				scale*=((event.GetY()>last_y)?1.1f:0.9f);
				center = ps-last_x/scale;
				MyRefresh();
			}
			if(cur_action == TF_ACTION_BC)
			{
				float dl = (event.GetX()-last_x)/scale;
				float dw = (event.GetY()-last_y)/(scale);
				CT::iso->DragWindow(CT::iso->GetWindow()+vec2(dl-dw,dl+dw));
				CT::need_rerender=1;
				CT::need_rerender2d=1;
				CT::need_rerender_tf=1;
			}
			if(cur_action == TF_ACTION_ARROW)
			{
				if(selected_obj==SELECTION_OBJECT_TF_POINT)
				{
					(*tf_points)[selected_id].color.w = clamp(0.0f,1.0f,real_y);
					(*tf_points)[selected_id].value = real_x;

					CT::iso->GetCurRM()->ApplyPoints();
					MyRefresh();
					CT::need_rerender=1;

				}else
				if(selected_obj==SELECTION_OBJECT_ISO)
				{
					if(selected_part)
						(*isos)[selected_id].color.w = clamp(0.0f,1.0f,real_y);
					else
						(*isos)[selected_id].value = real_x;

					CT::iso->UpdateUniforms();
					CT::iso->GetCurRM()->ApplyPoints();
					MyRefresh();
					CT::need_rerender=1;

				}else
				if(SELECTION_OBJECT_LEVEL==selected_obj)
				{
					vec2 ww=CT::iso->GetWindow();
					if(selected_id)
					{
						ww.y = real_x;
						if(ww.y<ww.x)ww.x=ww.y;
					}
					else
					{
						ww.x = real_x;
						if(ww.y<ww.x)ww.y=ww.x;

					}
					CT::iso->SetWindow(ww);
					CT::need_rerender=1;
					CT::need_rerender2d=1;
					MyRefresh();
				}else/*
				if(manual_tf_edit)
				{
					if(check_box[4]->IsChecked() || check_box[5]->IsChecked())
					
					{
						PutLine(event.GetX(),event.GetY()-TF_MARGIN,last_x,last_y-TF_MARGIN);
						CT::iso->ApplyColorTable();
						MyRefresh();
					}
				}*/
				if(SELECTION_OBJECT_QUAD==selected_obj)
				{
					if(selected_part==0)
					{
						(*tf_quads)[selected_id].l1 = real_x;
						if((*tf_quads)[selected_id].l2<real_x) (*tf_quads)[selected_id].l2 = real_x;
					}
					if(selected_part==1)
					{
						(*tf_quads)[selected_id].l2 = real_x;
						if((*tf_quads)[selected_id].l1>real_x) (*tf_quads)[selected_id].l1 = real_x;
					}
					if(selected_part==2)
					{
						//(*tf_quads)[selected_id].max_gm = clamp(0.0f,1.0f,real_y);
						(*tf_quads)[selected_id].max_gm = real_y;
						(*tf_quads)[selected_id].min_gm = min((*tf_quads)[selected_id].max_gm,(*tf_quads)[selected_id].min_gm);
					}
					if(selected_part==3)
					{
						float dx1 = (event.GetX()-last_x)/scale;
						float dy1 = (event.GetY()-last_y)/(float)height1;
						(*tf_quads)[selected_id].l1 += dx1;
						(*tf_quads)[selected_id].l2 += dx1;

						(*tf_quads)[selected_id].min_gm -= dy1;
						(*tf_quads)[selected_id].max_gm -= dy1;
						//(*tf_quads)[selected_id].max_gm = clamp(0.0f,1.0f,(*tf_quads)[selected_id].max_gm);
						//(*tf_quads)[selected_id].min_gm = clamp(0.0f,1.0f,(*tf_quads)[selected_id].min_gm);
					}
					if(selected_part==4)
					{
						//(*tf_quads)[selected_id].min_gm = clamp(0.0f,1.0f,real_y);
						(*tf_quads)[selected_id].min_gm = real_y;
						(*tf_quads)[selected_id].max_gm = max((*tf_quads)[selected_id].max_gm,(*tf_quads)[selected_id].min_gm);
					}
					CT::iso->UpdateUniforms();
					CT::iso->GetCurRM()->ApplyQuads();
					CT::need_rerender=1;
					MyRefresh();
				}
			}
		}
		/////////////// hover
		int old_s = sel_obj + sel_part*100 + sel_id*10;
		sel_obj=-1;

		if(cur_action == TF_ACTION_ARROW)
		{
			{
				
				vec2 ww=CT::iso->GetWindow();
				if(	abs((ww.x-center)*scale - event.GetX())<4) 
				{
					sel_obj = SELECTION_OBJECT_LEVEL;
					sel_id = 0;
				}
				if(	abs((ww.y-center)*scale - event.GetX())<4)
				{
					sel_obj = SELECTION_OBJECT_LEVEL;
					sel_id = 1;
				}
						
			}
			
			if(sel_obj==-1)
			for(int i=0;i<tf_points->size();i++)
			{
				int pt_w=8;
				int cx = ((*tf_points)[i].value-center)*scale;
				if(cx > last_x-pt_w && cx < last_x+pt_w)
				{
					int v1 = (1.0-(*tf_points)[i].color.w)*height1;
					int v2 = event.GetY()-TF_MARGIN;
					if(v1>v2-pt_w && v1<v2+pt_w)
					{
						sel_obj = SELECTION_OBJECT_TF_POINT;
						sel_id = i;
						break;
					}
				}
			}
			if(sel_obj==-1)
			for(int i=0;i<isos->size();i++)
			{
				int cx = ((*isos)[i].value-center)*scale;
				if(cx > last_x-5 && cx < last_x+5)
				{
					sel_obj = SELECTION_OBJECT_ISO;
					sel_id = i;
					int v1 = (1.0-(*isos)[sel_id].color.w)*height1;
					int v2 = event.GetY()-TF_MARGIN;
					sel_part = (v1>v2-5 && v1<v2+5);
					break;
				}
			}
			
		
			if(sel_obj==-1)
				for(int i=0;i<tf_quads->size();i++)
				{
					TF_QUAD qq = (*tf_quads)[i];
					int cx1 = (qq.l1-center)*scale;
					int cx2 = (qq.l2-center)*scale;
					int l2=(1-qq.min_gm)*height1+TF_MARGIN;
					int l1=(1-qq.max_gm)*height1+TF_MARGIN;
					if(event.GetY()>l1-TF_QUAD_DRAG_BORDER && event.GetY()<l2+TF_QUAD_DRAG_BORDER && event.GetX()>cx1-TF_QUAD_DRAG_BORDER && event.GetX()<cx2+TF_QUAD_DRAG_BORDER)
					{
						sel_part = 3;
						if(event.GetX()<cx1+TF_QUAD_DRAG_BORDER)
							sel_part = 0;
						if(event.GetX()>cx2-TF_QUAD_DRAG_BORDER)
							sel_part = 1;
						if(event.GetY()<l1+TF_QUAD_DRAG_BORDER)
							sel_part = 2;
						if(event.GetY()>l2-TF_QUAD_DRAG_BORDER)
							sel_part = 4;
						sel_obj = SELECTION_OBJECT_QUAD;
						sel_id = i;
						
					}
				}
			//

			
		}
		if(old_s != sel_obj + sel_part*100 + sel_id*10)
			MyRefresh();
	}
	
	if(event.IsButton())
	{
		if(event.RightDown())
		{
			if(sel_obj==-1)
			{
				if(CT::iso->GetCurRM()->AddPoint(vec2(real_x,real_y)))
				{
					selected_obj = SELECTION_OBJECT_TF_POINT;
					selected_id = tf_points->size()-1;
					
				}
			}else
			{
				if(sel_obj == SELECTION_OBJECT_TF_POINT)
					CT::iso->GetCurRM()->DeletePoint(sel_id);
				if(sel_obj == SELECTION_OBJECT_QUAD)
					CT::iso->GetCurRM()->DeleteQuad(sel_id);
				if(sel_obj == SELECTION_OBJECT_ISO)
					CT::iso->GetCurRM()->DeleteIso(sel_id);
				CT::need_rerender_tf=1;
				CT::need_rerender=1;
				selected_obj=-1;
				sel_obj=-1;

			}
		}
		if(event.LeftDown())
		{
			
			selected_obj = sel_obj;
			selected_id = sel_id;
			selected_part = sel_part;
			if(cur_creation == TF_ACTION_ADD_ISO)
			{
				if(CT::iso->GetCurRM()->AddIso(vec2(real_x,real_y)))
				{
	//				MyRefresh();
					par->SetButtonValue(par->tbtn_add_iso,0);
					selected_obj = SELECTION_OBJECT_ISO;
					selected_id = isos->size()-1;
					selected_part = 0;
				}
			}
			if(cur_creation == TF_ACTION_ADD_QUAD)
			{
				if(CT::iso->GetCurRM()->AddQuad(vec2(real_x,real_y)))
				{
//					MyRefresh();
					par->SetButtonValue(par->tbtn_add_quad,0);
					selected_obj = SELECTION_OBJECT_QUAD;
					selected_id = tf_quads->size()-1;
					selected_part = 1;

				}
			}
			cur_creation=-1;
		}
		
		MyRefresh();
	}
	
	if(event.m_wheelRotation)
	{
		float ps = last_x/scale+center;
		scale*=(event.m_wheelRotation>0?1.1f:0.9f);
		center = ps-last_x/scale;
		MyRefresh();
	}

	bool old_ctf=CT::is_changing_tf;
	CT::is_changing_tf = mouse_left_is_down && (selected_id!=-1);
	if(old_ctf!=CT::is_changing_tf)CT::need_rerender=1;


	last_x = event.GetX();
	last_y = event.GetY();
	UpdateCursor();

}
