#pragma once

#include "IncomingConnectionValidator.hpp"
#include "AudioFramesMixer.hpp"

#include <pjmedia.h>
#include <pjsua-lib/pjsua.h>

#include <pjsua2.hpp>

#undef isblank

#include <log4cpp/Category.hh>
#include <boost/noncopyable.hpp>

#include <string>
#include <stdexcept>
#include <climits>
#include <bits/unique_ptr.h>

namespace sip {

    constexpr int DEFAULT_PORT = 5060;
    constexpr int SAMPLING_RATE = 48000;

    class Exception : public std::runtime_error {
    public:
        Exception(const char *title) : std::runtime_error(title) {
            mesg += title;
        }

        Exception(const char *title, pj_status_t status) : std::runtime_error(title) {
            char errorMsgBuffer[500];
            pj_strerror(status, errorMsgBuffer, sizeof(errorMsgBuffer));

            mesg += title;
            mesg += ": ";
            mesg += errorMsgBuffer;
        }

        virtual const char *what() const throw() override {
            return mesg.c_str();
        }

    private:
        std::string mesg;
    };

    class _LogWriter;

    class _Account;

    class _Call;

    class _MumlibAudioMedia;

    class PjsuaCommunicator : boost::noncopyable {
    public:
        PjsuaCommunicator(IncomingConnectionValidator &validator);

        void connect(
                std::string host,
                std::string user,
                std::string password,
                unsigned int port = DEFAULT_PORT);

        virtual ~PjsuaCommunicator();

        void sendPcmSamples(
                int sessionId,
                int sequenceNumber,
                int16_t *samples,
                unsigned int length);

        std::function<void(int16_t *, int)> onIncomingPcmSamples;

        std::function<void(std::string)> onStateChange;

        pj_status_t mediaPortGetFrame(pjmedia_port *port, pjmedia_frame *frame);

        pj_status_t mediaPortPutFrame(pjmedia_port *port, pjmedia_frame *frame);

    private:
        log4cpp::Category &logger;
        log4cpp::Category &pjsuaLogger;

        std::unique_ptr<mixer::AudioFramesMixer> mixer;

        std::unique_ptr<_LogWriter> logWriter;
        std::unique_ptr<_Account> account;
        std::unique_ptr<_MumlibAudioMedia> media;

        pj_caching_pool cachingPool;

        pj::Endpoint endpoint;

        IncomingConnectionValidator &uriValidator;

        void registerAccount(std::string host,
                             std::string user,
                             std::string password);

        friend class _Call;

        friend class _Account;

        friend class _MumlibAudioMedia;
    };

}
