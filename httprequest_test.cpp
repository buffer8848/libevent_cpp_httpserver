// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//  httprequest_test.cpp
//  unit test for httprequest
//

#include <iostream>

#include "common/httpserver/httpserver.h"
#include "common/base/scoped_ptr.h"

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"

using namespace std;

namespace pushing {
namespace common {

class EVHTTPRequest {
 public:
  EVHTTPRequest() {
    req_ = evhttp_request_new(NULL, NULL);
  }
  ~EVHTTPRequest() {
    evhttp_request_free(req_);
  }
 public:
  struct evhttp_request* req_;
};

class HTTPRequestTest : public testing::Test {
 protected:
  virtual void SetUp() {
    request_.reset(new HTTPRequest(req_.req_));
  }

  EVHTTPRequest req_;
  scoped_ptr<HTTPRequest> request_;
}; 

TEST_F(HTTPRequestTest, GetUri) {
}

} //! namespace common
} //! namespace pushing
