#include "hmd_openhmd.h"

// gets float values from the device and prints them
void print_infof(ohmd_device* hmd, const char* name, int len, ohmd_float_value val)
{
	float f[len];
	ohmd_device_getf(hmd, val, f);
	printf("%-20s", name);
	for(int i = 0; i < len; i++)
		printf("%f ", f[i]);
	printf("\n");
}

////////////////////////////////////////////

bgfx::VRImplOpenHMD::VRImplOpenHMD():device(nullptr){}

bgfx::VRImplOpenHMD::~VRImplOpenHMD(){}

bool bgfx::VRImplOpenHMD::init(){
	ctx = ohmd_ctx_create();
	auto numDevices = ohmd_ctx_probe(ctx);
	if(numDevices == 0)
		return false;

	return true;
}

void bgfx::VRImplOpenHMD::shutdown()
{
	ohmd_ctx_destroy(ctx);
}

void bgfx::VRImplOpenHMD::connect(bgfx::VRDesc *_desc){
	// Open default device (0)

	ohmd_device_settings* settings = ohmd_device_settings_create(ctx);

	// If OHMD_IDS_AUTOMATIC_UPDATE is set to 0, ohmd_ctx_update() must be called at least 10 times per second.
	// It is enabled by default.

	int auto_update = 1;
	ohmd_device_settings_seti(settings, OHMD_IDS_AUTOMATIC_UPDATE, &auto_update);

	device = ohmd_list_open_device_s(ctx, 0, settings);

	ohmd_device_settings_destroy(settings);

	// Print hardware information for the opened device
	int ivals[2];
	ohmd_device_geti(device, OHMD_SCREEN_HORIZONTAL_RESOLUTION, ivals);
	ohmd_device_geti(device, OHMD_SCREEN_VERTICAL_RESOLUTION, ivals + 1);
	printf("resolution:         %i x %i\n", ivals[0], ivals[1]);

	print_infof(device, "hsize:",            1, OHMD_SCREEN_HORIZONTAL_SIZE);
	print_infof(device, "vsize:",            1, OHMD_SCREEN_VERTICAL_SIZE);
	print_infof(device, "lens separation:",  1, OHMD_LENS_HORIZONTAL_SEPARATION);
	print_infof(device, "lens vcenter:",     1, OHMD_LENS_VERTICAL_POSITION);
	print_infof(device, "left eye fov:",     1, OHMD_LEFT_EYE_FOV);
	print_infof(device, "right eye fov:",    1, OHMD_RIGHT_EYE_FOV);
	print_infof(device, "left eye aspect:",  1, OHMD_LEFT_EYE_ASPECT_RATIO);
	print_infof(device, "right eye aspect:", 1, OHMD_RIGHT_EYE_ASPECT_RATIO);
	print_infof(device, "distortion k:", 6, OHMD_DISTORTION_K);

	float screenSizeX, screenSizeY;
	ohmd_device_getf(device, OHMD_SCREEN_HORIZONTAL_SIZE, &screenSizeX);
	ohmd_device_getf(device, OHMD_SCREEN_VERTICAL_SIZE, &screenSizeY);

	_desc->m_deviceSize.m_w = screenSizeX;
	_desc->m_deviceSize.m_h = screenSizeY;

	_desc->m_refreshRate = 90;

	// sane defaults
	_desc->m_deviceSize.m_w = 2160;
	_desc->m_deviceSize.m_h = 1200;
	//_desc->m_deviceType     = 0;
	_desc->m_refreshRate    = 90.0f;
	_desc->m_neckOffset[0]  = 0.0805f;
	_desc->m_neckOffset[1]  = 0.075f;

	for (int eye = 0; eye < 2; ++eye)
	{
		_desc->m_eyeFov[eye].m_up   = 1.32928634f;
		_desc->m_eyeFov[eye].m_down = 1.32928634f;
	}

	_desc->m_eyeFov[0].m_left  = 1.05865765f;
	_desc->m_eyeFov[0].m_right = 1.09236801f;
	_desc->m_eyeFov[1].m_left  = 1.09236801f;
	_desc->m_eyeFov[1].m_right = 1.05865765f;

	// from library
	for (int eye = 0; eye < 2; ++eye)
	{
		_desc->m_eyeSize[eye].m_w = ivals[0]/2;//screenSizeX/2.f;
		_desc->m_eyeSize[eye].m_h = ivals[1];//screenSizeY;
		if(eye == 0){
			 //ohmd_device_getf(device, OHMD_LEFT_EYE_FOV, &_desc->m_eyeFov[eye][0]);
			 //ohmd_device_getf(device, OHMD_LEFT_EYE_FOV, &_desc->m_eyeFov[eye][1]);
		}
			//memcpy(&_desc->m_eyeFov[eye], &hmdDesc.DefaultEyeFov[eye], sizeof(_desc->m_eyeFov[eye]));

	}
}

void bgfx::VRImplOpenHMD::disconnect()
{

}

bool bgfx::VRImplOpenHMD::isConnected() const
{
	return true;
}

static float off = 2.19;
static float offDir = .1;

bool bgfx::VRImplOpenHMD::updateTracking(bgfx::HMD &_hmd){
	if(device == nullptr)
		return false;

	ohmd_ctx_update(ctx);

	//float zero[] = {.0, .1, .2, 1};
	//ohmd_device_setf(device, OHMD_ROTATION_QUAT, zero);
	//ohmd_device_setf(device, OHMD_POSITION_VECTOR, zero);

	float quat[4];
	ohmd_device_getf(device, OHMD_ROTATION_QUAT, quat);

	//printf("%f - %f \n", _hmd.eye[0].viewOffset[1], _hmd.eye[1].viewOffset[1]);

//	off+=offDir;
//	if(off>50||off<-50){
//		offDir*=-1;
//	}

	//printf("%f\n", off);

	_hmd.eye[0].viewOffset[0] = off;
	_hmd.eye[1].viewOffset[0] = -off;

	// invert quat
	quat[0] = quat[0];
	quat[1] = quat[1];
	quat[2] = quat[2];

	//print_infof(device, "rotation quat:", 4, OHMD_ROTATION_QUAT);
	for (int eye = 0; eye < 2; ++eye){

		HMD::Eye& hmdEye = _hmd.eye[eye];


		hmdEye.rotation[0] = quat[0];
		hmdEye.rotation[1] = quat[1];
		hmdEye.rotation[2] = quat[2];
		hmdEye.rotation[3] = quat[3];
//		hmdEye.translation[0] = pose.Position.x;
//		hmdEye.translation[1] = pose.Position.y;
//		hmdEye.translation[2] = pose.Position.z;
//		hmdEye.viewOffset[0] = -m_viewScale.HmdToEyeOffset[eye].x;
//		hmdEye.viewOffset[1] = -m_viewScale.HmdToEyeOffset[eye].y;
//		hmdEye.viewOffset[2] = -m_viewScale.HmdToEyeOffset[eye].z;

//		for(unsigned i=0;i<16;i++){

//			hmdEye.projection[i] =
//		}

		auto eyeMat = OHMD_LEFT_EYE_GL_PROJECTION_MATRIX;
		if(eye == 1)
			eyeMat = OHMD_RIGHT_EYE_GL_PROJECTION_MATRIX;
		ohmd_device_getf(device, eyeMat, hmdEye.projection);
	}

	return true;
}

void bgfx::VRImplOpenHMD::updateInput(bgfx::HMD& /*_hmd*/)
{
}

void bgfx::VRImplOpenHMD::recenter()
{
}
