#include <stdio.h>
#include <stdlib.h>
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
#include "vec.h"
#include "Img3DProc.h"
#include "RenderToTexture.h"
#include "seg_points.h"
#include "app_state.h"


extern bool auto_gpu_upload;

namespace CT
{
	
	bool build_histogram=1,analyse_data=1;

	int gl_back_buffer_target = GL_BACK;
	wxString screenshot_dst = "./screenshot";
	wxString cur_src_dir[MAX_VD_NUMBER];
	std::vector<wxString> field_files[MAX_VD_NUMBER];
	int cur_frame=0;

	vec3 center(0.5);//центр для 2д сечений коробки
	int MouseButtonFunction;

	bool is_changing_box=0,use_bicubic_filtering=1,is_changing_tf=0;
	bool is_left=0;
	
	ivec2 old_mouse_pos;
	ivec3 selected_box_corner;

	Histogram histogram[MAX_VD_NUMBER];
	Histogram2D histogram2D[MAX_VD_NUMBER];

	float cur_level=0;

	float interaction_quality=10;
bool action_is=0;
	bool use_bounding_mesh=0,draw_bounding_mesh=0;

bool RM_pic_man=0;
vec2 RM_center(0);
float RM_scale = 300;
bool  light_to_camera=1,rendering_to_file=0;
int frames_accum_limit=30;
float d_zoom=1;
Mouse mouse;
Camera cam;
vec3 bg_color(0.5);
Render*iso;
VolumeData *volume_data[MAX_VD_NUMBER];
VolumeData *volume_tf[MAX_VD_NUMBER];
int current_volume_data=0;
ShaderProgram*view2d_sp=0,*viewMPR_sp=0;

VolumeData *GetTFData(int id)
{
	return volume_tf[id];
}
VolumeData *GetCurTFData()
{
	return volume_tf[current_volume_data];
}

VolumeData *GetData(int id)
{
	return volume_data[id];
}
VolumeData *GetCurData()
{
	return volume_data[current_volume_data];
}
int GetCurDataID()
{
	return current_volume_data;
}
int GetVolumesNumber()
{
	int res = 0;
	for(int i=0;i<MAX_VD_NUMBER;i++)
		if(CT::GetData(i)->IsLoaded())res++;
	return res;
}
void SetCurDataID(int id)
{
	current_volume_data = id;
	//need_rerender = 1;
	need_rerender_tf = 1;
	
}
int RenderingType = 1; //0-permanent  1-once changed  2-permanent accum  3-draw small when changed


int keyboard[255];
int width = 450, height = 450;
float fast_res=2;

int stereo_on=0, anag=1;
float stereo_step=0.006f;
bool need_rerender=1,need_rerender2d=1,need_rerender_tf=1,need_rerender_tf2d=1,need_reload_volume_data=1,draw_small_data=1;
bool test_accel=0,is_changing=0;
bool draw_frame_is=1,draw_gpu_box=1,draw_gpu_dummy=1;
bool draw_MRP[3]={0,0,0};

void SetLight(vec3 pos);
void InitInterlace();

void Update_rtt()
{
	rtt_Update(width,height);
}
vec3 GetCenter()
{
	return center;
}
void SetCenter(vec3 c)
{
	center = c;
}

void MakeScreenShot(int s_width,int s_height)
{
	if(CT::RenderingType==2)
	{
		{
			glReadBuffer(GL_BACK);
			int width1=width;
			int height1=height;
			int psize=4;

			unsigned char *dt0 = new unsigned char [width1*height1*psize];
			unsigned char *dt = new unsigned char [width1*height1*psize];

			glReadPixels(0, 0, width1,height1, GL_RGBA,GL_UNSIGNED_BYTE,dt0);	
			
			//for(int i=0;i<height1;i++)
			//	memcpy(dt+(height1-i-1)*width1*psize,dt0+i*width1*psize,width1*psize);
			for(int i=0;i<height1;i++)
				for(int j=0;j<width1;j++)
				for(int k=0;k<3;k++)
				{
					dt[(j+i*width1)*3+k] = dt0[(j+(height1-i-1)*width1)*4+k];
				//memcpy(dt+(height1-i-1)*width1*psize,dt0+i*width1*psize,width1*psize);
				}
			wxImage img(width1,height1,dt,true);
			wxBitmap bp(img);
			bp.SaveFile(screenshot_dst,wxBITMAP_TYPE_PNG);
			delete[]dt;
			delete[]dt0;
		}
			
		return;
	}
	int ow = CT::width, oh = CT::height;
	CT::Resize(s_width,s_height);
	
	rendering_to_file=1;
	CT::FullDraw();
	rendering_to_file=0;
	CT::Resize(ow,oh);
}
void MakeMovie()
{
	int s_width=CT::width*2,s_height=CT::height*2;
	int ow = CT::width, oh = CT::height;
	

	bool stereo=CT::keyboard['C'];
	wxString cur_dir = "./pic";
	//MyApp::frame->OnFullScreen(wxCommandEvent());

	//CT::Resize(s_width,s_height);
	rendering_to_file=1;

	Progress progress(wxT("Making pictures..."));

	char fn[]="000000.png";
	
	for(int i=0;i<CT::FramesNum();i++)
	{
		Progress::inst->SetPercent(i/float(CT::FramesNum()));
		//CT::SetFrameFast(i);
		CT::SetFrame(i);
		
		fn[2]='0'+(i/1000)%10;		fn[3]='0'+(i/100)%10;		fn[4]='0'+(i/10)%10;		fn[5]='0'+i%10;
		screenshot_dst = cur_dir+"/"+fn;
		CT::FullDraw();
		//MakeScreenShot(CT::width/2,CT::height/2);

	}

	rendering_to_file=0;
	//CT::Resize(ow,oh);

	//MyApp::frame->OnFullScreen(wxCommandEvent());
}

void LoadDicomDir(wxString src)
{
	Progress progress(wxT("Loading DICOM files"));
	
	if(!DicomReader::LoadDICOMDIR(src))
	{
		wxMessageBox("Файлы не загружены: в папке "+src+" нет нужных DICOM-файлов");
		return;
	}

	

	CPU_VD::gpu_size.set(0,0,0);
	CPU_VD::gpu_offset.set(0,0,0);
	center = CPU_VD::GetScale()*0.5f;
	need_rerender=1;
	need_rerender2d=1;
	need_rerender_tf=1;
	//MyApp::frame->Update_(1);

	histogram[GetCurDataID()].Build(CPU_VD::full_data);

	//histogram2[GetCurDataID()].Build(CPU_VD::full_data,CPU_VD::full_data.GetSize().x*CPU_VD::full_data.GetSize().y*CPU_VD::full_data.GetSize().z);

	//segmentation::Init();

}

void UpdateBinSrc()
{

	bool metrics_loaded=0;
	int f_id = CT::GetCurDataID();
	field_files[f_id].clear();
	wxString f = wxFindFirstFile(cur_src_dir[f_id]+"/*.raw");
	while ( !f.empty() )
	{
		if(!metrics_loaded)
		{
			ivec3 data_size;
			vec3 data_spacing;
			int value_format;
			if(LoadRawDataMetrics(f,data_size,data_spacing,value_format))
			{
				metrics_loaded=1;
				CPU_VD::full_data.spacing=data_spacing;
				CPU_VD::full_data.Allocate(data_size,value_format);
				CPU_VD::CalcReal();
			}
		}

		field_files[f_id].push_back(f);
		f = wxFindNextFile();
	}
	if(!field_files[f_id].size())
	{
		wxMessageBox("No files *.raw in "+cur_src_dir[f_id]+" folder");
	}
	//else		SetFrame(0);

}
int FramesNum()
{
	return field_files[CT::GetCurDataID()].size();
}

void LoadRawFile(wxString fn,ivec3 data_size, vec3 spacing,int value_format, int offset)
{
	wxFile fs;
	fs.Open(fn);
	if(!fs.IsOpened())return;
	
	CPU_VD::full_data.Allocate(data_size,value_format);
	CPU_VD::full_data.spacing = spacing;

	CPU_VD::scale = 1.0f/max(max(spacing.x*data_size.x,spacing.y*data_size.y),spacing.z*data_size.z);

	


	CPU_VD::gpu_size.set(1,1,1);
	CPU_VD::gpu_offset.set(0,0,0);

	if(offset)
	{
		char *tmptmp=new char[offset];
		fs.Read((void*)tmptmp,offset);
		delete[]tmptmp;
	}

	Progress progress(wxT("Loading RAW file..."));
	
	for(int i=0;i<data_size.z;i++)
	{
		if(data_size.z*data_size.x*data_size.y>128*128*256)Progress::inst->SetPercent((i+1)/float(data_size.z));
		
		fs.Read((void*)CPU_VD::full_data.GetSlice(i),data_size.x*data_size.y*CPU_VD::full_data.GetValueSize());
		
//		for(int j=0;j<data_size.x*data_size.y;j++)
//			*(CPU_VD::full_data.GetSlice(i)+j) = arr[j] + value_offset;
			
	}
	int i=CT::GetCurDataID();
		///Histogram&histogram = CT::histogram[CT::GetCurDataID()];
	if(build_histogram) histogram[i].Build(CPU_VD::full_data);

	if(analyse_data) data_stat.AnalyseData(CPU_VD::full_data);
	

	CPU_VD::CalcReal();

	

	fs.Close();

	
}
bool LoadRawDataMetrics(wxString fileName,ivec3& data_size,vec3& data_spacing,int& value_format)
{
	value_format=0;
	
	if(wxFile::Exists(fileName+".txt"))fileName=fileName+".txt";else
	{
		fileName[fileName.size()-3]='d';
		fileName[fileName.size()-2]='a';
		fileName[fileName.size()-1]='t';
		
	}
	if(wxFile::Exists(fileName))
	{
		wxFile fs2(fileName,wxFile::read);
		char buf[4000];
		memset(buf,0,4000);
		fs2.Read(buf,4000);
		std::string ress(buf);
		char ENTER[]={13,10,0};
		
		std::vector<std::string> dt = str::split(buf,"	 ,():;"+std::string(ENTER));
		for(int i=0;i<dt.size();i++)
		{
			if((dt[i]=="size" || dt[i]=="Resolution") && i<dt.size()-3)
			{
				data_size.set(str::ToInt(dt[i+1]),str::ToInt(dt[i+2]),str::ToInt(dt[i+3]));
				
				output::application.println("size: "+str::ToString(data_size.ToVec3()));
			}
			if((dt[i]=="spacing" || dt[i]=="SliceThickness") && i<dt.size()-3)
			{
				data_spacing.set(str::ToFloat(dt[i+1]),str::ToFloat(dt[i+2]),str::ToFloat(dt[i+3]));
				output::application.println("spacing: "+str::ToString(data_spacing));
			}
			if((dt[i]=="value_format" || dt[i]=="Format" || dt[i]=="type") && i<dt.size()-1)
			{
				//data_spacing.set(str::ToFloat(dt[i+1]),str::ToFloat(dt[i+2]),str::ToFloat(dt[i+3]));
				if(dt[i+1]=="UBYTE" || dt[i+1]=="UCHAR")value_format=1;
				output::application.println("value_format: "+dt[i+1]);
			}
		}
	
		fs2.Close();
		return true;
	}
	return false;
}
void SetFrame(int frame)
{
	int cdt = CT::GetCurDataID();
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		CPU_VD::full_data.Clear();
		CT::SetCurDataID(i);

		if(frame>=0 && frame<field_files[i].size())
		{
			cur_frame = frame;

			wxString fileName = field_files[i][frame];
			ivec3 data_size = CPU_VD::full_data.GetSize();
			vec3 data_spacing = CPU_VD::full_data.spacing;
			int value_format = CPU_VD::full_data.GetValueFormat();
			CT::LoadRawDataMetrics( fileName, data_size, data_spacing,value_format);
			CT::LoadRawFile(fileName,data_size,data_spacing,value_format);

//			LoadRawFile(field_files[i][frame], CPU_VD::full_data.GetSize(), CPU_VD::full_data.spacing,0);
			UploadVD(i);
			need_rerender=1;
		}

	}
	CT::SetCurDataID(cdt);
	
