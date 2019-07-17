#include "VirtualKeyboard.h"
#include "../core/core.h"

namespace WPEFramework {
namespace VirtualKeyboard {

    class Controller {

    private:
        struct KeyData {
            keyactiontype Action;
            uint32_t Code;
        };
        typedef Core::IPCMessageType<0, KeyData, Core::IPC::Void> KeyMessage;
        typedef Core::IPCMessageType<1, Core::IPC::Void, Core::IPC::Text<20>> NameMessage;

        class KeyEventHandler : public Core::IIPCServer {
        private:
            KeyEventHandler() = delete;
            KeyEventHandler(const KeyEventHandler&) = delete;
            KeyEventHandler& operator=(const KeyEventHandler&) = delete;

        public:
            KeyEventHandler(FNKeyEvent callback)
                : _callback(callback)
            {
            }
            virtual ~KeyEventHandler()
            {
            }

        public:
            virtual void Procedure(Core::IPCChannel& source, Core::ProxyType<Core::IIPC>& data)
            {
                Core::ProxyType<KeyMessage> message(data);
                ASSERT(_callback != nullptr);
                _callback(message->Parameters().Action, message->Parameters().Code);
                source.ReportResponse(data);
            }

        private:
            FNKeyEvent _callback;
        };

        class NameEventHandler : public Core::IIPCServer {
        private:
            NameEventHandler() = delete;
            NameEventHandler(const NameEventHandler&) = delete;
            NameEventHandler& operator=(const NameEventHandler&) = delete;

        public:
            NameEventHandler(const string& name)
                : _name(name)
            {
            }
            virtual ~NameEventHandler()
            {
            }

        public:
            virtual void Procedure(Core::IPCChannel& source, Core::ProxyType<Core::IIPC>& data)
            {
                TRACE_L1("In NameEventHandler::Procedure -- %d", __LINE__);

                Core::ProxyType<NameMessage> message(data);
                message->Response() = _name;
                source.ReportResponse(data);
            }

        private:
            string _name;
        };

    private:
        Controller() = delete;
        Controller(const Controller&) = delete;
        Controller& operator=(const Controller&) = delete;

    public:
        Controller(const string& name, const Core::NodeId& source, FNKeyEvent callback)
            : _channel(source, 32)
        {
            _channel.CreateFactory<KeyMessage>(1);
            _channel.CreateFactory<NameMessage>(1);

            _channel.Register(KeyMessage::Id(), Core::ProxyType<Core::IIPCServer>(Core::ProxyType<KeyEventHandler>::Create(callback)));
            _channel.Register(NameMessage::Id(), Core::ProxyType<Core::IIPCServer>(Core::ProxyType<NameEventHandler>::Create(name)));

            _channel.Open(2000); // Try opening this channel for 2S
        }
        ~Controller()
        {
            _channel.Close(Core::infinite);

            _channel.Unregister(KeyMessage::Id());
            _channel.Unregister(NameMessage::Id());

            _channel.DestroyFactory<KeyMessage>();
            _channel.DestroyFactory<NameMessage>();
        }

    private:
        Core::IPCChannelClientType<Core::Void, false, true> _channel;
    };
}
}

#ifdef __cplusplus
extern "C" {
#endif

using namespace WPEFramework;

// Producer, Consumer, We produce the virtual keyboard, the receiver needs
// to destruct it once the done with the virtual keyboard.
// Use the Destruct, to destruct it.
void* ConstructKeyboard(const char listenerName[], const char connector[], FNKeyEvent callback)
{
    Core::NodeId remoteId(connector);

    return (new VirtualKeyboard::Controller(listenerName, remoteId, callback));
}

void DestructKeyboard(void* handle)
{
    delete reinterpret_cast<VirtualKeyboard::Controller*>(handle);
}

void* Construct(const char listenerName[], const char connector[], FNKeyEvent callback)
{
    return ConstructKeyboard(listenerName, connector, callback);
}

void Destruct(void* handle)
{
    DestructKeyboard(handle);
}


#ifdef __cplusplus
}
#endif
