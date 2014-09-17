// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//
//  httprequest.cpp
//  wrapper for http request content
//
//

#include "common/httpserver/httprequest.h"

#include "thirdparty/event2/buffer.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/glog/logging.h"

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
HTTPRequest::HTTPRequest(evhttp_request *req)
  : req_(req) {
}

HTTPRequest::~HTTPRequest() {
}

//-----------------------------------------------------------------------------
//TODO(binfei) : inline
const char* HTTPRequest::GetUri() {
  return evhttp_request_get_uri(req_);
}

const char* HTTPRequest::GetPath() {
  return evhttp_uri_get_path(evhttp_request_get_evhttp_uri(req_));
}

const char* HTTPRequest::GetHost() {
  return evhttp_uri_get_host(evhttp_request_get_evhttp_uri(req_));
}

uint32_t HTTPRequest::GetPort() {
  return evhttp_uri_get_port(evhttp_request_get_evhttp_uri(req_));
}

const char* HTTPRequest::FindHeaders(const char* key) {
  return evhttp_find_header(evhttp_request_get_input_headers(req_), key);
}

} //! namespace common
} //! namespace pushing
