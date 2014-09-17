// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//         Zeng, Fucen (fucen.zeng@intel.com)

//
//  httpserver_test.cpp
//  unit test for httpserver
//
//  Created by buffer on 2014-08-07.
//  Copyright (c) Intel inc. All rights reserved.
//

#include <iostream>
#include <curl/curl.h>

#include "common/base/scoped_ptr.h"
#include "common/httpserver/httpserver.h"
#include "common/system/thread_pool.h"

#include "thirdparty/boost/thread/thread.hpp"
#include "thirdparty/boost/function.hpp"
#include "thirdparty/boost/bind.hpp"
#include "thirdparty/event2/buffer.h"
#include "thirdparty/event2/dns.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/event2/http_struct.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/glog/logging.h"

using namespace std;
using namespace boost;

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
static std::string TEST_SERVER_IP = "127.0.0.1";
static uint32_t TEST_SERVER_PORT = 39527;
ThreadPool threadpool;
void workhandler(HTTPRequest *req, HTTPResponse *resp,
                 boost::function<void(bool)> finish);

class HTTPServerTest : public testing::Test {
 public:
  static void nethandler(HTTPRequest *req, HTTPResponse *resp,
                  boost::function<void(bool)> finish) {
    threadpool.AddTask(workhandler, req, resp, finish);
  }
 protected:
  virtual void SetUp() {
    http_server_.reset(new HTTPServer);
  }

  scoped_ptr<HTTPServer> http_server_;
};

struct CallbackContext {
  struct event_base* base_;
  const std::string* path_;
};

void RequestCallback(struct evhttp_request *req, void *arg) {
  int code = evhttp_request_get_response_code(req);
  EXPECT_EQ(200, code);
  char buffer[256] = {0};
  int nread;
  std::string body = "";
  while ((nread = evbuffer_remove(evhttp_request_get_input_buffer(req),
                                  buffer, sizeof(buffer))) > 0) {
    body += buffer;
  }
  struct CallbackContext* cc = static_cast<struct CallbackContext*>(arg);
  //LOG(INFO) << "Get response on " << *(cc->path_);
  if (*(cc->path_) == "/my_path")
    EXPECT_EQ("It's my path.", body);
  else if (*(cc->path_) == "/his_path")
    EXPECT_EQ("It's his path.", body);
  else if (*(cc->path_) == "/her_path")
    EXPECT_EQ("It's her path.", body);
  else  // not registered path, processed by default handler.
    EXPECT_EQ("Server will be stoped.", body);

  EXPECT_EQ(0, event_base_loopbreak(cc->base_));
}

void HttpClientFunc(uint32_t count, const std::string& path) {
  // Test timeout
  while (count > 0) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    --count;
  }

  struct event_base* base = event_base_new();
  struct evdns_base* dnsbase = evdns_base_new(base, 1);

  struct evhttp_connection* evhttp_connection;
  struct evhttp_request* evhttp_request;
  evhttp_connection = evhttp_connection_base_new(base, dnsbase,
                                                 TEST_SERVER_IP.c_str(),
                                                 TEST_SERVER_PORT);
  // NO need to free, the ownership is hold by sender.
  struct CallbackContext cc;
  cc.base_ = base;
  cc.path_ = &path;
  evhttp_request = evhttp_request_new(RequestCallback, &cc);
  evhttp_add_header(evhttp_request->output_headers, "User-Agent", "My Device");
  char buf[30];
  snprintf(buf, 30, "%s:%d", TEST_SERVER_IP.c_str(), TEST_SERVER_PORT);
  evhttp_add_header(evhttp_request->output_headers, "Host", buf);
  evhttp_make_request(evhttp_connection, evhttp_request, EVHTTP_REQ_GET,
                      path.c_str());
  //LOG(INFO) << "Start request on " << path;
  event_base_dispatch(base);

  evhttp_connection_free(evhttp_connection);
  evdns_base_free(dnsbase, 0);
  event_base_free(base);
  
  // Break server's event loop and let server exit.
  /*CURL *curl = curl_easy_init();
  std::stringstream url;
  url << TEST_SERVER_IP;
  url << ":";
  url << TEST_SERVER_PORT;
  url << "/admin";
  LOG(INFO) << "Request for " << url.str().c_str();
  curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);*/   
}

