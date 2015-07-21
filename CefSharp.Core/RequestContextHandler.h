
// Copyright � 2010-2014 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#include "Stdafx.h"
#include "Internals/StringUtils.h"
#include <include/cef_request_context_handler.h>

using namespace CefSharp::Internals;

namespace CefSharp
{
    private class RequestContextHandler : CefRequestContextHandler
    {
    private:
        CefRefPtr<CefCookieManager> _cookieManager;

    public:
        RequestContextHandler(CefRefPtr<CefCookieManager> cookieManager) : _cookieManager(cookieManager) {}
        ~RequestContextHandler(){};
        CefRefPtr<CefCookieManager> GetCookieManager() OVERRIDE{
            return _cookieManager;
        }

    private:
        // Include the default reference counting implementation.
        IMPLEMENT_REFCOUNTING(RequestContextHandler);
    };
}