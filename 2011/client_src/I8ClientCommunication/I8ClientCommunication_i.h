

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Aug 26 11:09:52 2011
 */
/* Compiler settings for .\I8ClientCommunication.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __I8ClientCommunication_i_h__
#define __I8ClientCommunication_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStartupClient_FWD_DEFINED__
#define __IStartupClient_FWD_DEFINED__
typedef interface IStartupClient IStartupClient;
#endif 	/* __IStartupClient_FWD_DEFINED__ */


#ifndef __StartupClient_FWD_DEFINED__
#define __StartupClient_FWD_DEFINED__

#ifdef __cplusplus
typedef class StartupClient StartupClient;
#else
typedef struct StartupClient StartupClient;
#endif /* __cplusplus */

#endif 	/* __StartupClient_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IStartupClient_INTERFACE_DEFINED__
#define __IStartupClient_INTERFACE_DEFINED__

/* interface IStartupClient */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IStartupClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AFCE254-A43E-4CF5-B3BF-06C6EEB9D136")
    IStartupClient : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunGame( 
            /* [in] */ LONG gid,
            /* [in] */ VARIANT_BOOL bRunBackstage,
            /* [retval][out] */ VARIANT_BOOL *bResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunClient( 
            /* [retval][out] */ VARIANT_BOOL *bResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsGameExist( 
            /* [in] */ LONG gid,
            /* [retval][out] */ VARIANT_BOOL *bResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTimeOut( 
            /* [in] */ LONG iTimeOut,
            /* [retval][out] */ VARIANT_BOOL *bResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStartupClientVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStartupClient * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStartupClient * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStartupClient * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStartupClient * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStartupClient * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStartupClient * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStartupClient * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunGame )( 
            IStartupClient * This,
            /* [in] */ LONG gid,
            /* [in] */ VARIANT_BOOL bRunBackstage,
            /* [retval][out] */ VARIANT_BOOL *bResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunClient )( 
            IStartupClient * This,
            /* [retval][out] */ VARIANT_BOOL *bResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsGameExist )( 
            IStartupClient * This,
            /* [in] */ LONG gid,
            /* [retval][out] */ VARIANT_BOOL *bResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTimeOut )( 
            IStartupClient * This,
            /* [in] */ LONG iTimeOut,
            /* [retval][out] */ VARIANT_BOOL *bResult);
        
        END_INTERFACE
    } IStartupClientVtbl;

    interface IStartupClient
    {
        CONST_VTBL struct IStartupClientVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStartupClient_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStartupClient_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStartupClient_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStartupClient_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStartupClient_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStartupClient_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStartupClient_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IStartupClient_RunGame(This,gid,bRunBackstage,bResult)	\
    ( (This)->lpVtbl -> RunGame(This,gid,bRunBackstage,bResult) ) 

#define IStartupClient_RunClient(This,bResult)	\
    ( (This)->lpVtbl -> RunClient(This,bResult) ) 

#define IStartupClient_IsGameExist(This,gid,bResult)	\
    ( (This)->lpVtbl -> IsGameExist(This,gid,bResult) ) 

#define IStartupClient_SetTimeOut(This,iTimeOut,bResult)	\
    ( (This)->lpVtbl -> SetTimeOut(This,iTimeOut,bResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStartupClient_INTERFACE_DEFINED__ */



#ifndef __I8ClientCommunicationLib_LIBRARY_DEFINED__
#define __I8ClientCommunicationLib_LIBRARY_DEFINED__

/* library I8ClientCommunicationLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_I8ClientCommunicationLib;

EXTERN_C const CLSID CLSID_StartupClient;

#ifdef __cplusplus

class DECLSPEC_UUID("E450EB6E-4DD3-4EAF-99CB-A778B97B1C39")
StartupClient;
#endif
#endif /* __I8ClientCommunicationLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


