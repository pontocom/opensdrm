#include <windows.h>
#include <stdlib.h>

#include "config.h"
#include "utility.h"

class Win32Volume
{
	public:
        enum eDeviceType { eWaveOut = 0, eCDOut };

		         Win32Volume( eDeviceType, HWND );
		virtual ~Win32Volume(void);

		void GetVolume(int32 &left, int32 &right);
		void SetVolume(int32 left, int32 right);


	protected:

	private:
		
		bool InitVolumeControl( eDeviceType, HWND );

		UINT m_nNumMixers;
		HMIXER m_hMixer;
		MIXERCAPS m_mxcaps;

      MIXERLINE mxl;
      MIXERCONTROL mxc;
      MIXERLINECONTROLS mxlc;

		string m_oDstLineName, m_oVolumeControlName;
		DWORD m_dwMinimum, m_dwMaximum;
		DWORD m_dwVolumeControlID;
};
