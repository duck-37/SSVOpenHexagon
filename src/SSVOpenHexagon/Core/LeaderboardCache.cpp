// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#include "SSVOpenHexagon/Core/LeaderboardCache.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>

namespace hg {

void LeaderboardCache::receivedScores(const std::string& levelValidator,
    const std::vector<Database::ProcessedScore>& scores)
{
    CachedScores& cs = _levelValidatorToScores[levelValidator];
    cs._scores = scores;
    cs._cacheTime = Clock::now();
}

void LeaderboardCache::receivedOwnScore(
    const std::string& levelValidator, const Database::ProcessedScore& score)
{
    CachedScores& cs = _levelValidatorToScores[levelValidator];
    cs._ownScore = score;
    cs._cacheTime = Clock::now();
}

void LeaderboardCache::requestedScores(const std::string& levelValidator)
{
    _levelValidatorToScores[levelValidator]._cacheTime = Clock::now();
}

[[nodiscard]] bool LeaderboardCache::shouldRequestScores(
    const std::string& levelValidator) const
{
    const auto it = _levelValidatorToScores.find(levelValidator);
    if(it == _levelValidatorToScores.end())
    {
        return true;
    }

    const CachedScores& cs = it->second;
    return (Clock::now() - cs._cacheTime) > std::chrono::seconds(6);
}

[[nodiscard]] const std::vector<Database::ProcessedScore>&
LeaderboardCache::getScores(const std::string& levelValidator) const
{
    const auto it = _levelValidatorToScores.find(levelValidator);
    if(it == _levelValidatorToScores.end())
    {
        return _emptyScores;
    }

    return it->second._scores;
}

[[nodiscard]] const Database::ProcessedScore* LeaderboardCache::getOwnScore(
    const std::string& levelValidator) const
{
    const auto it = _levelValidatorToScores.find(levelValidator);
    if(it == _levelValidatorToScores.end())
    {
        return nullptr;
    }

    const auto& os = it->second._ownScore;
    return os.has_value() ? &*os : nullptr;
}

} // namespace hg