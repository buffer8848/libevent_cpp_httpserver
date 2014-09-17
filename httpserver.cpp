// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//         Zeng, Fucen (fucen.zeng@intel.com)

#include "common/httpserver/httpserver.h"

#include <vector>

#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/event2/buffer.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/boost/shared_ptr.hpp"

using namespace std;
using namespace boost;

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
struct HTTPServer::HTTPServerImpl {
  struct event_base *base_;
  struct evhttp *http_;
  vector<boost::shared_ptr<HTTPHandler> > handlers_;
};

//-----------------------------------------------------------------------------

HTTPServer::HTTPServer() : running_(false) {
  core_ = new HTTPServerImpl();
  CHECK(core_);
  core_->base_ = event_base_new();
  CHECK(core_->base_) << "Couldn't create event_base.";
  core_->http_ = evhttp_new(core_->base_);
  CHECK(core_->http_) << "Couldn't create evhttp.";
}

HTTPServer::~HTTPServer() {
  if (running_)
    Stop();
  if (core_ != NULL && core_->base_ != NULL)
    event_base_free(core_->base_);
  if (core_ != NULL && core_->http_ != NULL)
    evhttp_free(core_->http_);
  delete core_;
}

//-----------------------------------------------------------------------------

void HTTPServer::RegisterHandler(const string& path, HttpHandlerFunc func) {
  boost::shared_ptr<HTTPHandler> handler(new HTTPHandler(func));
  evhttp_set_cb(core_->http_, path.c_str(), HTTPHandler::main_handle,
                handler.get());
  core_->handlers_.push_back(handler);
  LOG(INFO) << "Register Handler on " << path << " succefully!";
}

void HTTPServer::RegisterHandler(const std::string& path, boost::function<void()> func) {
  
}

bool HTTPServer::Start() {
  CHECK(!running_) << "HTTP server is already running";
  evhttp_set_cb(core_->http_, "/info", HTTPHandler::info_handle, NULL);
  evhttp_set_cb(core_->http_, "/admin", HTTPHandler::admin_handle, this);

  int ret = evhttp_bind_socket(core_->http_, address_.c_str(), port_);
  if (ret != 0) {
    LOG(ERROR) << "Couldn't bind socket.";
    return false;
  }

  running_ = true;
  LOG(INFO) << "Http Server(" << address_ << ":" << port_ << ") started.";
  event_base_dispatch(core_->base_);

  return true;
}

void HTTPServer::Stop() {
  event_base_loopexit(core_->base_, 0);
  running_ = false;
  LOG(INFO) << "Http Server(" << address_ << ":" << port_ << ") stopped.";
}

}  //! namespace common
}  //! namespace pushing
