#ifndef BGFX_OPENHMD_H_HEADER_GUARD
#define BGFX_OPENHMD_H_HEADER_GUARD

#include "bgfx_p.h"

#if BGFX_CONFIG_USE_OPEN_HMD

#include "hmd.h"
#include <openhmd.h>

namespace bgfx
{
	class VRImplOpenHMD : public VRImplI
	{
	public:

		VRImplOpenHMD();;
		virtual ~VRImplOpenHMD();;

		virtual bool init() BX_OVERRIDE;
		virtual void shutdown() BX_OVERRIDE;
		virtual void connect(VRDesc *_desc) BX_OVERRIDE;
		virtual void disconnect() BX_OVERRIDE;
		virtual bool isConnected() const BX_OVERRIDE;
		virtual bool updateTracking(HMD &_hmd) BX_OVERRIDE;
		virtual void updateInput(HMD &_hmd) BX_OVERRIDE;
		virtual void recenter() BX_OVERRIDE;
		virtual bool createSwapChain(const VRDesc &_desc, int _msaaSamples, int _mirrorWidth, int _mirrorHeight) BX_OVERRIDE = 0;
		virtual void destroySwapChain() BX_OVERRIDE = 0;
		virtual void destroyMirror() BX_OVERRIDE = 0;
		virtual void makeRenderTargetActive(const VRDesc &_desc) BX_OVERRIDE = 0;
		virtual bool submitSwapChain(const VRDesc &_desc) BX_OVERRIDE = 0;

	private:
		ohmd_context* ctx;
		ohmd_device* device;
	};
}

#endif // BGFX_CONFIG_USE_OPEN_HMD
#endif // BGFX_OPENHMD_H_HEADER_GUARD
