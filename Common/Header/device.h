
#ifndef	DEVICE_H
#define	DEVICE_H

#include <windows.h>
#include "sizes.h"
#include "MapWindow.h"

#define	NUMDEV		 2
#define	NUMREGDEV	 10

#define	devA()	&DeviceList[0]
#define	devB()	&DeviceList[1]

typedef	enum DeviceFlags_t {dfGPS, dfLogger, dfSpeed,	dfVario, dfBaroAlt,	dfWind};

typedef	struct{
	TCHAR	 *Name;
	int		 Flags;
}DeviceRegister_t;

typedef	struct DeviceDescriptor_t{
	int	Port;
	TCHAR	Name[32];
	BOOL (*ParseNMEA)(DeviceDescriptor_t *d, TCHAR *String,	NMEA_INFO	*GPS_INFO);
	BOOL (*PutMcReady)(DeviceDescriptor_t	*d,	double McReady);
	BOOL (*PutBugs)(DeviceDescriptor_t *d, double	Bugs);
	BOOL (*PutBallast)(DeviceDescriptor_t	*d,	double Ballast);
	BOOL (*Open)(DeviceDescriptor_t	*d,	int	Port);
	BOOL (*Close)(DeviceDescriptor_t *d);
	BOOL (*Init)(DeviceDescriptor_t	*d);
	BOOL (*LinkTimeout)(DeviceDescriptor_t *d);
	BOOL (*DeclBegin)(DeviceDescriptor_t *d, TCHAR *PilotsName,	TCHAR	*Class,	TCHAR	*ID);
	BOOL (*DeclEnd)(DeviceDescriptor_t *d);
	BOOL (*DeclAddWayPoint)(DeviceDescriptor_t *d, WAYPOINT	*wp);
	BOOL (*IsLogger)(DeviceDescriptor_t	*d);
	BOOL (*IsGPSSource)(DeviceDescriptor_t *d);
}DeviceDescriptor_t;

typedef	DeviceDescriptor_t *PDeviceDescriptor_t;

extern DeviceDescriptor_t	DeviceList[NUMDEV];

BOOL devRegister(TCHAR *Name,	int	Flags);
BOOL devInit(void);
PDeviceDescriptor_t devGetDeviceOnPort(int Port);

BOOL devParseNMEA(PDeviceDescriptor_t	d, TCHAR *String,	NMEA_INFO	*GPS_INFO);
BOOL devPutMcReady(PDeviceDescriptor_t d,	double McReady);
BOOL devPutBugs(PDeviceDescriptor_t	d, double	Bugs);
BOOL devPutBallast(PDeviceDescriptor_t d,	double Ballast);
BOOL devOpen(PDeviceDescriptor_t d,	int	Port);
BOOL devClose(PDeviceDescriptor_t	d);
BOOL devInit(PDeviceDescriptor_t d);
BOOL devLinkTimeout(PDeviceDescriptor_t	d);
BOOL devDeclBegin(PDeviceDescriptor_t	d, TCHAR *PilotsName,	TCHAR	*Class,	TCHAR	*ID);
BOOL devDeclEnd(PDeviceDescriptor_t	d);
BOOL devDeclAddWayPoint(PDeviceDescriptor_t	d, WAYPOINT	*wp);
BOOL devIsLogger(PDeviceDescriptor_t d);
BOOL devIsGPSSource(PDeviceDescriptor_t	d);


#endif
