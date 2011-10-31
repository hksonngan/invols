
#include "AllDef.h"
#include "Mouse.h"
#include "Camera.h"
#include "Draw.h"
#include "Draw2D.h"
#include <Math.h>
#include <ivec3.h>
#include "output.h"
#include "CT.h"
#include "MainFrame.h"
#include "mat34.h"
#include "DicomReader.h"
#include "CPU_VD.h"

#include "Progress.h"
#include "Img3DProc.h"
#include "Region.h"
#include "seg_points.h"

#include "RenderToTexture.h"
#include "event_proc.h"
#include "bounding_cells.h"

namespace CT
{
	bool is_draging_RM=0;
	
	vec3 MoveDot(int x,int y,vec3 dot)
	{
		x = x/fast_res;
		y = y/fast_res;
//		x = (x*width)/GetFastWidth();
//		y = (y*height)/GetFastHeight();
		Ray ray = cam.GetRay(x,y);
		vec3 n = cam.GetNav();
		float mx = max(abs(n.x),max(abs(n.y),abs(n.z)));
		flat f(dot-ray.pos,vec3(mx==abs(n.x),mx==abs(n.y),mx==abs(n.z)));
		return flat_cross_line(f,ray.nav)+ray.pos;
	}

	int setsElementPos( std::vector<int>& s1,int el)
	{
		for(int i=0;i<s1.size();i++)
		{
			if(s1[i]==el)return i;
		}
		return -1;

	}
	std::vector<int> setsCross( std::vector<int> s1, std::vector<int> s2)
	{
		std::vector<int>res;
		for(int i=0;i<s1.size();i++)
		for(int j=0;j<s2.size();j++)
			if(s1[i]==s2[j])res.push_back(s1[i]);
		return res;
	}
	std::vector<int> setsAminusB( std::vector<int> s1, std::vector<int> s2)
	{
		std::vector<int>res;
		for(int i=0;i<s1.size();i++)
		{
			if(setsElementPos(s2,s1[i])==-1)
				res.push_back(s1[i]);
		}
		return res;
	}
	
