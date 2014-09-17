cc_library(
  name = 'httpserver',
  srcs = [
    'httpserver.cpp',
	  'httphandler.cpp',
    'httprequest.cpp',
    'httpresponse.cpp',
  ],
  deps = [
	  '//thirdparty/event2:libevent',
	  '//thirdparty//glog:glog',
  ]
)

cc_test(
  name = 'httpserver_test',
  srcs = [
    'httpserver_test.cpp',
    'httphandler_test.cpp',
    'httprequest_test.cpp',
    'httpresponse_test.cpp',
  ],
  deps = [
    '#curl',
	  ':httpserver',
    '//common/system:thread_pool',
    '//thirdparty/boost:boost_thread',
    '//thirdparty/boost:boost_chrono',
    '//thirdparty/boost:boost_system',
  ],
)
