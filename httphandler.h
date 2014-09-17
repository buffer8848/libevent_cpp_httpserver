// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//         Zeng, Fucen (fucen.zeng@intel.com)

//
//  httphandler.h
//  wrapper for http request/response callback
//
//  Created by buffer on 2014-08-08.
//  Copyright (c) Intel inc. All rights reserved.
//

#ifndef COMMON_HTTPSERVER_HTTPHANDLER_H_
#define COMMON_HTTPSERVER_HTTPHANDLER_H_

#include "thirdparty/boost/function.hpp"
#include "thirdparty/gtest/gtest_prod.h"

//-----------------------------------------------------------------------------
//pre-define
struct evhttp_request;

//-----------------------------------------------------------------------------

namespace pushing {
namespace common {

class HTTPRequest;
class HTTPResponse;
typedef void(*HttpHandlerFunc)(HTTPRequest* req, HTTPResponse* resp,
                               boost::function<void(bool)> finish);

class HTTPHandler {
 public:
  HTTPHandler(HttpHandlerFunc func);
  HTTPHandler(boost::function<void()> func);
  virtual ~HTTPHandler();
  bool handle(HTTPRequest *req, HTTPResponse* resp);
  bool finish(bool success, HTTPRequest *req, HTTPResponse* resp);

 private:
  friend class HTTPServer;
  static void admin_handle(struct evhttp_request *req, void *arg);
  static void info_handle(struct evhttp_request *req, void *arg);
  static void main_handle(struct evhttp_request *req, void *arg);

 private:
  HttpHandlerFunc func_;

  FRIEND_TEST(HTTPHandlerTest, handle);
  FRIEND_TEST(HTTPHandlerTest, finish);
  FRIEND_TEST(HTTPHandlerTest, admin_handle);
  FRIEND_TEST(HTTPHandlerTest, info_handle);
  FRIEND_TEST(HTTPHandlerTest, main_handle);
};

} //! namespace common
} //! namespace pushing

#endif //! COMMON_HTTPSERVER_HTTPHANDLER_H_
