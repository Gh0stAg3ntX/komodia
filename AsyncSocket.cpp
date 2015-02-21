/*  
 *  
 *  
 *  Copyright (c) 2000 Barak Weichselbaum <barak@komodia.com>  
 *  All rights reserved.  
 *  
 * Redistribution and use in source and binary forms, with or without  
 * modification, are permitted provided that the following conditions  
 * are met:  
 * 1. Redistributions of source code must retain the above copyright  
 *    notice, this list of conditions and the following disclaimer.  
 * 2. Redistributions in binary form must reproduce the above copyright  
 *    notice, this list of conditions and the following disclaimer in the  
 *    documentation and/or other materials provided with the distribution.  
 *  
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND  
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE  
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL  
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS  
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT  
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY  
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF  
 * SUCH DAMAGE.  
 *  
 *  
 * Contact info:  
 * Site: http://www.komodia.com  
 * Email: barak@komodia.com  
 */   
   
#include "stdafx.h"   
#include "AsyncSocket.h"   
#include "SocketThreadManager.h"   
   
#ifdef _MEMORY_DEBUG    
    #define new    DEBUG_NEW     
    #define malloc DEBUG_MALLOC     
    static char THIS_FILE[] = __FILE__;     
#endif   
   
    KOMODIA_NAMESPACE_START   
   
//Static members   
BOOL CAsyncSocket::m_Window=FALSE;   
HWND CAsyncSocket::m_WindowHandle=0;   
HINSTANCE CAsyncSocket::m_Instance=0;   
BOOL CAsyncSocket::m_Initialized=0;   
CAsyncSocket::SocketMap CAsyncSocket::m_SocketMap;   
CSocketThreadManager* CAsyncSocket::m_pThreadManager=NULL;   
BOOL CAsyncSocket::m_bShuttingDown=FALSE;   
   
CAsyncSocket::CAsyncSocket() : CSpoofBase(),   
                               m_List(FALSE),   
                               m_Timeout(FALSE),   
                               m_pLocalThreadManager(NULL),   
                               m_hLocalWindowHandle(0),   
                               m_bFreeze(FALSE),   
                               m_bBlocking(FALSE),   
                               m_iMsg(0),   
                               m_lEvent(0)   
{   
    try   
    {   
        //Initialize all data   
        Initialize();   
    }   
    ERROR_HANDLER("CAsyncSocket")   
}   
   
CAsyncSocket::~CAsyncSocket()   
{   
    try   
    {   
        if (GetThreadManager())   
            //Remove from the thread manager   
            GetThreadManager()->DecreaseSocketCount(GetWindowHandle(),m_bFreeze);   
   
    }   
    ERROR_HANDLER("~CAsyncSocket")   
}   
   
BOOL CAsyncSocket::SetHandlers()   
{   
    try   
    {   
        //First create the window class   
        if (!m_Window)   
            if (!RegisterWindow())   
            {   
                //Error   
                ReportStaticError(CAsyncSocket_Class,"SetHandlers","Error registering the window, please check API error!");   
                return FALSE;   
            }   
            else   
                //Check if we need to register a local window, or a thread manager ?   
                if (CSpoofBase::IsMultiThreaded())   
                    //Initialize as multithreaded   
                    m_pThreadManager=new CSocketThreadManager(CSpoofBase::GetNumberOfThreads(),m_Instance,CAsyncSocket_Class);   
                else   
                {   
                    //Run on main thread   
                    m_WindowHandle=CreateWindowEx(0,CAsyncSocket_Class,SOCKET_WINDOW_NAME,   
                                                  WS_OVERLAPPED,0,0,0,0,0,NULL,GetInstance(),NULL);   
                    //Check the value of the window   
                    if (!m_WindowHandle)   
                    {   
                        //Error   
                        ReportStaticError(CAsyncSocket_Class,"SetHandlers","Error creating the window, please check API error!");   
                        return FALSE;   
                    }   
                    else   
                        //We have a window   
                        m_Window=TRUE;   
                }   
   
        //Created !!   
        //Success   
        return TRUE;   
    }   
    ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"CAsyncSocket",FALSE)   
}   
   
HINSTANCE CAsyncSocket::GetInstance()   
{   
    //Returns the instance of the application, must be overided   
    return m_Instance;   
}   
   
void CAsyncSocket::AddSocketToList()   
{   
    try   
    {   
        //Allocate our window   
        AllocateHandle();   
   
        //Add socket to list   
        m_SocketID=GetAsyncHandle();   
        m_SocketMap.insert(SocketMap::value_type(m_SocketID,this));   
   
        m_List=TRUE;   
    }   
    ERROR_HANDLER("AddSocketToList")   
}   
   
