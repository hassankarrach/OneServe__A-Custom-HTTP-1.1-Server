#include "../../includes/types/resTypes.hpp"
#include <string>

std::string RESPONSE::toString(RESPONSE::ResponseCode responseCode)
{
    switch (responseCode)
    {
        case RESPONSE::OK:
            return "OK";
        case RESPONSE::Created:
            return "Created";
        case RESPONSE::NoContent:
            return "No Content";
        case RESPONSE::BadRequest:
            return "Bad Request";
        case RESPONSE::Unauthorized:
            return "Unauthorized";
        case RESPONSE::Forbidden:
            return "Forbidden";
        case RESPONSE::NotFound:
            return "Not Found";
        case RESPONSE::InternalServerError:
            return "Internal Server Error";
        case RESPONSE::NotImplemented:
            return "Not Implemented";
        case RESPONSE::ServiceUnavailable:
            return "Service Unavailable";
        default:
            return "Unknown";
    }
}


