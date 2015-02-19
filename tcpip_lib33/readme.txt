Index (v3.3)
----------

1. What is this project ?
2. Who are we ?
3. Requirments !
4. Architecture.
5. Library description.
6. Samples.
7. What's next ?
8. Contact us.

1. What is this project ?
-------------------------

When I first started to learn about the world of security (and hacking)
I had some knowledge of basic exploits and networking.

I knew that if I want to find exploits my self I will have to deal with the nuts and bolts of the networking protocols.

And so on, I began investigating the matter until I found sockets 2.
Altough you must have nt2k to run this. (soon everyone will have)

After a 2 month of writing this library. (you won't believe how many little things like wrong bits, slowed my pace)

I'm releasing it so other programmers/hackers can learn/use this library.

I consider this library to be a milestone in the learning of socket2 on W2K platform.
(I hope you consider this as well when you use it)

The library is very developed since it's first apperance, and your feedback is very important
to me.

Respectfully,
		Barak Weichselbaum

2. Who are we ?
---------------

We are a new group at the scene of hacking/security, you can visit us at : http://www.komodia.com

3. Requirments !
----------------

OS : win2k/XP (All of the regular networking works on all the platform (you need to install winsock2 update on 95), but the spoofing features onlt as stated before)

To compile : you will need the recent DDK or SDK for win2k/XP. (Recomended)

C++ : Please change the setting to multithreaded (go to project->settings, choose c++, and
on the category tab select "code generation", change the runtime library to "debug multithreaded" 
(or just multithreaded if it's the release version) (On MFC it's mulithreaded by default)

Link: add ws2_32.lib to the input list (link settings)
Console mode: add the following lines (seen in the console samples)

#include <winsock2.h>
#include <windows.h>

On a newer version of include files (like Platform SDK, or DDK), you need to put the following lines, in the stdafx.h:

#include <winsock2.h>
#include <ws2tcpip.h>

When you want to add the library to your code there are two options:

1. Add the relevant files into your project.
2. Link with my two libraries (That already contain the files),
   If you decide to link with them, make sure that if you are using MFC, choose the MFC 
   configuration (debug), or MFC release (release), since the multithreaded setting for MFC are 
   different from the one I'm using)

4. Architecture.
-----------------------
This library uses sockets 2.

It has two main features:

1. It opens up a raw socket, and sets it in a way it alows to send raw IP headers. 
   (so you can do IP spoofing, and play with the bits and bolts of the protocol)
2. You can use it as a regular sockets, why - when I programmed my ATL server, I tried to open a 
   socket, but since the MFC frameworks takes the instance with AfxGetInstance, the ATL server 
   crashed, my application takes the application instance dynamicly.

We change alot since the last version, we redesigned the OOD in order to acheive greater control
and scalability.

Instead of CSpoofSocket (as base class), we created CSpoofBase, moved some non direct socket functionality
to that class, and added error handlers to the entire library.

We implemented some of the classes using multiple inheritance, which has its drawbacks. (but we decided the pro's overweight the con's)

Other classes in the library: (below CSpoofBase)

1. Class CSpoofSocket, the main IP carrier protocol.
2, Classes (Beneeth CSpoofSocket) CTCPSocket,CUDPSocket and CICMPSocket, Encapsulate the protocol they are named after.
3. Class CAsyncSocket, is the abstraction of asyn socket operations.
4. Class (Beneeth CAsyncSocket) CSniffSocket, Implementation of our sniffer socket.
5. Class CInterfaces, to retrieve a list of the available interfaces. 
6. Beneeth CAsyncSocket and CTCPSocket,CUDPSocket and CICMPSocket, 
   Comes : CTCPSocketAsync,CUDPSocketAsync and CICMPSocketAsync.
   These sockets are async sockets (no more SetBlock method)
7. CSocketThreadManager, manages the thread messaging.
8. CThreadPool, a worker thread manager.
9. CIPCrafter, CTCPCrafter, CUDPCrafter - for crafting packets.
10. Classes for multiplaform support (Critical section,mutex,sempahore,events,threads)

There are two helper classes CIPOptions, CTCPOptions which control the options for both protocols
(Currently IP options are supported for raw operations only)

If you feel you need to have more control over the IP header, you can :

A. Change the code.
B. Inherit from its classes and override ConstructIPHeader.

There are two basic ways to use the library :

A. Raw sockets
--------------

Allocate the desired class
Set it's properties to raw
Start messing around

B. Regular messaging
--------------------

Allocate the desired class
Set the application instance (Instance is needed only for win9x apps)
Create it 
Start working

* When working with the library not that the classes CTCPSocket,CUDPSocket and CICMPSocket are
blocking sockets, and CTCPSocketAsync,CUDPSocketAsync and CICMPSocketasync are async sockets.
(Here my frameworks creates an handler for all the async requests, each time the socket
 status changes, it calls the appropriate OnSocketEvent i.e. for receive OnSocketReceive.
 In my samples I inherited from the classes (you have to because they are abstract virtual))

From V3.1, the handlers can be multithreaded (allocate an handler in each thread, this will allow faster proccessing of sockets).
From V3.3, All methods that expect text IP (xXX.XXX.XXX.XXX) are overloaded to receive the unsigned long of the IP.

5. Library description.
-----------------------

CSpoofBase
----------


Functions:

static BOOL IsInitialized()
---------------------------

Is the sockets initialized.

Return value - True if initialized.


void SetLocalLog(CSpoofLog *pLog)
---------------------------------

Sets the local log.

pLog - The log to use.


static char FAR * LongToString(long lAddr)
------------------------------------------

Converts an IP address stored as long to string.

lAddr - The address to convert.

return value - The string address.


void SetLog(CSpoofLog* pLog)
----------------------------

Sets the socket's log for the entire session (until shutdown is called)

pLog - Your log class.


static BOOL InitializeSockets(BOOL bMultiThreaded=FALSE,int iNumberOfThreads=0)
-------------------------------------------------------------------------------

Must call on the beginning of your program.
Initialize the socket engine.

bMultithreaded - Create multiple number of threads. (FALSE means sockets runs in the context of the main thread, however it's safe to access from multiple threads)
iNumberOfThreads - Number of threads, relevant only if multithreaded.

return value - Non zero for success.


static BOOL ShutdownSockets()
-----------------------------

Clears the system resources, socket's and this library async mechanism.
(If there are allocated async sockets, they will be deleted)

return value - True for success.


int GetLastError() const
------------------

return value - The last error that occured, zero if no error.


Internal functions:

static int GetNumberOfThreads()
-------------------------------

Return value - Number of threads initialized.


static BOOL IsMultiThreaded()
-----------------------------

Return value - True when initialized as multithreaded.


void ReportError(LPCSTR lpMethod) const
---------------------------------

Reports an unknown error.

lpMethod- The name of the calling method.

static void ReportStaticError(LPCSTR lpClass,LPCSTR lpMethod)
-------------------------------------------------------------

Report an error from a static member.

lpClass - The calling class name.
lpMethod - The calling method name.


static void ReportStaticError(LPCSTR lpClass,LPCSTR lpMethod,LPCSTR lpMessage)
------------------------------------------------------------------------------

Report an error from a static member.

lpClass - The calling class name.
lpMethod - The calling method name.
lpMessage - The message to report.


virtual void NotifyShutdown()
-----------------------------

Method for the derived class to inherit, will be called when sockets is shuttin down.


void RegisterShutdown(CSpoofBase* pBase)
----------------------------------------

Registers a class to be notified when shutdown occurs. (1 class only per application)

pBase - The class to be notified.


void ReportError(LPCSTR lpMethod,LPCSTR lpMessage) const
--------------------------------------------------

Report an application error.

lpMethod - The name of the calling method.
lpMessage - The error message.


virtual void ReportError(LPSTR lpMethod,int iErrorCode) const
-------------------------------------------------------

Report a socket error.

lpMethod - The name of the calling method.
iErrorCode - The socket error code.


void SetName(LPCSTR lpName) const
---------------------------

Set the name of the active class (for error reporting)

lpName - The class name (string is copied and freed when dtor is called).


void SetLastError(LPCSTR lpMethod) const
----------------------------------

Get the socket last error.

lpMethod - The name of the calling method.


void SetLastError(LPCSTR lpMethod,int iErrorCode) const
-------------------------------------------------

Set a specific socket error.

lpMethod - The calling method name.
iErrorCode - The socket error code.



CSpoofLog* GetLog()
-------------------

Return value - The correct log (global or local).


private variables:

mutable int m_LastError - Last socket error that occured.

static CSpoofLog* m_Log - Our log reporting class.

CSpoofLog* m_LocalLog - Our local log.

static BOOL m_Initialized - Is the socket system initialized ?

mutable LPSTR m_lpClassName - Our class name (for error reporting).

static CSpoofBase* m_pShutdownClass - Class to be notified when shutdown occurs.

static BOOL m_bMultiThreaded - Are we multithreaded ?

static int m_NumberOfThreads - Number of threads to allocate.



CSpoofBase::CSpoofLog
---------------------

Internal functions:

virtual void ReportCatchError(LPCSTR lpClass,LPCSTR lpMethod,LPCSTR lpMessage)=0
-----------------------------------------------------------------------------

Reports an error captured from a catch phrase.

lpClass - The class where the error occured.
lpMethod - The method where the error occured.
lpMessage - The error message.


virtual void ReportInitiatedError(LPCSTR lpClass,LPCSTR lpMethod,LPCSTR lpMessage)=0
---------------------------------------------------------------------------------

Reports an error reported by the library (initiated, not catched)

lpClass - The class where the error occured.
lpMethod - The method where the error occured.
lpMessage - The error message.


virtual void ReportSocketError(LPCSTR lpClass,LPCSTR lpMethod,int iErrorCode)=0
-----------------------------------------------------------------------------

Reports a socket error.

lpClass - The class where the error occured.
lpMethod - The method where the error occured.
iErrorCode - The socket error code.



CSpoofSocket
------------

Functions:

BOOL SetBroadcast(BOOL bBroadcast)
----------------------------------

Sets the socket to broadcast mode.

bBroadcast - Switch broadcast on or off.

Return value - True for success.


BOOL CanWrite() const
---------------

Checks if the socket is ready for writing.

Return value - True when can write.


BOOL CanRead() const
--------------

Checks if the socket has data in it's buffer.

Return value - True when there's data in the buffer.


BOOL Shutdown(SocketShutdown eHow)
----------------------------------

Shutdown the socket - Disallow it to send/Receive or both (and not freeing its resources)

eHow - How to shut it down (send/receive or both)


unsigned short GetPeerPort() const
----------------------------
Return value - Port of the remote connected system.


long GetPeerAddress() const
---------------------

return value - Address of the remote address (Also ICMP and UDP)


virtual BOOL Sniff(BOOL bSniff)
-------------------------------

Set the socket to be a sniffer socket (recieve all packet on the binded interface - must bind)

Return value - True of success.


long ResolveDNS(LPCSTR lpAddress) const
---------------------------------

Resolve a name IP address (www.xxx.com) and return the address.

lpAddress - The address to resolve.

Return value - zero when resolving failed, the address otherwise.


BOOL ValidAddress(LPCSTR lpAddress) const
-----------------------------------

Check if an address of the type xxx.yyy.zzz.aaa is valid.

lpAddress - the address to check.

Return value - True when valid.


virtual int Receive(char* buf,int bufLen)
-----------------------------------------

Receives data from the socket buffer.

buf - Buffer to receive data to.
bufLen - Buffer length.

return value - Number of bytes read, SOCKET_ERROR for error.


CIPOptions* GetOptions() const
------------------------

Used to get the option structure the header contains.

return value - options structure.


void SetOptions(BOOL bOptions)
------------------------------

Allows options to be used within the socket.

bOptions - True to allow options.


void SetRaw(BOOL bRaw)
----------------------

Sets socket to raw or regular (need to be set before Create)

bRaw - True for raw.


void SetTTL(unsigned char ucTTL)
--------------------------------

Set time to live on the packet.
Could be used for tracing routes.


unsigned short CalculatePseudoChecksum(const char *buf, 
				       int BufLength,
				       LPCSTR lpDestinationAddress,
				       int iPacketLength)
---------------------------------------------------------

Calculate checksum for TCP and UDP which require calculating along a pseudo header.

buf - The packet to checksum.
BufLength - Size of the buf.
lpDestinationAddress - Destination address of the packet.
iPacketLength - Packet length.

return value - The checksum.


void SetSourceAddress(LPCSTR lpSourceAddress)
---------------------------------------------

Set the source address of the socket.
Calling bind before, will set the source address to the bind,
but calling bind after won't change the value seted by the user.

(this can be used to IP spoofing, setting the address to a diffrent one)


virtual BOOL Close()
--------------------

Close the socket and frees the resources.

return value - True for success.


virtual BOOL Bind(LPCSTR lpSourceAddress,int iPort=0)
-----------------------------------------------------

Binds the socket to a specific network (i.e. nic).

lpSourceAddress - The address to bind to. (Automaticly set source address as the bind address)
iPort - The port to bind to.

return value - True for success.


virtual BOOL Send(LPCSTR lpDestinationAddress,char* buf,int bufLength,unsigned short usDestinationPort=0)
--------------------------------------------------------------------------------------------

Send data using IP protocol.

lpDestinationAddress - The destination address.
buf - The buffer to send (can be other higher level protocols like tcp and udp)
bufLength - Length of the buffer.
usDestinationPort - Destination port. (TCP and UDP only)

return value - True on success.

BOOL Create(int iProtocol)
--------------------------

iProtocol - The protocol to use. (defined in the header)

return value - Non zero for success.


Internal functions:

void AssignSocket(SOCKET sok,unsigned char ucProtocol=IPPROTO_TCP)
-----------------------------

Bind to a given socket.

sok - The socket.
ucProtocol - The protocol of the new socket.


BOOL isRaw() const
------------

Check if this is a raw socket.

Return value - True on raw socket.


void SetProtocol(int iProtocol)
-------------------------------

Adjust the protocol before socket creation.

iProtocol - Protocol number (IPPROTO_IP ...)


BOOL CheckSocketValid() const
-----------------------

Checks if the socket is valid.

Return value - True when socket is valid.


unsigned short CalculateChecksum(const unsigned short* usBuf,int iSize) const
-----------------------------------------------------------------------------

Calculate the IP header checksum for a given buffer.

usBuf - The buffer to calculate the checksum to.
iSize - Size of the given buffer.

Return value - The calculated checksum.


BOOL ValidSocket() const
------------------

Checks if the socket is valid.

Return value - True when socket is valid.


SOCKET getHandle() const
------------------

Return value - The socket handle.


virtual void InitializeIP()
---------------------------

Initialize private class members.


virtual void SetIPHeaderAddress(LPIpHeader lpHead,LPCSTR lpSourceAddress,LPCSTR lpDestinationAddress)
-----------------------------------------------------------------------------------------------------

Set the data within the given IP header.

lpHead - The header to change.
lpSourceAddress - The new source address for the header.
lpDestinationAddress - The new destination address for the header.


virtual LPIpHeader ConstructIPHeader (unsigned char  ucProtocol,
				      unsigned short usFragmentationFlags,
				      unsigned char  ucTTL,
			              unsigned short usIdentification,
       				      unsigned char  ucHeaderLength)
--------------------------------------------------------------------

Create the IP header for the send.

ucProtocol - The protocol we are sending.
usFragmentationFlags - Fragmentation flags (Fragment/Never fragment).
ucTTL - Packet time to live.
usIdentification - How to ID our packet (needed for fragmentation).
ucHeaderLength - The length of the header including IP options.

Return value - The new constructed IP header.

virtual void FinalIPHeader(LPIpHeader lpHead)
---------------------------------------------

Notify that this is the last chance to modify the header before sending it.

lpHead - The header.


sockaddr_in pResolveDNS(LPCSTR lpAddress) const
-----------------------------------------

Resolve the DNS address.

lpAddress - the remote address to resolve.

Return value - The resolved address.


private variables:

sockaddr_in m_ConnectedTo - Where the socket is connected to.

CIPOptions* m_IPOptions - Our IP options.

BOOL m_Options - Do we even have options.

BOOL m_Raw - Is the socket raw.

unsigned char m_TTL - Our time to live.

unsigned char m_Protocol - Our protocol.

LPCSTR m_SourceAddress - Our source address.

SOCKET m_SpoofSocket - The socket handle.



CIPOptions
----------

Functions:

void AddOption_Option
---------------------

Add that option to the options list (for a complete detail of each option, go to the IP RFC).


void AddOption_ENDLIST()
------------------------

Indicates end of options.


void Reset()
------------

Delete all options.


void SetAutoPad(BOOL bAutoPAD)
------------------------------

Set the pading option (If to put zeros so the option will align to 4 bytes).

bAutoPad - True to pad.



CTCPSocket
----------

Functions:
	
BOOL Send(char* buf,int bufLen)
-------------------------------

Send data. (works only when non raw)

buf - The buffer to send.
bufLen - The size of the buffer.

Return value - True on success.


BOOL Accept(CTCPSocket* tSok)
-----------------------------

Accepts a connection to a pre exists socket. (calls the method accepted on the accepted socket)

tSok - The user socket.

return value - True for success


CTCPSocket* Accept()
--------------------

Constructs a new socket and accept the connection into it.

return value - NULL for error, if not then the new socket address.


virtual BOOL Listen(int iBackLog)
---------------------------------

Listen to incoming data.

iBackLog - How many connection to accept into quene.

return value - True for success.


virtual BOOL CreateRegular()
----------------------------

Create as a regular non raw socket.

Return value - True for success.


CTCPOptions* GetTCPOptions() const
----------------------------

return value - The TCP option structure.


virtual BOOL Connect(int iSourcePort,LPCSTR lpDestinationAddress,int iDestinationPort)
--------------------------------------------------------------------------------------

When in raw mode the initialize SYN packet will be sent only.
When in non raw mode (created via CreateRegular, will connect using the OS functions)

iSourcePort - Source port to connect from (zero for any)
lpDestinationAddress - Destination address.
iDestinationPort - Destination port to connect to (zero for any)

return value - True for success.


virtual BOOL Create()
---------------------

Creates a raw socket.

Return value - True on success.


void SetTCPOptions(BOOL bOptions)
---------------------------------

Allows the usage of TCP options.

bOptions - True to enable options.
	

Internal functions:

void InitializeTCP()
--------------------

Initializes the TCP data.


void SetHeaderFlag(LPTCPHeader lpHead,int iFlag)
------------------------------------------------

Sets data in the TCP header.

lpHead - The TCP header to modify.
iFlag - The flag value (SYN,ACK,RST,FIN,URG,PSH)


LPTCPHeader ConstructTCPHeader(int iSourcePort,int iDestinationPort,int iHeaderLength)
--------------------------------------------------------------------------------------

Creates a TCP header.

iSourcePort - The source port of the packet.
iDestinationPort - The destination port of the packet.
iHeaderLength - The packet header length. (including options)


virtual void Accepted()
-----------------------

Will be called when socket is accepted.


virtual void FinalTCPHeader(LPTCPHeader lpHead)
-----------------------------------------------

Called before the TCP packet is sent.

lpHead - The packet header.


virtual BOOL SendRaw(unsigned short usSourcePort, 
		     LPCSTR lpDestinationAddress, 
		     unsigned short usDestinationPort,
		     char* buf,
		     int BufLength,
		     unsigned char ucFlags=0)
---------------------------------------------

Send a raw TCP packet.

usSourcePort - The source port.
lpDestinationAddress - The destination address.
usDestinationPort - The destination port.
buf - The buffer to send.
BufLength - The length of the buffer.
ucFlags - The packet flags.


Internal members:

CTCPOptions* m_TCPOptions - The TCP options.

BOOL m_Options - Do we have TCP options.

static unsigned int m_Sequence - TCP sequence number.



CTCPOptions
-----------

Same as IP options.



CUDPSocket
----------

Functions:

BOOL CreateRegular()
--------------------

Creates a regular socket. (Same as Create)

Return value - True on success.


BOOL SetBroadcast(BOOL bBroadcast)
----------------------------------

Allows the socket to broadcast.

return value - Non zero for success.


BOOL Send(int iSourcePort, 
	  LPCSTR lpDestinationAddress, 
	  int iDestinationPort,
	  const char* buf,
	  int BufLength)
------------------------

iSourcePort - Source port to connect from (zero for any)
lpDestinationAddress - Destination address.
iDestinationPort - Destination port to connect to (zero for any)
buf - Buffer to send over.
BufLength - Buffer length.

return value - Non zero for success.


BOOL Create()
-------------

Creates the socket.

return value - True for success.


CICMPSocket
-----------

Functions:

SendType
--------

Sends an ICMP message of that type (for a complete description go to the ICMP RFC)

virtual BOOL ProccessICMP(char* buf)
------------------------------------

this is an internal function to proccess the incoming ICMP data (separates the IP header)


unsigned long GetLastDataSize() const
-------------------------------

return value - Data size of the last incoming packet.


const LPIpHeader GetLastIPHeader() const
----------------------------------

return value - The last proccess IP header.


const LPICMPHeader GetLastICMPHeader() const
--------------------------------------

return value - The last proccessed ICMP header.


Internal functions:

void ReverseHeader()
--------------------

Convert the received header so we can work with it (htons/htonl)


BOOL Send(LPCSTR lpDestinationAddress,unsigned char cICMP,unsigned char cType)
------------------------------------------------------------------------------

Sends the ICMP packet.

lpDestinationAddress - The address to send the packet to.
cICMP - The ICMP type.
cType - The ICMP code.

Return value - True on success.


virtual LPICMPHeader ConstructICMP() const
------------------------------------


Constructs a generic ICMP header.

Return value - The contructed ICMP header.


virtual BOOL ProccessICMP(char* buf)
------------------------------------

Proccess an incoming ICMP packet, and save the received IP and ICMP data.

buf - The buffer to proccess.

Return value - True on success.


Internal members:

char* m_Data - The data received.

LPIpHeader m_ICMPIPHeader - The last received copy of the ICMP header.

unsigned short m_DataSize - The received data size.

LPIpHeader m_IPHeader - The received IP header.

LPICMPHeader m_ICMPHeader - The ICMP header.



CInterfaces
-----------

Get available inetrfaces and more information about them.
The class contains a list, and it is iterated by using movenext.
(I'm assuming each computer has atleast one interface (loopback))

Functions:

BOOL IsMulticast() const
------------------

Return value - True when the interface supports multicast.


BOOL IsPPP() const
------------

Return value - True when the interface is a PPP connection.


BOOL IsLoopback() const
-----------------

Return value - True when the interface is a loopback interface.


BOOL IsBroadcast() const
------------------

Return value - True when the interface supports broadcast.


BOOL IsRunning() const
----------------

Return value - True when the interface is up and running.


long GetBroadcast() const
-------------------

Return value - The broadcast address of the interface.


long GetMask() const
--------------

Return value - The interface netmask.


BOOL MoveNext()
---------------

Move to the next interface.

Return value - True when there is another interface in the list. (False indicates end of the list)


long GetAddress() const
-----------------

Return value - The inteface address.


BOOL GetInterfaces()
--------------------

Retrieve the list of all the interfaces.

Return value - True on success.


Internal functions:

long GetFlags() const
---------------

Return value - The interface flags.


Internal members:

INTERFACE_INFO* m_pInfo - The interface info.

int m_iMaxInterfaces - Number of retrieved interfaces.

int m_iStructures - How many structures we can load (defined by the ctor)

int m_iPosition - Our position in the iterator.



CAsyncSocket
------------

This class is the base class for all async sockets (no longer need to use SetBlocking)
All async sockets inherit from this class and it's root protocol class (CTCPSocket, etc.)

The class holds a list of all the async sockets using it (using STL's map).

This is an abstract class its derived classes must override some of them.
If you want to create a derived class but you don't want to receive all the messages.
You can use a set of predefined macros :

If you don't want an OOB indication, then instead of writing:

virtual BOOL OnSocketOOB(int iErrorCode) {return TRUE;}

You can write:

NO_OnSocketOOB

Which is basicly the same (just saves some time)

Functions:

void FreezeThread()
-------------------

Used to freeze the thread allocation.
The next socket will not receive the current thread.
I used it when I closed my socket (from the socket's thread) and 
called Sleep.
A connection was accepted and was allocated on the thread who sleeps, and hangs the program.
So I called freeze, and the thread wasn't allocated.


BOOL DisableAsync()
-------------------

Turn the socket back to blocking mode.

Return value - True if success.


static void Initialize()
------------------------

Initialize the async handlers.


void Shutdown()
---------------

Indicates a system shutdown used to delete the windows, handlers and the tree.


BOOL KillTimer()
----------------

Kill the timer (if set)

Return value - True on success.


BOOL SetTimeout(int iMs)
------------------------

Create a timer for a timeout (OnSocketTimeout will be called)

iMs - The timeout in milliseconds.

Return value - True on success.


void SetInstance(HINSTANCE hInst)
---------------------------------

Set the instance of our application.

hInst - The application instance. (on W2K and NT can be null)


Internal functions:

virtual BOOL OnSocketTimeout()=0
--------------------------------

Will be called when the timeout is reached.

Must return TRUE.


virtual BOOL OnSocketConnect(int iErrorCode)=0
virtual BOOL OnSocketAccept(int iErrorCode)=0
virtual BOOL OnSocketClose(int iErrorCode)=0
virtual BOOL OnSocketOOB(int iErrorCode)=0
virtual BOOL OnSocketWrite(int iErrorCode)=0
virtual BOOL OnSocketReceive(int iErrorCode)=0
----------------------------------------------

Will be called when the event they are named after occured.

iErrorCode - If there was an error during the proccess.

Must return TRUE


static void Initialize()
------------------------

If handlers are not initialized yet, it initializes them.
(If multithreaded is supported, then the multithreaded handler is generated)

int GetSocketID() const
-----------------

Return value - The current socket ID.


HWND GetWindowHandle() const
----------------------

Return value - The constructed winodw handle. (Notification sink)


virtual SOCKET GetAsyncHandle()const=0
--------------------------------------

Return value - The actual socket handle.


virtual BOOL SetAsync()=0
-------------------------

Called when a socket is switched to an async mode.

Return value - True on success.


void RemoveSocketFromList()
---------------------------

Remove the current socket from the socket list.


void AddSocketToList()
----------------------

Add the current socket to the list.


BOOL IsTimeout() const
----------------

Does the socket has an active timeout.

Return value - True if has a timeout.


BOOL RemoveHandlers()
---------------------

Delete the window and the handler.

Return value - True on success.


HINSTANCE GetInstance()
-----------------------

Return value - The current application instance.


BOOL SetHandlers()
------------------

Create the window and the handler. (Notification sink)

Return value - True on success.


BOOL RegisterWindow()
---------------------

Register our window's class.

Return value - True on success.


static CAsyncSocket* GetSocketByID(int iSockID)
-----------------------------------------------

Finds a socket in the list to send him a message.

iSockID - The ID of the socket.

Return value - The socket (if found, NULL if not)


static LRESULT CALLBACK SocketMessageHandler(HWND hwnd,
					     UINT uMsg,
					     WPARAM wParam,
					     LPARAM lParam)
-----------------------------------------------------------

The message handler, for single threaded software.


CSocketThreadManager* GetThreadManager() const
----------------------------------------

Return value - The active thread manager.


Internal members:

static SocketMap m_SocketMap - Our map of sockets.

static BOOL m_Window - Do we have a window ?

static HWND m_WindowHandle - The window's handle.

HINSTANCE m_Instance - Our application instance.

int m_SocketID - ID of our socket.

BOOL m_List - Is the socket in the socket's list.

class CAsyncSocket::CAsyncShutdown - Used to notify us of a shutdown. (Created not to break some C++ rules)

BOOL m_Timeout - Timeout indicator.

CTCPSocketAsync
---------------

This class is an sync TCP wrapper.
I intetionaly create it's events abstract virtual, so who ever want to use it must override it.

All functions within this class are the same as in CTCPSocket, they just change some under the hood
status.



CUDPSocketAsync
---------------

This class is an sync UDP wrapper.
I intetionaly create it's events abstract virtual, so who ever want to use it must override it.

All functions within this class are the same as in CUDPSocket, they just change some under the hood
status.


CICMPSocketAsync
----------------

This class is an sync ICMP wrapper.
I intetionaly create it's events abstract virtual, so who ever want to use it must override it.

All functions within this class are the same as in CICMPSocket, they just change some under the hood
status.



CSniffSocket
------------

Used to sniff data on a specific interface.

All functions within this class are the same as its base classes, they just change some under the hood
status.



CSocketThreadManager
--------------------

void DecreaseSocketCount(HWND hWindowHandle)
--------------------------------------------

Decreases the socket count of a specific thread.

hWindowHandle - The handle of the window to decrease the socket count.


HWND GetWindowHandle()
----------------------

Gets a window handle of the most available thread.

Return value - The window handle.


int GetIndexByHWND(HWND hHandle) const
--------------------------------

Gets a thread by it's window handle.

Return value - The thread index position.


int GetMostAvailableThread() const
----------------------------

Gets the index of the most available thread.

Return value - The index of the thread position.


static DWORD SocketThread(LPVOID lpParameter)
---------------------------------------------

The socket thread functions, creates the windows, and runs the message map.


void SpawnThreads()
-------------------

Create all the threads, and waits until all the windows have been created.


Internal members:

int m_iThreadCount - Number of threads.

ThreadData* m_pThreadData - Data of the threads (socket count, handles)

HINSTANCE m_hInstance - Our instance.

CRITICAL_SECTION m_pCSection - Our critical section.

6. The samples.
--------------

Samples:

1.Attacker - Simple MFC program that employs 3 known attacks, just to demonstrate the library.
2.kping - Sample ping program. (Console)
3.ktracert - Sample trace route. (Console)
4.TCPServer - Sample of a very basic TCP server. (Console)
5.UDPServer - Sample of a very basic UDP server. (Console)
6.UDPScanner - Scans UDP ports.
7.Stealh TCP scanner - Scan TCP ports without being detected.
8.ksniffer - Network packet sniffer.
9.KIPConfig - Display available interface data.
10.PacketCrafter - Construct a crafted packet.

each sample has a simple readme.txt in its directory.

7. What's next ?
----------------

Currently we are working to expand the library (to version 4), which we want to make multiplatform.
We are very proud of the fact that this library is widely use for security checking and is some commercial
applications.

8. Contact us.
--------------

Site : http://www.komodia.com
email : barak@komodia.com