	MyApp::frame->Update_(1);

	
	
}

void SetFrameFast(int frame)
{
	build_histogram=0;
	analyse_data=0;
	SetFrame(frame);
	build_histogram=1;
	analyse_data=1;

	
}
void SetVSync(bool sync)
{ 
	typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);

	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if( wglSwapIntervalEXT )
		wglSwapIntervalEXT(sync);
}
void initIVP()
{
	SetupGL();
	cam.SetCenter(vec3(0.5,0.5,0.4));
//	cam.SetXRot(HALF_PI);
//	cam.SetYRot(PI);
	cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width,height);

	SetVSync(1);
	
	center = vec3(0.5);
	iso = new Render();
	//iso->LoadShader("RM//simple.fs");

	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		volume_data[i] = new VolumeData();
		volume_tf[i] = new VolumeData();
	}
//	LoadFolder(cur_src_dir);
/*
	volume_data->Upload(arr,size,2);
	

	delete[] arr;

	//CT::iso->SetScale(vec3(1,1,0.7));
//	CT::iso->SetScale(vec3(1,1,1));
*/	
	
	cam.SetDistance(1.5f);

	stereo_on=0;
	InitInterlace();
	if(!view2d_sp)view2d_sp = new ShaderProgram("common_shaders//srf.vs", "common_shaders//view2d.fs");
	if(!viewMPR_sp)viewMPR_sp = new ShaderProgram("common_shaders//srf.vs", "common_shaders//viewMPR.fs");


	rtt_Init();
	seg_BuildBox(vec3(0),vec3(1));

	
}

