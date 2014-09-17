// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//         Zeng, Fucen (fucen.zeng@intel.com)

//
//  httpserver.h
//  c++ wrapper for libevent-http-server
//
//  Created by buffer on 2014-08-07.
//  Copyright (c) Intel inc. All rights reserved.
//

#ifndef COMMON_HTTPSERVER_HTTPSERVER_H_
#define COMMON_HTTPSERVER_HTTPSERVER_H_

#include <stdint.h>
#include <string>

#include "common/httpserver/httphandler.h"
#include "common/httpserver/httprequest.h"
#include "common/httpserver/httpresponse.h"

#include "thirdparty/gtest/gtest_prod.h"

//-----------------------------------------------------------------------------
namespace pushing {
namespace common {

class HTTPServer {
 public:
  HTTPServer();
  virtual ~HTTPServer();

  void SetAddress(const std::string& address) {
    address_ = address;
  }
  void SetPort(uint32_t port) {
    port_ = port;
  }

  // Register a http handler
  //   path, an URI path, e.g. '/info'
  //   func, a callback pointer on some path.
  //   arg, args to pass to func, e.g. a context pointer.
  //
  // TODO(fucen): support boost::function and c++0x functor object, lamda expr.
  void RegisterHandler(const std::string& path, HttpHandlerFunc func);
  void RegisterHandler(const std::string& path, boost::function<void()> func);
  
  bool Start();
  void Stop();

 private:
  struct HTTPServerImpl;
  HTTPServerImpl *core_;
  
  std::string address_;
  uint32_t port_;
  bool running_;

  FRIEND_TEST(HTTPServerTest, RegisterHandler);
  FRIEND_TEST(HTTPServerTest, LoopRequest);
  FRIEND_TEST(HTTPServerTest, abtest);
};

} //! namespace common
} //! namespace pushing

#endif //! COMMON_HTTPSERVER_HTTPSERVER_H_
