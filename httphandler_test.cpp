// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//  httphandler_test.cpp
//  unit test for httphandler
//

#include <iostream>

#include "common/httpserver/httpserver.h"
#include "common/base/scoped_ptr.h"

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/event2/dns.h"

using namespace std;

//-----------------------------------------------------------------------------
namespace pushing {
namespace common {

void DoMyFunc(HTTPRequest* req, HTTPResponse* resp,
                        boost::function<void(bool)> finish) {
  //TODO(binfei): do nothing but
  LOG(INFO) << "Test for httphandler.";
}

class HTTPHandlerTest : public testing::Test {
 protected:
  virtual void SetUp() {
    handler_.reset(new HTTPHandler(&DoMyFunc));
    base_ = event_base_new();
    dnsbase_ = evdns_base_new(base_, 1);
    connection_ = evhttp_connection_base_new(base_, dnsbase_, "0.0.0.0", 8080);
    request_ = evhttp_request_new(NULL, NULL);
    evhttp_make_request(connection_, request_, EVHTTP_REQ_GET, "");
  }

  virtual void TearDown() {
    evhttp_connection_free(connection_);
    //evhttp_request_free(request_);
    evdns_base_free(dnsbase_, 0);
    event_base_free(base_);
  }

  scoped_ptr<HTTPHandler> handler_;
  struct event_base* base_;
  struct evdns_base* dnsbase_;;
  struct evhttp_connection *connection_;
  struct evhttp_request *request_;
};

TEST_F(HTTPHandlerTest, handle) {
  HTTPRequest *newreq = new HTTPRequest(request_);
  HTTPResponse *newresp = new HTTPResponse(request_);
  EXPECT_TRUE(handler_->handle(newreq, newresp));
  EXPECT_FALSE(handler_->handle(newreq, NULL));
  EXPECT_FALSE(handler_->handle(NULL, newresp));
  EXPECT_FALSE(handler_->handle(NULL, NULL));
  delete newreq;
  delete newresp;
}

TEST_F(HTTPHandlerTest, finish) {
  //HTTPRequest *newreq = new HTTPRequest(request_);
  //HTTPResponse *newresp = new HTTPResponse(request_);
  //EXPECT_TRUE(handler_->finish(false, newreq, newresp));
  //EXPECT_FALSE(handler_->finish(false, newreq, NULL));
  //EXPECT_FALSE(handler_->finish(false, NULL, newresp));
  //EXPECT_FALSE(handler_->finish(false, NULL, NULL));
  //newreq = new HTTPRequest(request_);
  //newresp = new HTTPResponse(request_);
  //EXPECT_TRUE(handler_->finish(true, newreq, newresp));
  //EXPECT_FALSE(handler_->finish(true, newreq, NULL));
  //EXPECT_FALSE(handler_->finish(true, NULL, newresp));
  //EXPECT_FALSE(handler_->finish(true, NULL, NULL));
  //delete newreq;
  //delete newresp;
}

TEST_F(HTTPHandlerTest, admin_handle) {
  //handler_->admin_handle(NULL, NULL);
  //handler_->admin_handle(NULL, request_);
  //handler_->admin_handle(request_, NULL);
  //handler_->admin_handle(request_, request_);
}

TEST_F(HTTPHandlerTest, info_handle) {
  //handler_->info_handle(NULL, NULL);
  //handler_->info_handle(NULL, request_);
  //handler_->info_handle(request_, NULL);
  //handler_->info_handle(request_, request_);
}

TEST_F(HTTPHandlerTest, main_handle) {
  //handler_->main_handle(NULL, NULL);
  //handler_->main_handle(NULL, request_);
  //handler_->main_handle(request_, NULL);
  //handler_->main_handle(request_, handler_.get());
}

} //! namespace common
} //! namespace pushing