int CAsyncSocket::GetSocketID() const   
{   
    return m_SocketID;   
}   
   
CAsyncSocket* CAsyncSocket::GetSocketByID(int iSockID)   
{   
    try   
    {   
        //Find the socket   
        SocketMap::iterator aTheIterator;   
        aTheIterator=m_SocketMap.find(iSockID);   
   
        //Check if we have it   
        if (aTheIterator!=m_SocketMap.end())   
            return aTheIterator->second;   
        else   
            return NULL;   
    }   
    ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"GetSocketByID",NULL)   
}   
   
BOOL CAsyncSocket::RegisterWindow()   
{   
    try   
    {   
        WNDCLASS wc;   
   
        /* Fill in window class structure with parameters that describe the       */   
        /* main window.                                                           */   
   
        wc.style = 0;                                         /* Class style(s).                    */   
        wc.lpfnWndProc = (WNDPROC)SocketMessageHandler;       /* Function to retrieve messages for  */   
                                            /* windows of this class.             */   
        wc.cbClsExtra = 0;                  /* No per-class extra data.           */   
        wc.cbWndExtra = 0;                  /* No per-window extra data.          */   
        wc.hIcon = NULL;                    /* Icon name from .RC        */   
        wc.hInstance = GetInstance();          /* Application that owns the class.   */   
        wc.hCursor = NULL;   
        wc.hbrBackground = NULL;   
        wc.lpszMenuName =  NULL;   /* Name of menu resource in .RC file. */   
        wc.lpszClassName = CAsyncSocket_Class ; /* Name used in call to CreateWindow. */   
   
        /* Register the window class and return success/failure code. */   
   
        return (RegisterClass(&wc));   
    }   
    ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"RegisterWindow",FALSE)   
}   
   
void CAsyncSocket::SetInstance(HINSTANCE hInst)   
{   
    m_Instance=hInst;   
}   
   
BOOL CAsyncSocket::RemoveHandlers()   
{   
    try   
    {   
        //First shut down the windows   
        if (m_Window)   
        {   
            if (!DestroyWindow(m_WindowHandle))   
                return FALSE;   
   
            if (!UnregisterClass(CAsyncSocket_Class,GetInstance()))   
                return FALSE;   
        }   
   
        m_Window=FALSE;   
        m_WindowHandle=NULL;   
   
        return TRUE;   
    }   
    ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"RemoveHandlers",FALSE)   
}   
   
HWND CAsyncSocket::GetWindowHandle() const   
{   
    //Check if we are multithreaded ?   
    return m_hLocalWindowHandle;   
}   
   
void CAsyncSocket::RemoveSocketFromList()   
{   
    try   
    {   
        if (m_List)   
            m_SocketMap.erase(GetSocketID());   
    }   
    ERROR_HANDLER("RemoveSocketFromList")   
}   
   
BOOL CAsyncSocket::SetTimeout(int iMs)   
{   
    try   
    {   
        HWND hWindowHandle;   
        hWindowHandle=GetWindowHandle();   
   
        if (!hWindowHandle || m_Timeout)   
            return FALSE;   
   
        //Set the timer   
        m_Timeout=SetTimer(hWindowHandle,GetAsyncHandle(),iMs,NULL);   
   
        return m_Timeout;   
    }   
    ERROR_HANDLER_RETURN("SetTimeout",FALSE)   
}   
   
BOOL CAsyncSocket::KillTimer()   
{   
    try   
    {   
        HWND hWindowHandle;   
        hWindowHandle=GetWindowHandle();   
   
        if (!hWindowHandle || !m_Timeout)   
            return FALSE;   
   
        //No timer in any case   
        m_Timeout=FALSE;   
   
        BOOL bResult;   
        bResult=::KillTimer(hWindowHandle,GetAsyncHandle());   
   
        if (!bResult)   
            //Fire an error   
            ReportError("KillTimer");   
   
        return bResult;   
    }   
    ERROR_HANDLER_RETURN("KillTimer",FALSE)   
}   
   
