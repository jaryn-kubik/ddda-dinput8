#pragma once
#include <windows.h>

class ISteamUser
{
public:
	virtual INT32 GetHSteamUser() = 0;
	virtual bool BLoggedOn() = 0;
	virtual LPVOID GetSteamID() = 0;
	virtual int InitiateGameConnection( void *pAuthBlob, int cbMaxAuthBlob, LPVOID steamIDGameServer, UINT32 unIPServer, UINT16 usPortServer, bool bSecure ) = 0;
	virtual void TerminateGameConnection( UINT32 unIPServer, UINT16 usPortServer ) = 0;
	virtual void TrackAppUsageEvent( LPVOID gameID, int eAppUsageEvent, const char *pchExtraInfo = "" ) = 0;
	virtual bool GetUserDataFolder( char *pchBuffer, int cubBuffer ) = 0;
	virtual void StartVoiceRecording( ) = 0;
	virtual void StopVoiceRecording( ) = 0;
	virtual int GetAvailableVoice( UINT32 *pcbCompressed, UINT32 *pcbUncompressed, UINT32 nUncompressedVoiceDesiredSampleRate ) = 0;
	virtual int GetVoice( bool bWantCompressed, void *pDestBuffer, UINT32 cbDestBufferSize, UINT32 *nBytesWritten, bool bWantUncompressed, void *pUncompressedDestBuffer, UINT32 cbUncompressedDestBufferSize, UINT32 *nUncompressBytesWritten, UINT32 nUncompressedVoiceDesiredSampleRate ) = 0;
	virtual int DecompressVoice( const void *pCompressed, UINT32 cbCompressed, void *pDestBuffer, UINT32 cbDestBufferSize, UINT32 *nBytesWritten, UINT32 nDesiredSampleRate ) = 0;
	virtual UINT32 GetVoiceOptimalSampleRate() = 0;
	virtual UINT32 GetAuthSessionTicket( void *pTicket, int cbMaxTicket, UINT32 *pcbTicket ) = 0;
	virtual int BeginAuthSession( const void *pAuthTicket, int cbAuthTicket, LPVOID steamID ) = 0;
	virtual void EndAuthSession( LPVOID steamID ) = 0;
	virtual void CancelAuthTicket( UINT32 hAuthTicket ) = 0;
	virtual int UserHasLicenseForApp( LPVOID steamID, UINT32 appID ) = 0;
	virtual bool BIsBehindNAT() = 0;
	virtual void AdvertiseGame( LPVOID steamIDGameServer, UINT32 unIPServer, UINT16 usPortServer ) = 0;
	virtual UINT64 RequestEncryptedAppTicket( void *pDataToInclude, int cbDataToInclude ) = 0;
	virtual bool GetEncryptedAppTicket( void *pTicket, int cbMaxTicket, UINT32 *pcbTicket ) = 0;
	virtual int GetGameBadgeLevel( int nSeries, bool bFoil ) = 0;
	virtual int GetPlayerSteamLevel() = 0;
	virtual UINT64 RequestStoreAuthURL( const char *pchRedirectURL ) = 0;
};
typedef ISteamUser *(__cdecl *tSteamUser)();