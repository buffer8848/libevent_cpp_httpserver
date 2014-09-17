// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//
//  HTTPResponse.h
//  wrapper for http response content
//
//

#ifndef COMMON_HTTPSERVER_HTTPREPONSE_H_
#define COMMON_HTTPSERVER_HTTPREPONSE_H_

#include <stdint.h>

#include "common/httpserver/httphandler.h"

#include "thirdparty/gtest/gtest_prod.h"

//-----------------------------------------------------------------------------
//pre-define
struct evbuffer;
struct evhttp_request;
struct evhttp_connection;

namespace pushing {
namespace common {
//-----------------------------------------------------------------------------
class HTTPResponse
{
 public:
  HTTPResponse(struct evhttp_request *req);
  virtual ~HTTPResponse();
  
  //TODO(binfei): these api are not threadsafe
  bool SetHeader(const char* key, const char* value);
  bool SetBody(const char* content);
  bool SetChunk(const char* content);

 private:
  friend class HTTPHandler;
  struct evhttp_request* req_;
  struct evbuffer *buffer_; //TODO(binfei): do we really need a external buffer
  
  FRIEND_TEST(HTTPRequestTest, SetHeader);
  FRIEND_TEST(HTTPRequestTest, SetBody);
  FRIEND_TEST(HTTPRequestTest, SetChunk);
};

} //! namespace common
} //! namespace pushing

#endif  //! COMMON_HTTPSERVER_HTTPREPONSE_H_