	void SelectPoints(ivec2 a,ivec2 b,std::vector<vec3>&points, std::vector<int>& dst_id)
	{
		bool single_selection=0;
		float ml=10000,ll;
		int id=-1;

		if(a.x==b.x){b.x+=6;a.x-=6;}
		if(a.y==b.y){b.y+=6;a.y-=6;}
		if(abs(b.x-a.x)+abs(b.y-a.y)<40)single_selection=1;

		Flats reg(cam.GetPosition(),cam.GetRay(a.x,a.y).nav,cam.GetRay(b.x,a.y).nav,cam.GetRay(b.x,b.y).nav,cam.GetRay(a.x,b.y).nav);
		for(int i=0;i<points.size();i++)
		//if(setsElementPos(dst_id,i)==-1)
		{
			bool in_reg = reg.InRegion(points[i]);
			if(in_reg)
			{
				if(single_selection)
				{
					ll = cam.GetPosition().lengthSQR(points[i]);
					if(ll<ml)
					{
						ml=ll;
						id=i;
					}
				}else
				{
					dst_id.push_back(i);
				}
			}
		}
		if(single_selection && id!=-1)dst_id.push_back(id);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void MouseMove ( int x, int y )
	{
		mouse.Move(x,y);
		//cam.MouseMove(x,y);

		if(mouse.btn&1)
		{
			CT::need_rerender=1;
			switch(MouseButtonFunction)
			{
			case CT::MouseBC:
				{
				vec2 dd = vec2(-mouse.dy-mouse.dx,-mouse.dy+mouse.dx)*0.0001;
				iso->SetWindow(iso->GetWindow()+dd);
				CT::need_rerender_tf=1;
				CT::need_rerender2d=1;
				}
				break;
			case CT::MouseZoom:
				cam.SetDistance(cam.GetDistance()*exp((mouse.dx+mouse.dy)*0.02));
				break;
			}

		}
		if(((mouse.btn&1)&&CT::MouseRotate==MouseButtonFunction) || mouse.btn&2)
		{
			vec3 old_pos = cam.GetPosition();
			CT::need_rerender=1;
			vec3 a_dx = cam.GetTop();
			float an_x = -PI*mouse.oldy/float(CT::height)+HALF_PI;
			mat34 mrx(cos(an_x),sin(an_x),cam.GetLeft());
			a_dx = mrx.MultRot(a_dx);
			cam.Rotate(mouse.dx*0.004,a_dx);

			vec3 a_dy = cam.GetLeft();
			float an_y = -PI*mouse.oldx/float(CT::width)+HALF_PI;
			mat34 mry(cos(an_y),sin(an_y),cam.GetTop());
			a_dy = mry.MultRot(a_dy);
			cam.Rotate(mouse.dy*0.004,a_dy);
			//if(keyboard['Q'])cam.SetCenter(cam.GetCenter()+old_pos - cam.GetPosition());
		
		
		}

		if(((mouse.btn&1)&&CT::MouseTranslate==MouseButtonFunction) || mouse.btn&4)
		{
			CT::need_rerender=1;
			cam.SetCenter(cam.GetCenter() + (cam.GetLeft() *(float)(-mouse.dx) + cam.GetTop() * (float)(mouse.dy))*(2*cam.GetDistance()*cam.CameraZoom2D/height));
		}

	//	if(keyboard['1']) 		iso->SetLevel(iso->GetMinLevel()+mouse.dy*0.001f);
		
		if(is_changing_box && (MouseButtonFunction == CT::MouseArrow) && (mouse.btn&1))
		{
			vec3 b[2],bb;
			b[0] = iso->GetBoundingBox(0);
			b[1] = iso->GetBoundingBox(1);
			bb.set(b[selected_box_corner.x].x,b[selected_box_corner.y].y,b[selected_box_corner.z].z);
			//bb = MoveDot(x,y,bb);
			bb += MoveDot(x,y,bb)-MoveDot(x-mouse.dx,y-mouse.dy,bb);
			b[selected_box_corner.x].x=bb.x;
			b[selected_box_corner.y].y=bb.y;
			b[selected_box_corner.z].z=bb.z;
			iso->SetBoundingBox(b[0],b[1]);
			
			MyApp::frame->box_panel->Update_(1);
		}

		if(RM_pic_man && (MouseButtonFunction == CT::MouseArrow))
		{
			if(mouse.btn&1)
			{
				float RM_scale1 = RM_scale*height/800.0f;
				CT::need_rerender=1;
				if(is_draging_RM)
				{
					iso->GetCurRM()->pos  += vec2(mouse.dx,mouse.dy)/RM_scale1;

				}else
				{
					RM_center -= vec2(mouse.dx,mouse.dy)/RM_scale1;
				}

			}
		}
		if(!(mouse.btn&1))
		{
			is_changing_box = 0;
			is_draging_RM = 0;
		}

	
		
		if(keyboard['M']) 
		{
			vec3 ddd=(iso->volume_transform[CT::GetCurDataID()].x+iso->volume_transform[CT::GetCurDataID()].y+iso->volume_transform[CT::GetCurDataID()].z)/2.0;
			iso->volume_transform[CT::GetCurDataID()].center = MoveDot(x,y,iso->volume_transform[CT::GetCurDataID()].center);
			need_rerender=1;
		}

		if(keyboard['Z']) 
		{
			vec3 ddd=(iso->volume_transform[CT::GetCurDataID()].x+iso->volume_transform[CT::GetCurDataID()].y+iso->volume_transform[CT::GetCurDataID()].z)/2.0;
			iso->volume_transform[CT::GetCurDataID()].center+=ddd;
			float dd = 1+(mouse.dx+mouse.dy)*0.02;
			iso->volume_transform[CT::GetCurDataID()].x *= dd;
			iso->volume_transform[CT::GetCurDataID()].y *= dd;
			iso->volume_transform[CT::GetCurDataID()].z *= dd;
			ddd*=dd;
			iso->volume_transform[CT::GetCurDataID()].center-=ddd;
			need_rerender=1;

		}
		if(keyboard['R']) 
		{
			float ang_x = (-mouse.dx)*0.01f;
			mat34 mrx(cos(ang_x),sin(ang_x),cam.GetNav());

			vec3 ddd=(iso->volume_transform[CT::GetCurDataID()].x+iso->volume_transform[CT::GetCurDataID()].y+iso->volume_transform[CT::GetCurDataID()].z)/2.0;
			iso->volume_transform[CT::GetCurDataID()].center+=ddd;
			
			iso->volume_transform[CT::GetCurDataID()].x=mrx.MultRot(iso->volume_transform[CT::GetCurDataID()].x);
			iso->volume_transform[CT::GetCurDataID()].y=mrx.MultRot(iso->volume_transform[CT::GetCurDataID()].y);
			iso->volume_transform[CT::GetCurDataID()].z=mrx.MultRot(iso->volume_transform[CT::GetCurDataID()].z);
			ddd=mrx.MultRot(ddd);
			iso->volume_transform[CT::GetCurDataID()].center-=ddd;

			need_rerender=1;

		}
		
	}
			

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void MouseButton ( int button, int state )
	{
		//iso->SetStepLength(!state);
		mouse.Button(button,state);

		is_changing = (button==1 || button==2 || MouseButtonFunction!=CT::MouseArrow) && state;
		is_changing_box=0;

		if(button==0)
		{
			if(state)
			{
				old_mouse_pos = ivec2(mouse.oldx,mouse.oldy);
			}else
			{
				//seg_Cut(ivec2(min(mouse.oldx,old_mouse_pos.x),min(mouse.oldy,old_mouse_pos.y)),ivec2(max(mouse.oldx,old_mouse_pos.x),max(mouse.oldy,old_mouse_pos.y)));

			}
		}
		

		if(button==0 && MouseButtonFunction == CT::MouseArrow)
		{
			if(state)
			{
				Ray ray = cam.GetRay(mouse.oldx,mouse.oldy);
				vec3 b[2];
				b[0] = iso->GetBoundingBox(0);
				b[1] = iso->GetBoundingBox(1);
				float ml=99999;
				for(int i=0;i<2;i++)
				for(int j=0;j<2;j++)
				for(int k=0;k<2;k++)
				{
					vec3 bb(b[i].x,b[j].y,b[k].z);
					float ll = vec3::line_vec(bb-ray.pos,ray.nav);
					if(ml>ll){ml=ll;selected_box_corner.set(i,j,k);}
				}
				if(ml<0.015*0.015)
					is_changing_box=1;

				if(RM_pic_man)
				{
					for(int i=0;i<iso->rendering_methods.size();i++)
					{
						float RM_scale1 = RM_scale*height/800.0f;
						vec2 pos = (iso->rendering_methods[i]->pos-RM_center)*RM_scale1;
						vec2 size = iso->rendering_methods[i]->size*RM_scale1;
						if(pos.x<mouse.oldx && pos.x+size.x>mouse.oldx && pos.y-0.1*size.y<mouse.oldy && pos.y+size.y>mouse.oldy)
						{
							is_draging_RM = (pos.y>mouse.oldy);
							if(iso->GetCurRMID() != i)
							{
								need_rerender_tf=1;
								iso->SetCurRM(i);
								MyApp::frame->perf_panel->Update_(1);
							}
							/*
							if(keyboard['C'])
							{
								iso->SetCurRM(iso->CloneRM(i));
							}
							*/
							break;
						}


					}
				}
			} 
			
		}


	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KeyButton ( int key, int state )
	{
		keyboard[key]=state;
		
		//if(0)
		if(state)
		{
			if(key=='B'){draw_bounding_mesh=!draw_bounding_mesh;CT::need_rerender=1;}
			
			if(key=='Q')
			{
//				MakeBoundingCells(GetCurData()->st,GetCurData()->GetSize(),bounding_cells_data, bounding_cells_data_size,GL_SHORT,"ProcBC/bc_DVR.fs");

//				seg_BuildFromBC(bounding_cells_data,bounding_cells_data_size);
				need_rerender=1;
				//int sz=2048;
				//glGetIntegerv(GL_MAX_TEXTURE_SIZE,&sz);


			}
			
			if(key=='N')
			{
				screenshot_dst = "./screen_shot.png";
				MakeScreenShot(CT::width*4,CT::height*4);
			}

			if(key=='W')
			{
				//screenshot_dst = "./screen_shot.png";
				//MakeScreenShot(CT::width*4,CT::height*4);

				Progress progress(wxT("Resampling to 256^3"));
				//CPU_VD::full_data.spacing = CPU_VD::full_data.spacing*CPU_VD::full_data.GetSize().ToVec3()/vec3(256);
				//VData dt = ResampleTo256(CPU_VD::full_data);
				VData dt = GetMipData(CPU_VD::full_data,2);
				CPU_VD::full_data.Clear();
				CPU_VD::full_data = dt;

				
				CPU_VD::scale = 1.0f/max(max(CPU_VD::full_data.spacing.x*CPU_VD::full_data.GetSize().x,CPU_VD::full_data.spacing.y*CPU_VD::full_data.GetSize().y),CPU_VD::full_data.spacing.z*CPU_VD::full_data.GetSize().z);

			}
			if(key=='S')
			{
				vec3 tmpv = GetPointByDepth(mouse.oldx,mouse.oldy);
				CT::SetCenter(tmpv);
				marker = tmpv;
				need_rerender=1;
			}
			/*
			if(key=='D')
			{
				if(iso->rendering_methods.size()!=1)
				if(RM_pic_man)
					iso->DeleteRM(iso->GetCurRMID());
			}
			*/
			if(key=='R')
			{
				//if(RM_pic_man)
					iso->GetCurRM()->need_reload_shader=1;			
			}
			

			if(key=='/')
			{
			}
			if(key=='.' && fast_res>1)
			{
			}
			static int mode=0;
			if(key=='P')mode=0;
			if(key=='J')mode=1;
			if(key=='F')mode=2;
			if(key=='O')mode=3;

			if(key>='1' && key<='3') CT::SetCurDataID(key-'1');

			/*
			if(key>='1' && key<='9' && (mode==0 || mode==3))
			{
				ivec3 tf_size(GetCurData()->GetSize());
				VData tf_data(tf_size);
				switch(key)
				{
				case '1':
						ApplyFilter2(GetCurData()->st,GetCurData()->GetSize(),tf_data,GL_UNSIGNED_SHORT,"Proc/dSmoothness.fs");
					break;
					default:
						ApplyFilter2(GetCurData()->st,GetCurData()->GetSize(),tf_data,GL_UNSIGNED_SHORT,"Proc/grad_mag.fs");
				}
				
				
				if(mode==3)
				{
					GetCurData()->Upload(tf_data);
					CPU_VD::full_data.Clear();
					CPU_VD::full_data = tf_data;
					
					CPU_VD::gpu_size.set(1,1,1);
					CPU_VD::gpu_offset.set(0,0,0);
					histogram[0].Build(CPU_VD::full_data);
				}

				/*
				if(mode==0)
				{
					
					GetCurTFData()->Upload(tf_data);
					ivec3 sub_size = GetCurData()->GetSize();
					ivec3 pos = CPU_VD::GetRecomendedGPUOffset();
					VData sub_data;
					sub_data = GetSubData(CPU_VD::full_data,pos,sub_size);

					CT::histogram2D[CT::GetCurDataID()].Build(sub_data,sub_size,tf_data,tf_size);

					if(sub_size.x!=512)	delete[] sub_data;
					delete[] tf_data;
					
				}

			}
				*/
				
			/*
			if(key>='1' && key<='9' && mode==1)
			{
				switch(key)
				{
					case '1':LoadRawFile("C:\\backpack16.raw",ivec3(512,512,373),vec3(0.9766, 0.9766, 1.25));break;

						//Head Angiography Dataset provided by Volvis Project Group of Tuebingen University, Germany.
						//3T MRT Time-of-Flight Angiography dataset of a human head. The dataset has been resampled into an isotropic voxel grid (hence the peculiar slice size).
					case '2':LoadRawFile("C:\\mrt16_angio.raw",ivec3(416,512,112),vec3(0.412, 0.412, 0.412));break;
					
						//CT scan of a Colon phantom with several different objects and five pedunculated large polyps in the central object.
					case '3':LoadRawFile("C:\\colon_phantom16.raw",ivec3(512 , 512 , 442),vec3(0.9316, 0.9316, 0.5));break;
					
						//CT scan of abdomen in prone orientation, back faces ceiling, belly faces table.
					case '4':LoadRawFile("C:\\prone16.raw",ivec3( 512 , 512 , 463),vec3(0.625, 0.625, 1.0));break;

						//CT Scan of the abdomen and pelvis. The dataset contains also a stent in the abdominal aorta. No contrast agent was used to enhance the blood vessels.
					case '5':LoadRawFile("C:\\stent16.raw",ivec3(512 , 512 , 174),vec3(0.8398, 0.8398, 3.2));break;

						//Vertebra Dataset provided by Volvis Project Group of Tübingen University, Germany.
						//Rotational angiography scan of a head with an aneurysm. Only contrasted blood vessels are visible.
					case '6':LoadRawFile("C:\\vertebra16.raw",ivec3(512),vec3(0.1953, 0.1953, 0.1953));break;
					case '7':LoadRawFile("C:\\datasets\\hand16.dat.raw",ivec3(244,124,257),vec3(1));break;
					case '8':LoadRawFile("C:\\datasets\\tooth16.dat.raw",ivec3(256,256,161),vec3(1));break;


				}
			}
			*/
			if(key=='L')
			{
				
				iso->SetLightDir(cam.GetNav()*(-1));

			}
			
			if(key==']')SetFrameFast(CT::cur_frame+(keyboard['O']?10:1));
			if(key=='[')SetFrameFast(CT::cur_frame-(keyboard['O']?10:1));
			
			

		}else
		{
		}
		
	}
	void MouseWheel( int val )
	{
		mouse.Wheel(val);
		CT::need_rerender=1;
		
		if(RM_pic_man)
		{
			float RM_scale1 = RM_scale*height/800.0f;
			vec2 curs(mouse.oldx,mouse.oldy);
			vec2 ps = curs/RM_scale1+RM_center;
			RM_scale*=(val>0?1.1f:0.9f);
			RM_scale1 = RM_scale*height/800.0f;
			RM_center = ps-curs/RM_scale1;
		}else
		{
			d_zoom*=(val<0?1.05f:0.95f);
			//cam.SetDistance(cam.GetDistance()*(val<0?1.1f:0.9f));
		}
	}
}