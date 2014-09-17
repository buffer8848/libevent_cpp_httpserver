/// Copyright (c) 2014 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)
//
//
//  HTTPResponse.cpp
//  wrapper for http response content
//
//

#include "common/httpserver/httpresponse.h"

#include "thirdparty/event2/buffer.h"
#include "thirdparty/event2/bufferevent.h"
#include "thirdparty/event2/event.h"
#include "thirdparty/event2/http.h"
#include "thirdparty/event2/http_struct.h"
#include "thirdparty/event2/src/http-internal.h"
#include "thirdparty/glog/logging.h"

namespace pushing {
namespace common {

//-----------------------------------------------------------------------------
HTTPResponse::HTTPResponse(struct evhttp_request *req) : req_(req) {
  buffer_ = evbuffer_new();
}

HTTPResponse::~HTTPResponse() {
  if(buffer_ != NULL)
    evbuffer_free(buffer_);
  buffer_ = NULL;
}

//-----------------------------------------------------------------------------
bool HTTPResponse::SetHeader(const char* key, const char* value) {
  evhttp_remove_header(evhttp_request_get_output_headers(req_), key);
  evhttp_add_header(evhttp_request_get_output_headers(req_), key, value);

  return true;
}

bool HTTPResponse::SetBody(const char* content) {
  if (buffer_ == NULL)
    return false;
  
  evbuffer_add_printf(buffer_, content);
  
  return true;
}

bool HTTPResponse::SetChunk(const char* content) {
  if (buffer_ == NULL)
    return false;
 
  evbuffer_add_printf(buffer_, content);
  evhttp_send_reply_chunk(req_, buffer_);

  return true;
}

} //! namespace common
} //! namespace pushing
