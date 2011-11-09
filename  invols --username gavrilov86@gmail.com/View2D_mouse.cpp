#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"


#include "AllDef.h"
#include "View2D.h"
#include "drawtext.h"
#include "Img3DProc.h"
#include "CPU_VD.h"

bool AnearB(vec2 a,vec2 b,float w)
{
	return ((a.x>=b.x-w)&&(a.y>=b.y-w)&&(a.x<=b.x+w)&&(a.y<=b.y+w));
}
bool AinsideBC(vec2 a,vec2 b,vec2 c)
{
	return ((a.x>=b.x)&&(a.y>=b.y)&&(a.x<=c.x)&&(a.y<=c.y));
}

void View2D::OnMouseEvent(wxMouseEvent& event)
{
	
    static vec2 last_pos;

	vec2 full_size = Get2DPoint(CPU_VD::GetFullSize());

	this->SetFocus();
	float hscale = 1.0f/(height+0.0001f);
	vec2 mouse_pos(event.GetX(),event.GetY());

	vec2 real_pos = (mouse_pos)*(hscale/scale)+center;
	static vec2 real_pos_old = real_pos;
	mouse_left_is_down=event.LeftIsDown();
	if(!(mouse_pos==last_pos))
	{
		//wxLogStatus(MyApp::frame, _T("%g"), (event.GetX()/scale+center)*(256*128)+data_stat.MinValue);

		

		if(event.MiddleIsDown() || event.RightIsDown() || (event.LeftIsDown() && cur_action == V2D_ACTION_TRANSLATE))
		{
			center -= ((mouse_pos-last_pos))*mirror*(hscale/scale);
			//center -= real_pos-real_pos_old;
			MyRefresh();
		}

		
		if(event.LeftIsDown())
		{
			if(cur_action == V2D_ACTION_ZOOM)
			{
				scale*=((mouse_pos.y>last_pos.y)?1.1f:0.9f);
				center = real_pos-(last_pos)*(hscale/scale);

				MyRefresh();
			}
			if(cur_action == V2D_ACTION_BC)
			{
				float dw = (event.GetX()-last_pos.x)*0.0001;
				float dl = (event.GetY()-last_pos.y)*0.0001;
				//vec2 oww = CT::iso->GetWindow();
				bc += vec2(dl-dw,dl+dw);
				
				CT::need_rerender2d=1;
				
			}
			if(cur_action == V2D_ACTION_ARROW)
			{
				if(selected_obj==V2D_SELECTION_OBJECT_CENTER)
				{
					
					CT::need_rerender2d=1;
					vec3 center1 = CT::GetCenter();
					
					if(selected_part==V2D_SELECTION_PART_X || selected_part==V2D_SELECTION_PART_CENTER)
					{
						center1.SetByID(axis_y,clamp(0.0f,1.0f,real_pos.y/full_size.y));
						MyApp::frame->v2d[axis_y]->need_Update1=1;
					}
					if(selected_part==V2D_SELECTION_PART_Y || selected_part==V2D_SELECTION_PART_CENTER)
					{
						center1.SetByID(axis_x,clamp(0.0f,1.0f,real_pos.x/full_size.x));
						MyApp::frame->v2d[axis_x]->need_Update1=1;
					}
					CT::SetCenter(center1);
				}
		
				if(selected_obj==V2D_SELECTION_OBJECT_BB)
				{
					vec2 b1 = Get2DBoxMin(), b2 = Get2DBoxMax();

					if(selected_part==V2D_SELECTION_PART_POINT)
					{
						if(selected_id==0)
							b1 = real_pos;
						else if(selected_id==1)
						{b1.y = real_pos.y;b2.x = real_pos.x;}
						else if(selected_id==2)
						{b2.y = real_pos.y;b1.x = real_pos.x;}
						else if(selected_id==3)
							b2 = real_pos;
					}
					else
					if(selected_part==V2D_SELECTION_PART_EDGE)
					{
						if(selected_id==0)
							b1.y = real_pos.y;
						else if(selected_id==1)
							b2.y = real_pos.y;
						else if(selected_id==2)
							b1.x = real_pos.x;
						else if(selected_id==3)
							b2.x = real_pos.x;

					}
					else
					if(selected_part==V2D_SELECTION_PART_FACE)
					{
						b1 += real_pos-real_pos_old;
						b2 += real_pos-real_pos_old;
					}

					if(b2.x < b1.x) b2.x = b1.x = (b2.x + b1.x)*0.5f;
					if(b2.y < b1.y) b2.y = b1.y = (b2.y + b1.y)*0.5f;

					CT::iso->SetBoundingBox(Get3DPoint(b1,CT::iso->GetBoundingBox(0).GetByID(axis)),Get3DPoint(b2,CT::iso->GetBoundingBox(1).GetByID(axis)));


					MyRefresh();
				
				}
			}
		}
			
	}
		/////////////// hover
		int old_s = sel_obj + sel_part*100 + sel_id*10;
		sel_obj=-1;
		

		if(cur_action == V2D_ACTION_ARROW)
		{
			int bb_frame = 4;
			vec2 cc_screen = ((Get2DPoint(CT::GetCenter())*full_size-center))*scale/hscale;
			if(event.GetX()>cc_screen.x-bb_frame && event.GetX()<cc_screen.x+bb_frame)
			{
				sel_obj = V2D_SELECTION_OBJECT_CENTER;
				sel_part = V2D_SELECTION_PART_Y;
			}
			if(event.GetY()>cc_screen.y-bb_frame && event.GetY()<cc_screen.y+bb_frame)
			{
				sel_part = (sel_obj==-1)?V2D_SELECTION_PART_X : V2D_SELECTION_PART_CENTER;
				sel_obj = V2D_SELECTION_OBJECT_CENTER;
			}

			if(sel_obj==-1)
			{
			vec2 b1 = Get2DBoxMin(), b2 = Get2DBoxMax();
			vec2 b1_screen = ((b1-center))*scale/hscale, b2_screen = ((b2-center))*scale/hscale;
			vec2 b1_screen1 = vec2::Min(b1_screen,b2_screen),b2_screen1 = vec2::Max(b1_screen,b2_screen);
			if(AinsideBC(mouse_pos , b1_screen1-vec2(bb_frame), b2_screen1+vec2(bb_frame)))
			{
				sel_obj = V2D_SELECTION_OBJECT_BB;
				if(AnearB(mouse_pos,b1_screen,bb_frame))
				{
					sel_part = V2D_SELECTION_PART_POINT;
					sel_id = 0;
				}else
				if(AnearB(mouse_pos,vec2(b2_screen.x,b1_screen.y),bb_frame))
				{
					sel_part = V2D_SELECTION_PART_POINT;
					sel_id = 1;
				}else
				if(AnearB(mouse_pos,vec2(b1_screen.x,b2_screen.y),bb_frame))
				{
					sel_part = V2D_SELECTION_PART_POINT;
					sel_id = 2;
				}else
				if(AnearB(mouse_pos,b2_screen,bb_frame))
				{
					sel_part = V2D_SELECTION_PART_POINT;
					sel_id = 3;
				}else
				
				if(event.GetY()<b1_screen.y+bb_frame && event.GetY()>b1_screen.y-bb_frame)
				{
					sel_part = V2D_SELECTION_PART_EDGE;
					sel_id = 0;
				}else
				if(event.GetY()>b2_screen.y-bb_frame && event.GetY()<b2_screen.y+bb_frame)
				{
					sel_part = V2D_SELECTION_PART_EDGE;
					sel_id = 1;
				}else
				if(event.GetX()<b1_screen.x+bb_frame && event.GetX()>b1_screen.x-bb_frame)
				{
					sel_part = V2D_SELECTION_PART_EDGE;
					sel_id = 2;
				}else
				if(event.GetX()>b2_screen.x-bb_frame && event.GetX()<b2_screen.x+bb_frame)
				{
					sel_part = V2D_SELECTION_PART_EDGE;
					sel_id = 3;
				}else
					sel_part = V2D_SELECTION_PART_FACE;
						
			}
			}
		}
		if(old_s != sel_obj + sel_part*100 + sel_id*10)
			MyRefresh();
	
	
	if(event.IsButton())
	{
		
		if(event.LeftDown())
		{
			selected_obj = sel_obj;
			selected_id = sel_id;
			selected_part = sel_part;
			
		}else selected_obj=-1;
		
		MyRefresh();
	}
	
	if(event.m_wheelRotation)
	{
		scale*=(event.m_wheelRotation>0?1.1f:0.9f);
		center = (real_pos-(last_pos)*(hscale/scale))*mirror;
		
		MyRefresh();
	}
	
	last_pos=mouse_pos;
	real_pos_old = real_pos;
	UpdateCursor();
	

}