///////////////////////////////////////////////////////////////////
// Case #1
///////////////////////////////////////////////////////////////////
void my_path_handler(HTTPRequest *req, HTTPResponse *resp,
                     boost::function<void(bool)> finish) {
  //LOG(INFO) << "Handle by my_path_handler!";
  const char* path = req->GetPath();
  EXPECT_STREQ("/my_path", path);
  resp->SetBody("It's my path.");
  finish(true);
}

void his_path_handler(HTTPRequest *req, HTTPResponse *resp,
                      boost::function<void(bool)> finish) {
  //LOG(INFO) << "Handle by his_path_handler!";
  const char* path = req->GetPath();
  EXPECT_STREQ("/his_path", path);
  resp->SetBody("It's his path.");
  finish(true);
}

void her_path_handler(HTTPRequest *req, HTTPResponse *resp,
                      boost::function<void(bool)> finish) {
  //LOG(INFO) << "Handle by her_path_handler!";
  const char* path = req->GetPath();
  EXPECT_STREQ("/her_path", path);
  resp->SetBody("It's her path.");
  finish(true);
}

TEST_F(HTTPServerTest, RegisterHandler) {
  EXPECT_FALSE(http_server_->running_);
  http_server_->SetAddress(TEST_SERVER_IP);
  http_server_->SetPort(TEST_SERVER_PORT);

  //boost::mutex mutex;
  LOG(INFO) << "Register handler!";
  http_server_->RegisterHandler("/my_path", &my_path_handler);
  http_server_->RegisterHandler("/his_path", &his_path_handler);
  http_server_->RegisterHandler("/her_path", &her_path_handler);

  boost::thread my_http_client(&HttpClientFunc, 2000, "/my_path");
  boost::thread his_http_client(&HttpClientFunc, 2000, "/his_path");
  boost::thread her_http_client(&HttpClientFunc, 2000, "/her_path");
  boost::thread stop_http_client(&HttpClientFunc, 6000, "/admin"); //server stop

  EXPECT_TRUE(http_server_->Start());
  my_http_client.join();
  his_http_client.join();
  her_http_client.join();
}

///////////////////////////////////////////////////////////////////
// Case #2
///////////////////////////////////////////////////////////////////
void LoopTest(uint32_t req_count, const std::string& path, uint32_t time) {
  for (int i = 0; i < req_count; ++i) {
    HttpClientFunc(time, path);
  }
  //LOG(INFO) << "now calling stop server";
  HttpClientFunc(time, "/admin");
}

TEST_F(HTTPServerTest, LoopRequest) {
  EXPECT_FALSE(http_server_->running_);
  http_server_->SetAddress(TEST_SERVER_IP);
  http_server_->SetPort(TEST_SERVER_PORT);

  http_server_->RegisterHandler("/my_path", &my_path_handler);

  boost::thread http_client(&LoopTest, 100, "/my_path", 10);

  EXPECT_TRUE(http_server_->Start());
  http_client.join();
}

///////////////////////////////////////////////////////////////////
// Case #3 : 1 net thread + N work thread
///////////////////////////////////////////////////////////////////
void workhandler(HTTPRequest *req, HTTPResponse *resp,
                 boost::function<void(bool)> finish) {
  const char* path = req->GetPath();
  EXPECT_STREQ("/my_path", path);
  resp->SetBody("It's my path.");
  finish(true);
}

TEST_F(HTTPServerTest, abtest) {
  EXPECT_FALSE(http_server_->running_);
  http_server_->SetAddress(TEST_SERVER_IP);
  http_server_->SetPort(TEST_SERVER_PORT);
  http_server_->RegisterHandler("/my_path", &nethandler);

  boost::thread http_client(&LoopTest, 7, "/my_path", 10);

  EXPECT_TRUE(http_server_->Start());
  http_client.join();
}

} //! namespace common
} //! namespace pushing