void CAsyncSocket::Shutdown()   
{   
    try   
    {   
        //Indicate we're shutting down   
        m_bShuttingDown=TRUE;   
   
        //Clear the map   
        SocketMap::iterator aTheIterator;   
        aTheIterator=m_SocketMap.begin();   
   
        //While not end of the map   
        while (aTheIterator!=m_SocketMap.end())   
        {   
            //Delete the socket   
            delete aTheIterator->second;   
            aTheIterator->second=NULL;   
   
            ++aTheIterator;   
        }   
   
        //Wait for clean up   
        Sleep(1000);   
   
        //Delete the thread manager   
        if (m_pThreadManager)   
        {   
            delete m_pThreadManager;   
            m_pThreadManager=NULL;   
        }   
   
        //Remove the handlers   
        RemoveHandlers();   
    }   
    ERROR_HANDLER_STATIC(CAsyncSocket_Class,"Shutdown")   
}   
   
CAsyncSocket::CAsyncShutdown::CAsyncShutdown() : CSpoofBase()   
{   
    try   
    {   
        //Register myself   
        SetName(CAsyncShutdown_Class);   
   
        //Register for shutdown   
        RegisterShutdown(this);   
    }   
    ERROR_HANDLER("CAsyncShutdown")   
}   
   
CAsyncSocket::CAsyncShutdown::~CAsyncShutdown()   
{   
}   
   
void CAsyncSocket::CAsyncShutdown::NotifyShutdown()   
{   
    try   
    {   
        //Socket shutdown!   
        CAsyncSocket::Shutdown();   
    }   
    ERROR_HANDLER("NotifyShutdown")   
}   
   
BOOL CAsyncSocket::IsTimeout() const   
{   
    return m_Timeout;   
}   
   
void CAsyncSocket::Initialize()   
{   
    try   
    {   
        //Initialize all data   
        if (!m_Initialized && CSpoofBase::IsInitialized())   
        {   
            //Create handlers   
            if (!SetHandlers())   
                ReportStaticError(CAsyncSocket_Class,"CAsyncSocket","Failed to init handlers!");   
   
            //Create a new socket to do the shutdown   
            CAsyncShutdown* pShutdown;   
            pShutdown=new CAsyncShutdown;   
   
            //The class registers itself   
            m_Initialized=TRUE;   
        }   
    }   
    ERROR_HANDLER_STATIC(CAsyncSocket_Class,"Initialize")   
}   
   
//Message handler   
LRESULT CALLBACK CAsyncSocket::SocketMessageHandler(HWND hwnd,      // handle to window   
                                                    UINT uMsg,      // message identifier   
                                                    WPARAM wParam,  // first message parameter   
                                                    LPARAM lParam)   // second message parameter                                                       
{   
    if (m_bShuttingDown)   
        return TRUE;   
   
    try   
    {   
        //first get the socket   
        CAsyncSocket* cSock;   
   
        cSock=GetSocketByID((int)wParam);   
   
        if (cSock)   
            //Socket exists   
            switch (uMsg)   
            {   
            case WM_SOCKET_GENERAL:   
                if (WSAGETSELECTEVENT(lParam) == FD_READ)   
                    return cSock->OnSocketReceive(WSAGETSELECTERROR(lParam));   
                else if (WSAGETSELECTEVENT(lParam) == FD_WRITE)   
                    return cSock->OnSocketWrite(WSAGETSELECTERROR(lParam));   
                else if (WSAGETSELECTEVENT(lParam) == FD_OOB)   
                    return cSock->OnSocketOOB(WSAGETSELECTERROR(lParam));   
                else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)   
                    return cSock->OnSocketClose(WSAGETSELECTERROR(lParam));   
                break;   
            case WM_SOCKET_CONNECT:   
                if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)   
                    return cSock->OnSocketConnect(WSAGETSELECTERROR(lParam));   
                break;   
            case WM_SOCKET_ACCEPT:   
                if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)   
                    return cSock->OnSocketAccept(WSAGETSELECTERROR(lParam));   
                break;   
            case WM_TIMER:   
                //Inform the socket   
                return cSock->OnSocketTimeout();   
            default:                       /* Passes it on if unproccessed    */   
                return (int)(DefWindowProc(hwnd, uMsg, wParam, lParam));   
            }   
        else   
            return (int)(DefWindowProc(hwnd, uMsg, wParam, lParam));   
   
        return TRUE;   
    }   
    ERROR_HANDLER_STATIC_RETURN(CAsyncSocket_Class,"SocketMessageHandler",TRUE)   
}   
   
CSocketThreadManager* CAsyncSocket::GetThreadManager() const   
{   
    if (!m_pLocalThreadManager)   
        return m_pThreadManager;   
    else   
        return m_pLocalThreadManager;   
}   
   
