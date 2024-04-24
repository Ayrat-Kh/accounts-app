#include "jsonSerialize.hpp"
#include "date.hpp"

using namespace ::accounts;

namespace std::chrono
{
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, Datetime timePoint)
    {
        jv = timePointToString(timePoint);
    }

    boost::json::result<::accounts::Datetime> tag_invoke(boost::json::try_value_to_tag<::accounts::Datetime> v, const boost::json::value &s)
    {
        if (!s.if_string())
        {
            return Datetime{};
        }

        return convertToTimePoint(std::string(s.get_string()));
    }
}
