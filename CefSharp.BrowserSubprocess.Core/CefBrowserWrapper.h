// Copyright � 2010-2014 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#pragma once

#include "include/cef_browser.h"
#include "include/cef_runnable.h"
#include "include/cef_v8.h"

#include "TypeUtils.h"
#include "Stdafx.h"

#include "CefTaskScheduler.h"

using namespace CefSharp::Internals;
using namespace System;
using namespace System::ServiceModel;
using namespace System::Threading;
using namespace System::Threading::Tasks;

namespace CefSharp
{
    // "Master class" for wrapping everything that the CefSubprocess needs.
    public ref class CefBrowserWrapper : DisposableResource
    {
    
    private:
        MCefRefPtr<CefBrowser> _cefBrowser;

    public:
        CefBrowserWrapper(CefRefPtr<CefBrowser> cefBrowser)
        {
            _cefBrowser = cefBrowser;
            BrowserId = cefBrowser->GetIdentifier();
            RenderThreadTaskFactory = gcnew TaskFactory(gcnew CefTaskScheduler(TID_RENDERER));
        }

        ~CefBrowserWrapper()
        {
            _cefBrowser = nullptr;
        }

        property int BrowserId;
        property TaskFactory^ RenderThreadTaskFactory;

        JavascriptResponse^ EvaluateScriptInContext(CefRefPtr<CefV8Context> context, CefString script)
        {
            CefRefPtr<CefV8Value> result;
            CefRefPtr<CefV8Exception> exception;
            JavascriptResponse^ response = gcnew JavascriptResponse();

            response->Success = context->Eval(script, result, exception);
            if (response->Success)
            {
                response->Result = TypeUtils::ConvertFromCef(result);
            }
            else if (exception.get())
            {
                response->Message = StringUtils::ToClr(exception->GetMessage());
            }

            return response;
        }

        virtual void DoDispose( bool disposing ) override
        {
            _cefBrowser = nullptr;
            RenderThreadTaskFactory = nullptr;
            CefBrowserWrapper::DoDispose( disposing );
        }

        virtual JavascriptResponse^ DoEvaluateScript(System::Int64 frameId, String^ script) override
        {
            auto frame = _cefBrowser->GetFrame(frameId);
            CefRefPtr<CefV8Context> context = frame->GetV8Context();

            if (context.get() && context->Enter())
            {
                try
                {
                    return EvaluateScriptInContext(context, StringUtils::ToNative(script));
                }
                finally
                {
                    context->Exit();
                }
            }

            return nullptr;
        }
    };
}