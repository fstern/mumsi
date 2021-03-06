#include "MumbleCommunicator.hpp"

#include <cstring>
#include <functional>

namespace mumble {
    class MumlibCallback : public mumlib::BasicCallback {
    public:
        std::shared_ptr<mumlib::Mumlib> mum;
        MumbleCommunicator *communicator;

        virtual void audio(
                int target,
                int sessionId,
                int sequenceNumber,
                int16_t *pcm_data,
                uint32_t pcm_data_size) override {
            communicator->onIncomingPcmSamples(sessionId, sequenceNumber, pcm_data, pcm_data_size);
        }

        virtual void channelState(
                std::string name,
                int32_t channel_id,
                int32_t parent,
                std::string description,
                std::vector<uint32_t> links,
                std::vector<uint32_t> inks_add,
                std::vector<uint32_t> links_remove,
                bool temporary,
                int32_t position) override {
            communicator->onIncomingChannelState(name, channel_id);
        }

        virtual void serverSync(
                std::string welcome_text,
                int32_t session,
                int32_t max_bandwidth,
                int64_t permissions) override {
            communicator->onServerSync();
        };

    };
}

mumble::MumbleCommunicator::MumbleCommunicator(boost::asio::io_service &ioService)
        : ioService(ioService),
          logger(log4cpp::Category::getInstance("MumbleCommunicator")) {
}

void mumble::MumbleCommunicator::connect(
        std::string user,
        std::string password,
        std::string host,
        int port) {

    callback.reset(new MumlibCallback());

    mum.reset(new mumlib::Mumlib(*callback, ioService));
    callback->communicator = this;
    callback->mum = mum;

    mum->connect(host, port, user, password);
}

void mumble::MumbleCommunicator::sendPcmSamples(int16_t *samples, unsigned int length) {
    mum->sendAudioData(samples, length);
}

mumble::MumbleCommunicator::~MumbleCommunicator() {
    mum->disconnect();
}

void mumble::MumbleCommunicator::sendTextMessage(std::string message) {
    mum->sendTextMessage(message);
}

void mumble::MumbleCommunicator::joinChannel(int channel_id) {
    mum->joinChannel(channel_id);
}
