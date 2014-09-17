// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//         Zeng, Fucen (fucen.zeng@intel.com)

#include "common/httpserver/httphandler.h"

#include "thirdparty/event2/buffer.h"
#include "thirdparty/event2/bufferevent.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/event2/http_struct.h"
#include "thirdparty/event2/src/http-internal.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/boost/bind.hpp"

#include "common/httpserver/httpserver.h"

static const char *connection_key = "Connection";
static const char *connection_value = "keep-alive";

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
HTTPHandler::HTTPHandler(HttpHandlerFunc func){
  func_ = func;
}

HTTPHandler::~HTTPHandler() {
}

//-----------------------------------------------------------------------------
void HTTPHandler::admin_handle(struct evhttp_request *req, void *arg) {
  //TODO(binfei): need ssl and sepecial command to do admin privilege
  LOG(INFO) << "Request for server admin.";
  // TODO(binfei): need memory pool
  struct evbuffer *databuf = evbuffer_new();
  evbuffer_add_printf(databuf, "Server will be stoped.");
  evhttp_send_reply_start(req, 200, "OK");
  evhttp_send_reply_chunk(req, databuf);
  evhttp_send_reply_end(req);
  evbuffer_free(databuf);
  
  HTTPServer *server = static_cast<HTTPServer*>(arg);
  server->Stop();
}

void HTTPHandler::info_handle(struct evhttp_request *req, void *arg) {
  if (evhttp_request_get_command(req) != EVHTTP_REQ_GET)
    return;

  LOG(INFO) << "Request for server infomation.";

  // TODO(binfei): need memory pool
  struct evbuffer *databuf = evbuffer_new();
  evbuffer_add_printf(databuf, "hello world");
  evhttp_send_reply_start(req, 200, "OK");
  evhttp_send_reply_chunk(req, databuf);
  evhttp_send_reply_end(req);
  evbuffer_free(databuf);
}

void HTTPHandler::main_handle(struct evhttp_request *req, void *arg) {
  // TODO(binfei): need pre-process
  HTTPHandler *handler = static_cast<HTTPHandler*>(arg);
  HTTPRequest *newreq = new HTTPRequest(req);
  HTTPResponse *newresp = new HTTPResponse(req);
  handler->handle(newreq, newresp);
}

//-----------------------------------------------------------------------------
bool HTTPHandler::handle(HTTPRequest *req, HTTPResponse *resp) {
  //call callback functions
  if (func_ == NULL || req == NULL || resp == NULL)
    return false;
  
  //prepare header, check if long or short connection
  const char* value = req->FindHeaders(connection_key);
  if (value != NULL && 
      strncmp(connection_value, value, strlen(connection_value)) == 0)
    evhttp_add_header(resp->req_->output_headers, connection_key, 
                      connection_value);
  evhttp_send_reply_start(resp->req_, HTTP_OK, "OK");
  
  (*func_)(req, resp, boost::bind(&HTTPHandler::finish, this, 
                                        _1, req, resp));
  return true;
}

bool HTTPHandler::finish(bool success, HTTPRequest *req, HTTPResponse *resp) {
  //TODO(binfei): libevent send reply
  if (func_ == NULL || req == NULL || resp == NULL)
    return false;

  evhttp_send_reply_chunk(resp->req_, resp->buffer_);
  evhttp_send_reply_end(resp->req_);

  delete req;
  delete resp;

  return true;
}

} //! namespace common
} //! namespace pushing
