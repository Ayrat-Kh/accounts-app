#include "jsonSerialize.hpp"

namespace std::chrono
{
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, app::shared::Datetime timePoint)
    {
        jv = app::utils::timePointToString(timePoint);
    }

    boost::json::result<app::shared::Datetime> tag_invoke(boost::json::try_value_to_tag<app::shared::Datetime> v, const boost::json::value &s)
    {
        if (!s.if_string())
        {
            return boost::json::result<app::shared::Datetime>();
        }

        return app::utils::convertToTimePoint(std::string(s.get_string()));
    }
}
