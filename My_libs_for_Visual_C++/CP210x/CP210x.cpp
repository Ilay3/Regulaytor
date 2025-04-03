#include "stdafx.h"
#include "CP210x.h"
/*********************    GetPortNumXP2000Vista    *********************
*		Возвращает номер COM порта подключенного USB-RS232 преобразователя
*	по заданным VendorID (vid), PendorID (pid) и Serial Number (ser).
*		Если такого устройства не подключено, возвращается -1.
***********************************************************************/
int GetPortNumXP2000Vista(WORD vid, WORD pid, TCHAR* ser)
{
	//Variables used for Registry access
	HKEY tmpKey, tmpSubKey, tmpPortKey;
	TCHAR portKeyString[255];
	DWORD valtype;
	TCHAR* portString;
	DWORD length = 100;
	portString = new TCHAR[101];

	//Set portnum to -1, so if there is an error we will
	//know by returning a negative port value
	int portNum = -1;

	//  Open keys to get to the key where the port number is located. This key is:
	//  HKLM\System\CurrentControlSet\Enum\USB\Vid_xxxx&Pid_yyyy&Mi_00\zzzz_00\DeviceParameters\PortName
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, _TEXT("SYSTEM\\CurrentControlSet\\"), 0, KEY_READ, &tmpKey))
	{
		if (ERROR_SUCCESS == RegOpenKey(tmpKey, _TEXT("Enum\\USB\\"), &tmpSubKey))
		{
			//Loop through and replace spaces for WinXP2000Vista
			int i = 0;
			while (ser[i] != _TEXT('\0'))
			{
				if (ser[i] == _TEXT(' '))
					ser[i] =  _TEXT('_');
				i++;
			}

			//The portkey string should look like this
			//"Vid_XXXX&Pid_XXXX&MI_00\\XXXX_00" where the XXXX's are Vid, Pid and serial string - version less than 5.0
			//"Vid_XXXX&Pid_XXXX\\XXXX" where the XXXX's are Vid, Pid and serial string - version greater than or equal to 5.0
			_stprintf_s(portKeyString,sizeof(portKeyString)/2,_TEXT("Vid_%04x&Pid_%04x&Mi_00\\%s_00\\Device Parameters\\"), vid, pid, ser);

			//If the portkey string is in the registry, then go ahead and open the portname
			if (ERROR_SUCCESS == RegOpenKeyEx(tmpSubKey, portKeyString, 0, KEY_READ, &tmpPortKey))
			{
				if (ERROR_SUCCESS == RegQueryValueEx(tmpPortKey, _TEXT("PortName"), NULL, &valtype, (LPBYTE)portString, &length))
				{
					// When we obtain this key, it will be in string format of
					// "COMXX" where XX is the port. Simply make the first three
					// elements of the string 0, and call the atoi function to obtain
					// the number of the port.
					portString[0] = _TEXT('0');
					portString[1] = _TEXT('0');
					portString[2] = _TEXT('0');
					portNum = _ttoi(portString);
				}
				//Make sure to close all open keys for cleanup
				RegCloseKey(tmpPortKey);
			}
			else
			{
				_stprintf_s(portKeyString,sizeof(portKeyString)/2,_TEXT("Vid_%04x&Pid_%04x\\%s\\Device Parameters\\"), vid, pid, ser);

				//If the portkey string is in the registry, then go ahead and open the portname
				if (ERROR_SUCCESS == RegOpenKeyEx(tmpSubKey, portKeyString, 0, KEY_READ, &tmpPortKey))
				{
					if (ERROR_SUCCESS == RegQueryValueEx(tmpPortKey, _TEXT("PortName"), NULL, &valtype, (LPBYTE)portString, &length))
					{
						// When we obtain this key, it will be in string format of
						// "COMXX" where XX is the port. Simply make the first three
						// elements of the string 0, and call the atoi function to obtain
						// the number of the port.
						portString[0] = _TEXT('0');
						portString[1] = _TEXT('0');
						portString[2] = _TEXT('0');
						portNum = _ttoi(portString);
					}
					//Make sure to close all open keys for cleanup
					RegCloseKey(tmpPortKey);
				}
			}

			RegCloseKey(tmpSubKey);
		}
		RegCloseKey(tmpKey);
	}
	RegCloseKey(HKEY_LOCAL_MACHINE);

	delete portString;

	// Return the number of the port the device is connected too
	return portNum;
}