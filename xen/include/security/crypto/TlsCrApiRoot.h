#ifndef _TLS_CR_API_ROOT_H

#define _TLS_CR_API_ROOT_H



// Global Mgmt. Heder files

#include <CrConfig.h>

#include <CrBN.h>

#include <CrSMemMgr.h>

#include <TlsCrConfig.h>

#include <TlsCrKeySt.h>

#include <TlsCrSecurity.h>

#include <TlsCrSSLeayLib.h>

#include <TlsCrMemMgr.h>

#include <TlsCrError.h>







// RSA Api Mgmt. Header files

#ifdef TLS_API_RSA

#include <TlsCrRSA.h>

#include <TlsCrRSAMem.h>

#endif









// ECC Api Mgmt. Header files.

#ifdef TLS_API_ECC

#	include <CrECAppConfig.h>

#	include <CrBN.h>

#	include <CrECStruct.h>

#	include <TlsCrECParam.h>

#	include <TlsCrECDSA.h>

#	include <TlsCrECPMS.h>

#	include <TlsCrECKeyGen.h>

#	include <TlsCrECMem.h>

#endif







// DH Api Mgmt. Header files.

#ifdef TLS_API_DH

#	include <CrDH.h>

#	include <TlsCrSSLeayDH.h>

#	include <TlsCrDH.h>

#	include <TlsCrDHMem.h>

#endif







// Hash Api Mgmt. Header files

#ifdef TLS_API_HASH

#	ifdef TLS_API_SHA1

#		include <CrSHA.h>

#	endif

#	if defined(TLS_API_SHA256) || defined(TLS_API_SHA384) || defined(TLS_API_SHA512)

#		include <CrSHA2.h>

#	endif

#	ifdef TLS_API_MD2

#		include <CrMD2.h>

#	endif

#	ifdef TLS_API_MD5

#		include <CrMD5.h>

#	endif

#	include <TlsCrHash.h>

#endif





#ifdef TLS_API_PRF

#	include <TlsCrPrf.h>

#endif









// Symmetric Cipher Api Mgmt. Header files.

#ifdef TLS_API_SYMMETRIC_CIPHER

#	include <CrConfig.h>

#	include <CrEvp.h>

#	include <TlsCrBlockCipher.h>

#endif





#endif // #ifndef _TLS_CR_API_ROOT_H



