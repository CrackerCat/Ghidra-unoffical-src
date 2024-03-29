//==============================================================================
// File:		SspiClient.cpp
//
// Description:	SspiClient class implementation. This acts as an sspi client
//              for authentication agains an sspi server
//
// Revisions: 	11/20/1999 - created 
//              11/22/1999 - added alternate credentials capabilities
//
//==============================================================================
// Copyright(C) 1999, Tomas Restrepo. All rights reserved
//==============================================================================

#include "stdafx.h"
#include "..\inc\SspiPackage.h"
#include "..\inc\SspiDef.h"
#include "..\inc\SspiClient.h"

// SEC_WINNT_AUTH_IDENTITY makes it unusually hard
// to compile for both Unicode and ansi, so I use this macro:
#ifdef _UNICODE
#define USTR(str) (str)
#else
#define USTR(str) ((unsigned char*)(str))
#endif

using namespace wsspi;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SspiClient::SspiClient ( const SspiPackage & package, const TCHAR * target )
    : m_Package ( package ),
      m_HaveContext ( false ),
      m_State ( AuthContinue )
{
    if ( target != NULL )
        m_Target = _tcsdup ( target );
    else
       m_Target = NULL;

    m_Identity.Domain         = NULL;
    m_Identity.DomainLength   = 0;
    m_Identity.User           = NULL;
    m_Identity.UserLength     = 0;
    m_Identity.Password       = NULL;
    m_Identity.PasswordLength = 0;
}


SspiClient::~SspiClient ( )
{
    // free context
    if ( m_HaveContext )
        _pSecurityInterface->DeleteSecurityContext ( &m_Context );

    if ( m_Identity.Domain != NULL )
        free ( m_Identity.Domain );
    if ( m_Identity.User != NULL )
        free ( m_Identity.User );
    if ( m_Identity.Password != NULL )
        free ( m_Identity.Password );
}

//==========================================================
// SspiClient::FreeBuffer()
//	Description
//      releases the memory allocated for an SspiData
//      allocated by PrepareOutboundPackage()
//		
//	Parameters
//		data		 -  data buffer 
//	Return
//		void		 - none
//==========================================================

void SspiClient::FreeBuffer(SspiData* &data) const
{
	if (data != NULL) {
		delete[] (BYTE*) data;
		data = NULL;
	}
}

//==========================================================
// SspiClient::AcquireCredentials()
//	Description
//		Acquires credentials based on the current logon id
//      
//	Parameters
//	Return
//		void		 - 
//==========================================================
void SspiClient::AcquireCredentials ( )
{
    TimeStamp           Expiration;
    SECURITY_STATUS     status;

    status = _pSecurityInterface->AcquireCredentialsHandle ( 
                            NULL,
                            m_Package->Name,
                            SECPKG_CRED_OUTBOUND,
                            NULL, NULL,
                            NULL, NULL,
                            &m_Credentials,
                            &Expiration
                        );
    if ( status != SEC_E_OK )
        THROWEXE ( ErrorNoCredentials, status );

}

