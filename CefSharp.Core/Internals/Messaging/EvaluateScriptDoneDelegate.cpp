// Copyright � 2010-2015 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#include "Stdafx.h"
#include "Messages.h"
#include "../Serialization/Primitives.h"
#include "../Serialization/V8Serialization.h"
#include "EvaluateScriptDoneDelegate.h"

using namespace CefSharp::Internals::Serialization;
    
namespace CefSharp
{
    namespace Internals
    {
        namespace Messaging
        {
            EvaluateScriptDoneDelegate::EvaluateScriptDoneDelegate(PendingTaskRepository<JavascriptResponse^>^ pendingTasks, Dictionary<int, IJavascriptCallbackFactory^>^ callbackFactories)
                :_pendingTasks(pendingTasks), _callbackFactories(callbackFactories)
            {

            }

            Task<JavascriptResponse^>^ EvaluateScriptDoneDelegate::EvaluateScriptAsync(CefRefPtr<CefBrowser> cefBrowser, int browserId, int64 frameId, String^ script, Nullable<TimeSpan> timeout)
            {
                //create a new taskcompletionsource
                auto idAndComplectionSource = _pendingTasks->CreatePendingTask(timeout);

                auto message = CefProcessMessage::Create(kEvaluateJavascriptRequest);
                auto argList = message->GetArgumentList();
                argList->SetInt(0, browserId);
                SetInt64(frameId, argList, 1);
                SetInt64(idAndComplectionSource.Key, argList, 2);
                argList->SetString(3, StringUtils::ToNative(script));

                cefBrowser->SendProcessMessage(CefProcessId::PID_RENDERER, message);

                return idAndComplectionSource.Value->Task;
            }

            bool EvaluateScriptDoneDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
            {
                auto handled = false;
                auto name = message->GetName();
                if (name == kEvaluateJavascriptResponse)
                {
                    auto argList = message->GetArgumentList();
                    auto success = argList->GetBool(0);
                    auto callbackId = GetInt64(argList, 1);

                    IJavascriptCallbackFactory^ callbackFactory;
                    _callbackFactories->TryGetValue(browser->GetIdentifier(), callbackFactory);

                    auto pendingTask = _pendingTasks->RemovePendingTask(callbackId);
                    if (pendingTask != nullptr)
                    {
                        auto response = gcnew JavascriptResponse();
                        response->Success = success;

                        if (success)
                        {
                            response->Result = DeserializeV8Object(argList, 2, callbackFactory);
                        }
                        else
                        {
                            response->Message = StringUtils::ToClr(argList->GetString(2));
                        }

                        pendingTask->SetResult(response);
                    }

                    handled = true;
                }

                return handled;
            }
        }
    }
}