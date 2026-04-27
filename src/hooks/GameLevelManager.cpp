#include "../Globals.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/GameLevelManager.hpp>

using namespace geode::prelude;

// much thanks to ery and cvolton
class $modify(ChallengeGameLevelManager, GameLevelManager) {
public:
    void onProcessHttpRequestCompleted(CCHttpClient* client, CCHttpResponse* response) {
        if (Challenge::inCorrectLevel()) {
            auto dataVector { response->getResponseData() };

            std::string data(dataVector->data(), dataVector->size());
            // if level download fails due to the level being unfetchable, the response will be "-1"
            log::debug("data head(20): {}", data.substr(0, 20));
            // not sure if this works  yet
            Challenge::legitDownloadFailed = (data == "-1");
        }
        GameLevelManager::onProcessHttpRequestCompleted(client, response);
    }
};
