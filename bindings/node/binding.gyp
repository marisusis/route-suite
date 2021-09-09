{
  "targets": [
    {
      "target_name": "hello",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "APIClientWrap.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../../api"
      ],
#       'libraries': [
#           'RouteAPI.lib'
#       ],
#       'link_settings': {
#           'library_dirs': [
#               '../../cmake-build-debug-x64/api'
#           ]
#       },
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
