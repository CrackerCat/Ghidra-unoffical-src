//==============================================================================
// File:		SspiClient.h
//
// Description:	SspiClient class declaration
//
// Revisions: 	Created: 11/22/1999
//
//==============================================================================
// Copyright(C) 1999, Tomas Restrepo. All rights reserved
//==============================================================================

#if !defined(_SSPISERVER_H_)
#define _SSPISERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace wsspi
{
    class SspiClient
    {
    public:
	    SspiClient ( const SspiPackage & package, const TCHAR * target = NULL );
	    virtual ~SspiClient ( );

        virtual void AcquireCredentials ( );
        virtual void AcquireAlternateCredentials ( 
            const TCHAR * domain, const TCHAR * user, const TCHAR * password 
        );
        virtual SspiData * PrepareOutboundPackage ( SspiData * pInbound );

        // -- buffer management --
        void FreeBuffer ( SspiData* &data ) const;

        // -- accessors --
        AuthState GetAuthState ( ) { return m_State; }

        // -- private data --
    private:
        // package info
        SspiPackage             m_Package;
        // credentials
        CredHandle              m_Credentials;
        // context
        CtxtHandle              m_Context;
        // do we have a context yet?
        bool                    m_HaveContext;
        // state of the authentication
        AuthState               m_State;
        // target against which we authenticate
        const TCHAR*            m_Target;
        // it's contents are provider specific:
        // KERBEROS: The server principal name or the security context of the destination server. 
        // NTLM:     Must be NULL
        // SSL:      The server principal name for the first call to InitializeSecurityContext 
        SEC_WINNT_AUTH_IDENTITY m_Identity;
    };
}
#endif // _SSPICLIENT_H
