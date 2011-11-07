#ifndef CT_INC
#define CT_INC

#include "AllInc.h"
#include "Render.h"
#include "wxIncludes.h"
#include "Img3DProc.h"


struct Mouse;
class Camera;


template<class T>
void SaveItem(wxFile& fs,T& item)
{
	fs.Write( (char*)&item, sizeof(T));
}
template<class T>
void OpenItem(wxFile& fs,T& item)
{
	fs.Read( (char*)&item, sizeof(T));
}
template<class T>
void SaveVector(wxFile& fs,std::vector<T>&v)
{
	int num = v.size();
	//fs << num;
	fs.Write( (char*)&num,sizeof(int));
	for(int i=0;i<num;i++)
	{
		SaveItem(fs,v[i]);
	}
}
template<class T>
void OpenVector(wxFile& fs,std::vector<T>&v)
{
	int num=0;
	//fs >> num;
	fs.Read( (char*)&num,sizeof(int));
	v.clear();
	if(!num)return;
	for(int i=0;i<num;i++)
	{
		T tmp;
		OpenItem(fs,tmp);
		v.push_back(tmp);
	}
}

void SaveString(wxFile& fs,wxString& str);
void OpenString(wxFile& fs,wxString& str);

namespace CT
{
	void MakeScreenShot(int s_width,int s_height);
	void MakeMovie();
	extern wxString screenshot_dst;

	// для бинарных файлов
	int FramesNum();
	void SetFrame(int frame);
	void SetFrameFast(int frame);
	void UpdateBinSrc();

	ivec3 GetBoundingBox(bool is_max);
	void LoadDicomDir(wxString src);
	void MouseMove ( int x, int y );
	void MouseButton ( int button, int state );
	void KeyButton ( int key, int state );

	void MouseWheel( int val );
	void initIVP();
	void FullDraw();
	void Resize(int _width,int _height);
	void KillCT();
	void SetStereoMode(int stereo_mode);

	void DoRegular();
	void SaveProfile(wxFile& fs);
	void LoadProfile(wxFile& fs);
	float glfwGetTime();
	bool IsFastView();
	void SetLight(vec3 pos);
	void RunFullScreen ();

	void SetupGL();
	void SetProjection(bool mode);

	void LoadRawFile(wxString fn, ivec3 data_size, vec3 spacing,int value_format,int offset=0);
	bool LoadRawDataMetrics(wxString fileName,ivec3& data_size,vec3& data_spacing,int& value_format);
	void UploadVD(int id);
	void Update_rtt();
	vec3 GetCenter();
	void SetCenter(vec3 c);
	int GetFastWidth();
	int GetFastHeight();


	vec3 GetPointByDepth(int x,int y);
	float GetScreenDepth(int x,int y);
	void UpdateScreenDepth();

	VolumeData *GetData(int id);
	VolumeData *GetCurData();
	VolumeData *GetTFData(int id);
	VolumeData *GetCurTFData();

	int GetVolumesNumber();
	int GetCurDataID();

	void SetCurDataID(int id);





	extern float *screen_depth, fast_res;

	extern Mouse mouse;
	extern Camera cam;
	extern bool action_is,draw_frame_is,is_changing, light_to_camera;
	extern bool draw_gpu_box,draw_gpu_dummy;
	extern bool draw_MRP[3];

	extern float stereo_step, fps, cur_level,interaction_quality;
	
	extern int stereo_on, anag, gl_back_buffer_target;
	extern bool is_left;

	extern bool is_changing_box,is_changing_tf,draw_bounding_mesh;
	extern ivec3 selected_box_corner;

	extern bool RM_pic_man;
	extern vec2 RM_center;
	extern float RM_scale;
	extern ivec2 old_mouse_pos;
	extern vec3 marker, bg_color;
	extern Render*iso;


	extern wxString cur_src_dir[MAX_VD_NUMBER];

	extern ShaderProgram*view2d_sp,*viewMPR_sp;
	extern int frames_accum_limit;
	extern float d_zoom;
	extern bool RM_pic_man;
	extern int keyboard[255];
	extern int width, height; 

	extern int RenderingType;
	extern bool need_rerender,need_rerender2d,need_rerender_tf,draw_small_data,need_reload_volume_data;

	extern bool use_bicubic_filtering;

	extern int cur_frame;

	extern Histogram histogram[MAX_VD_NUMBER];
	extern Histogram2D histogram2D[MAX_VD_NUMBER];

	enum
	{
		MouseRotate=0,
		MouseTranslate,
		MouseZoom,
		MouseArrow,
		MouseMeasureLength,
		MouseBC
	};
	extern int MouseButtonFunction;


}

#endif