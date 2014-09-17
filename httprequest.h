// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//
//  httprequest.h
//  wrapper for http request content
//
//

#ifndef COMMON_HTTPSERVER_HTTPREQUEST_H_
#define COMMON_HTTPSERVER_HTTPREQUEST_H_

#include <stdint.h>

#include "thirdparty/gtest/gtest_prod.h"

//-----------------------------------------------------------------------------
//pre-define
struct evhttp_request;

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
class HTTPRequest
{
 public:
  HTTPRequest(evhttp_request* req);
  virtual ~HTTPRequest();

  const char* GetUri();
  const char* GetPath();
  const char* GetHost();
  uint32_t GetPort();
  const char* FindHeaders(const char* key);

 private:
  evhttp_request* req_;

  FRIEND_TEST(HTTPRequestTest, GetUri);
  FRIEND_TEST(HTTPRequestTest, GetPath);
  FRIEND_TEST(HTTPRequestTest, GetHost);
  FRIEND_TEST(HTTPRequestTest, GetPort);
  FRIEND_TEST(HTTPRequestTest, FindHeaders);
};

} //! namespace common
} //! namespace pushing

#endif  //! COMMON_HTTPSERVER_HTTPREQUEST_H_