void KillCT()
{
	if(screen_depth)delete[]screen_depth;

	rtt_Cleanup();
	CPU_VD::UnInit();
//	segmentation::UnInit();
	delete iso;
	for(int i=0;i<MAX_VD_NUMBER;i++)
		delete volume_data[i];
	for(int i=0;i<MAX_VD_NUMBER;i++)
		delete volume_tf[i];
	delete view2d_sp;
	delete viewMPR_sp;
}



void SaveProfile(wxFile& fs)
{
	iso->Save(fs);
	SaveItem(fs,cam);
	seg_Save(fs);
	SaveItem(fs,cur_loaded_file_type);
	SaveString(fs,cur_loaded_file);

	SaveItem(fs,CT::bg_color);
	SaveItem(fs,CT::draw_frame_is);
	SaveItem(fs,CT::RM_center);
	SaveItem(fs,CT::RM_scale);
	SaveItem(fs,CT::RM_pic_man);
	SaveItem(fs,CT::stereo_step);
	SaveItem(fs,CT::interaction_quality);
	SaveItem(fs,CT::fast_res);
}

void LoadProfile(wxFile& fs)
{
	iso->Load(fs);
	OpenItem(fs,cam);
	seg_Load(fs);
	OpenItem(fs,cur_loaded_file_type);
	OpenString(fs,cur_loaded_file);

	OpenItem(fs,CT::bg_color);
	OpenItem(fs,CT::draw_frame_is);
	OpenItem(fs,CT::RM_center);
	OpenItem(fs,CT::RM_scale);
	OpenItem(fs,CT::RM_pic_man);
	OpenItem(fs,CT::stereo_step);
	OpenItem(fs,CT::interaction_quality);
	OpenItem(fs,CT::fast_res);

	if(0)
	if(cur_loaded_file_type)
	{
		if(cur_loaded_file_type==1)
		{
			LoadDicomDir(cur_loaded_file);
			if(auto_gpu_upload)	MyApp::frame->OnLoadToGPU(wxCommandEvent());
		}
	//	if(cur_loaded_file_type==2)
	//		MyApp::frame->rf->OnBtnOK(wxCommandEvent());
	}
	//CT::RM_pic_man=1;
		//seg_BuildBox(vec3(0.1),vec3(0.9),ivec3(4));

}
void SetProjection(bool mode)
{
	cam.SetProjection(mode);
	iso->ReLoadCurShader();
	//cam.SetCenter(cam.GetCenter());
	CT::need_rerender=1;

	/*
	if(mode)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
		*/
	

}
ivec3 GetBoundingBox(bool is_max)
{
	return ivec3::Clamp(ivec3(0),CPU_VD::full_data.GetSize()-ivec3(1),(CT::iso->GetBoundingBox(is_max)*CPU_VD::full_data.GetSize().ToVec3()).ToIVec3());
}