//==========================================================
// AcquireAlternateCredentials()
//	Description
//		Call this instead of AcquireCredentials()
//      to acquires credentials based on an alternate user
//
//	Parameters
//		domain		 - domain of the user account
//		user		 - user account name
//		password	 - password
//	Return
//		void		 - 
//==========================================================
void SspiClient::AcquireAlternateCredentials ( 
                        const TCHAR * domain, 
                        const TCHAR * user, 
                        const TCHAR * password 
                    )
{
    TimeStamp           Expiration;
    SECURITY_STATUS     status;

    // set up credentials
    // only domain == NULL makes sense.
    // user and password must _not_ be NULL
    assert ( (user != NULL) && (password != NULL) );
        
    if ( domain != NULL )
    {
        m_Identity.Domain         = USTR(_tcsdup ( domain));
        m_Identity.DomainLength   = _tcslen ( domain );
    }
    m_Identity.User           = USTR(_tcsdup ( user ));
    m_Identity.UserLength     = _tcslen ( user );
    m_Identity.Password       = USTR(_tcsdup ( password ));
    m_Identity.PasswordLength = _tcslen ( password );
#ifdef _UNICODE
    m_Identity.Flags          = SEC_WINNT_AUTH_IDENTITY_UNICODE;
#else
    m_Identity.Flags          = SEC_WINNT_AUTH_IDENTITY_ANSI;
#endif

    status = _pSecurityInterface->AcquireCredentialsHandle ( 
                            NULL,
                            m_Package->Name,
                            SECPKG_CRED_OUTBOUND,
                            NULL, 
                            &m_Identity,
                            NULL, NULL,
                            &m_Credentials,
                            &Expiration
                        );
    if ( status != SEC_E_OK )
        THROWEXE ( ErrorNoCredentials, status );

}
//==========================================================
// SspiClient::PrepareOutboundPackage()
//	Description
//      prepares an outbound package to send to the 
//      authentication server.
//		
//	Parameters
//		pInbound		 - inbound data from server
//	Return
//		SspiData*		 - outbound data to server
//==========================================================
SspiData * SspiClient::PrepareOutboundPackage ( 
                                SspiData * pInbound 
                            )
{
    SecBufferDesc   ibd, obd;
    SecBuffer       ib,  ob;
    SECURITY_STATUS status;
    SspiData*       pOutbound;        

    // prepare outbound buffer
    ob.BufferType = SECBUFFER_TOKEN;
    ob.cbBuffer   = m_Package->cbMaxToken;
    pOutbound     = reinterpret_cast<SspiData*>(new BYTE[ob.cbBuffer + sizeof(DWORD)]);
    if ( pOutbound == NULL )
        THROWEX ( ErrorNoMemory );
    ob.pvBuffer   = pOutbound->Buffer;
    // prepare buffer description
    obd.cBuffers  = 1;
    obd.ulVersion = SECBUFFER_VERSION;
    obd.pBuffers  = &ob;

    // the first time around we won't have inbound data
    if ( pInbound != NULL )
    {
        // prepare inbound buffer
        ib.BufferType = SECBUFFER_TOKEN;
        ib.cbBuffer   = pInbound->Size;
        ib.pvBuffer   = pInbound->Buffer;
        // prepare buffer description
        ibd.cBuffers  = 1;
        ibd.ulVersion = SECBUFFER_VERSION;
        ibd.pBuffers  = &ib;
    }

    // prepare our context
    DWORD      CtxtAttr;
    TimeStamp  Expiration;

    status = _pSecurityInterface->InitializeSecurityContext ( 
                            &m_Credentials,
                            m_HaveContext ? &m_Context : NULL,
                            const_cast<TCHAR*>(m_Target),
                            ISC_REQ_REPLAY_DETECT | ISC_REQ_SEQUENCE_DETECT |
                            ISC_REQ_CONFIDENTIALITY | ISC_REQ_DELEGATE, 
                            0,
                            SECURITY_NATIVE_DREP,
                            (pInbound != NULL) ? &ibd : NULL, 
                            0,
                            &m_Context,
                            &obd,
                            &CtxtAttr,
                            &Expiration 
                        );

    if ( (status == SEC_I_COMPLETE_NEEDED) ||
         (status == SEC_I_COMPLETE_AND_CONTINUE) )
    {
        if ( _pSecurityInterface->CompleteAuthToken != NULL )
            _pSecurityInterface->CompleteAuthToken ( &m_Context, &obd );
    }

    switch ( status )
    {
    case SEC_E_OK:
    case SEC_I_COMPLETE_NEEDED:
        m_State = AuthSuccess;   // we're done here
        break;
    case SEC_I_CONTINUE_NEEDED:
    case SEC_I_COMPLETE_AND_CONTINUE:
        m_State = AuthContinue;  // keep on going
        break;
    case SEC_E_LOGON_DENIED:
        m_State = AuthFailed;    // logon denied
        break;
    default:
        m_State = AuthFailed;
        // make sure we don't leak memory
        FreeBuffer ( pOutbound ); 
        THROWEXE ( ErrorAuthFailed, status );
    }

    // we should now have a context
    m_HaveContext = true;

    // adjust the size, as ISC() might have changed it:
    pOutbound->Size = ob.cbBuffer;

    return pOutbound;
}
