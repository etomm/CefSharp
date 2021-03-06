// Copyright � 2010-2014 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#pragma once

#include "Stdafx.h"
#include "Internals\MCefRefPtr.h"
#include "RequestContextSettings.h"
#include "include\cef_request_context.h"

using namespace CefSharp;

namespace CefSharp
{
    public ref class RequestContext
    {
    private:
        MCefRefPtr<CefRequestContext> _requestContext;
        RequestContextSettings^ _settings;
    public:
        RequestContext()
        {
            CefRequestContextSettings settings;
            _requestContext = CefRequestContext::CreateContext(settings, NULL);
        }

        RequestContext(RequestContextSettings^ settings) : _settings(settings)
        {
            _requestContext = CefRequestContext::CreateContext(settings, NULL);
        }

        !RequestContext()
        {
            _requestContext = NULL;
        }

        ~RequestContext()
        {
            this->!RequestContext();

            delete _settings;
        }

        operator CefRefPtr<CefRequestContext>()
        {
            if(this == nullptr)
            {
                return NULL;
            }
            return _requestContext.get();
        }
    };
}