void Demo()
{
}
void DoRegular()
{
	
	if(d_zoom!=1)
	{
		cam.SetDistance(cam.GetDistance()*d_zoom);
		need_rerender=1;
		d_zoom = (d_zoom-1)*0.83 +1;
		if(abs(d_zoom-1)<0.01)d_zoom=1.0;
	}

	if(action_is)Demo();

/*
	float cam_speed=0.02f;
	if(keyboard['W']) 
	{
		cam.SetCenter(cam.GetCenter()+cam.GetNav()*cam_speed);
		need_rerender=1;
	}
	if(keyboard['S']) 
	{
		cam.SetCenter(cam.GetCenter()-cam.GetNav()*cam_speed);
		need_rerender=1;
	}
	if(keyboard['A']) 
	{
		cam.SetCenter(cam.GetCenter()-cam.GetLeft()*cam_speed);
		need_rerender=1;
	}
	if(keyboard['D']) 
	{
		cam.SetCenter(cam.GetCenter()+cam.GetLeft()*cam_speed);
		need_rerender=1;
	}
*/
}


}

void SaveString(wxFile& fs,wxString& str)
{
	int num = str.size();
	
	fs.Write( (char*)&num,sizeof(int));
	for(int i=0;i<num;i++)
	{
		SaveItem(fs,str[i]);
	}
}
void OpenString(wxFile& fs,wxString& str)
{
	int num=0;
	
	fs.Read( (char*)&num,sizeof(int));
	str.resize(num);
	if(!num)return;
	for(int i=0;i<num;i++)
	{
		char tmp;
		OpenItem(fs,tmp);
		str[i]=tmp;
	}
}