/* Copyright (c) 2014, Fengping Bao <jamol@live.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __KUMAAPI_H__
#define __KUMAAPI_H__

#include "kmdefs.h"
#include "evdefs.h"

#include <stdint.h>
#ifdef KUMA_OS_WIN
# include <Ws2tcpip.h>
#else
# include <sys/socket.h>
#endif

KUMA_NS_BEGIN

class EventLoopImpl;
class TcpSocketImpl;
class UdpSocketImpl;
class TcpServerSocketImpl;
class TimerImpl;

class KUMA_API EventLoop
{
public:
    EventLoop(PollType poll_type = POLL_TYPE_NONE);
    ~EventLoop();
    
public:
    bool init();
    int registerFd(SOCKET_FD fd, uint32_t events, IOCallback& cb);
    int registerFd(SOCKET_FD fd, uint32_t events, IOCallback&& cb);
    int updateFd(SOCKET_FD fd, uint32_t events);
    int unregisterFd(SOCKET_FD fd, bool close_fd);
    
    PollType getPollType();
    bool isPollLT(); // level trigger
    
public:
    bool isInEventLoopThread();
    int runInEventLoop(LoopCallback& cb);
    int runInEventLoop(LoopCallback&& cb);
    int runInEventLoopSync(LoopCallback& cb);
    void loopOnce(uint32_t max_wait_ms);
    void loop(uint32_t max_wait_ms = -1);
    void stop();
    EventLoopImpl* getPimpl();
    
private:
    EventLoopImpl*  pimpl_;
};

class KUMA_API TcpSocket
{
public:
    typedef std::function<void(int)> EventCallback;
    
    TcpSocket(EventLoop* loop);
    ~TcpSocket();
    
    int bind(const char* bind_host, uint16_t bind_port);
    int connect(const char* host, uint16_t port, EventCallback& cb, uint32_t flags = 0, uint32_t timeout = 0);
    int connect(const char* host, uint16_t port, EventCallback&& cb, uint32_t flags = 0, uint32_t timeout = 0);
    int attachFd(SOCKET_FD fd, uint32_t flags = 0);
    int detachFd(SOCKET_FD &fd);
    int startSslHandshake(bool is_server);
    int send(uint8_t* data, uint32_t length);
    int send(iovec* iovs, uint32_t count);
    int receive(uint8_t* data, uint32_t length);
    int close();
    
    void setReadCallback(EventCallback& cb);
    void setWriteCallback(EventCallback& cb);
    void setErrorCallback(EventCallback& cb);
    void setReadCallback(EventCallback&& cb);
    void setWriteCallback(EventCallback&& cb);
    void setErrorCallback(EventCallback&& cb);
    
    SOCKET_FD getFd();
    TcpSocketImpl* getPimpl();
    
private:
    TcpSocketImpl* pimpl_;
};

class KUMA_API TcpServerSocket
{
public:
    typedef std::function<void(SOCKET_FD)> AcceptCallback;
    typedef std::function<void(int)> ErrorCallback;
    
    TcpServerSocket(EventLoop* loop);
    ~TcpServerSocket();
    
    int startListen(const char* host, uint16_t port);
    int stopListen(const char* host, uint16_t port);
    int close();
    
    void setAcceptCallback(AcceptCallback& cb);
    void setErrorCallback(ErrorCallback& cb);
    void setAcceptCallback(AcceptCallback&& cb);
    void setErrorCallback(ErrorCallback&& cb);
    TcpServerSocketImpl* getPimpl();
    
private:
    TcpServerSocketImpl* pimpl_;
};

class KUMA_API UdpSocket
{
public:
    typedef std::function<void(int)> EventCallback;
    
    UdpSocket(EventLoop* loop);
    ~UdpSocket();
    
    int bind(const char* bind_host, uint16_t bind_port, uint32_t flags = 0);
    int send(uint8_t* data, uint32_t length, const char* host, uint16_t port);
    int send(iovec* iovs, uint32_t count, const char* host, uint16_t port);
    int receive(uint8_t* data, uint32_t length, char* ip, uint32_t ip_len, uint16_t& port);
    int close();
    
    int mcastJoin(const char* mcast_addr, uint16_t mcast_port);
    int mcastLeave(const char* mcast_addr, uint16_t mcast_port);
    
    void setReadCallback(EventCallback& cb);
    void setErrorCallback(EventCallback& cb);
    void setReadCallback(EventCallback&& cb);
    void setErrorCallback(EventCallback&& cb);
    UdpSocketImpl* getPimpl();
    
private:
    UdpSocketImpl* pimpl_;
};

class KUMA_API Timer
{
public:
    Timer(EventLoop* loop);
    ~Timer();
    
    bool schedule(unsigned int time_elapse, TimerCallback& cb, bool repeat = false);
    bool schedule(unsigned int time_elapse, TimerCallback&& cb, bool repeat = false);
    void cancel();
    TimerImpl* getPimpl();
    
private:
    TimerImpl* pimpl_;
};

KUMA_NS_END

#endif