void CAsyncSocket::AllocateHandle()   
{   
    try   
    {   
        if (GetThreadManager())   
            //We are   
            m_hLocalWindowHandle=GetThreadManager()->GetWindowHandle();   
        else   
            //Single threaded   
            m_hLocalWindowHandle=m_WindowHandle;       
    }   
    ERROR_HANDLER("AllocateHandle")   
}   
   
void CAsyncSocket::DeAllocateHandle()   
{   
   
}   
   
BOOL CAsyncSocket::DisableAsync()   
{   
    try   
    {   
        //Quit if not ok   
        if (!CheckAsyncSocketValid())   
            return FALSE;   
   
        //Set event to read / write / close / oob   
        int iResult;   
   
        iResult=WSAAsyncSelect(GetAsyncHandle(),GetWindowHandle(),0,0);   
        if (iResult)   
        {   
            SetLastError("DisableAsync");   
            return FALSE;   
        }   
           
        return TRUE;   
    }   
    ERROR_HANDLER_RETURN("DisableAsync",FALSE)   
}   
   
BOOL CAsyncSocket::CheckAsyncSocketValid() const   
{   
    try   
    {   
        //Check if socket is invalid   
        if (GetAsyncHandle()==INVALID_SOCKET)   
        {   
            ReportError("CheckAsyncSocketValid","Operation made on non existant socket!");   
            return FALSE;   
        }   
   
        //OK   
        return TRUE;   
    }   
    ERROR_HANDLER_RETURN("CheckAsyncSocketValid",FALSE)   
}   
   
void CAsyncSocket::SocketClosing()   
{   
    try   
    {   
        if (m_Timeout)   
            KillTimer();   
    }   
    ERROR_HANDLER("SocketClosing")   
}   
   
void CAsyncSocket::FreezeThread()   
{   
    m_bFreeze=TRUE;   
}   
   
BOOL CAsyncSocket::ReBlock()   
{   
    if (m_bBlocking)   
        return TRUE;   
   
    try   
    {   
        //Quit if not ok   
        if (!CheckAsyncSocketValid())   
            return FALSE;   
   
        if (Block())   
           
        //Set to reblock   
        m_bBlocking=TRUE;   
   
        //And quit   
        return TRUE;   
    }   
    ERROR_HANDLER_RETURN("ReBlock",FALSE)   
}   
   
int CAsyncSocket::InternalWSAAsyncSelect(unsigned int wMsg, long lEvent)   
{   
    try   
    {   
        //Cache the values   
        m_iMsg=wMsg;   
        m_lEvent=lEvent;   
   
        if (m_bBlocking)   
            return 0;   
        else   
            //And call the async select   
            return WSAAsyncSelect(GetAsyncHandle(),GetWindowHandle(),wMsg,lEvent);   
    }   
    ERROR_HANDLER_RETURN("InternalWSAAsyncSelect",SOCKET_ERROR)   
}   
   
BOOL CAsyncSocket::ReAsync()   
{   
    if (!m_bBlocking)   
        return TRUE;   
   
    try   
    {   
        //Quit if not ok   
        if (!CheckAsyncSocketValid())   
            return FALSE;   
   
        //First disable the events   
        int iResult;   
        iResult=WSAAsyncSelect(GetAsyncHandle(),GetWindowHandle(),m_iMsg,m_lEvent);   
   
        if (iResult)   
        {   
            SetLastError("ReAsync");   
            return FALSE;   
        }   
   
        //Set to async   
        m_bBlocking=FALSE;   
   
        //And quit   
        return TRUE;   
    }   
    ERROR_HANDLER_RETURN("ReAsync",FALSE)   
}   
   
BOOL CAsyncSocket::Block()   
{   
    try   
    {   
        //First disable the events   
        int iResult;   
        iResult=WSAAsyncSelect(GetAsyncHandle(),GetWindowHandle(),0,0);   
   
        if (iResult)   
        {   
            SetLastError("Block");   
            return FALSE;   
        }   
   
        unsigned long ulBlocking;   
        ulBlocking=0;   
   
        //And return to non-blocking   
        iResult=ioctlsocket(GetAsyncHandle(),FIONBIO,&ulBlocking);   
   
        if (iResult)   
        {   
            SetLastError("Block");   
            return FALSE;   
        }   
   
        return TRUE;   
    }   
    ERROR_HANDLER_RETURN("Block",FALSE)   
}   
   
BOOL CAsyncSocket::IsBlocking() const   
{   
    return m_bBlocking;   
}   
   
KOMODIA_NAMESPACE_END  
