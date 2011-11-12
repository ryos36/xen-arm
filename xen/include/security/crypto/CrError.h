#ifndef _CR_ERROR_H_

#define _CR_ERROR_H_



#define noError					 0



#define eCrError				-10000





#define eNoMoreMemory			eCrError





#define eECNotAssignedNumber eNoMoreMemory-1

#define eECKeyGen			eECNotAssignedNumber-1

#define eECMSA1				eECKeyGen-1

#define eECVerifyEMSA1		eECMSA1-1

#define eECDH				eECVerifyEMSA1 -1

#define eECDHNotOnTheCurve	eECDH-1

#define eECDHPointMult		eECDHNotOnTheCurve-1

#define eECDHOutConversion	eECDHPointMult-1

#define eECDHC				eECDHOutConversion -1 

#define eECSPDSA			eECDHC -1

#define eECVPDSA			eECSPDSA-1

#define eECDSASign			eECVPDSA - 1

#define eECDSAVerify		eECDSASign -1

#define eECCompressedForm	eECDSAVerify -1

#define eECPoint2OSFailure  eECCompressedForm-1

#define eECOS2PointFailure  eECPoint2OSFailure-1

#define eECWrongPC			eECOS2PointFailure -1

#define eECRandomlyGeneratedBinaryCurve eECWrongPC -1 

#define eECRandomInt		eECRandomlyGeneratedBinaryCurve-1	



#define eDTCFE2OSP				eECRandomInt-1

#define eDTCOS2FEP				eDTCFE2OSP-1

#define eDTCI2OSP				eDTCOS2FEP-1

#define eDTCFE2IP				eDTCI2OSP-1

#define eDTCDecompressY			eDTCFE2IP-1









#define CHECK_ERROR(e) if (e!= noError) return (e);

#define CHECK_LOCAL_ERROR(e) if (e!=noError) goto ERR;



#endif



