#include "utils/json_response.h"
#include "Poco/JSON/Stringifier.h"
#include <sstream>

using namespace Poco::Net;

namespace utils {

void JsonResponse::sendJSON(HTTPServerResponse& response, int statusCode, 
                            Poco::JSON::Object::Ptr data) {
    response.setStatusAndReason(static_cast<HTTPResponse::HTTPStatus>(statusCode));
    response.setContentType("application/json");
    response.setChunkedTransferEncoding(true);
    
    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(data, oss);
    
    std::ostream& ostr = response.send();
    ostr << oss.str();
}

void JsonResponse::sendJSON(HTTPServerResponse& response, int statusCode,
                            Poco::JSON::Array::Ptr data) {
    response.setStatusAndReason(static_cast<HTTPResponse::HTTPStatus>(statusCode));
    response.setContentType("application/json");
    response.setChunkedTransferEncoding(true);
    
    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(data, oss);
    
    std::ostream& ostr = response.send();
    ostr << oss.str();
}

void JsonResponse::sendError(HTTPServerResponse& response, int statusCode,
                             const std::string& message) {
    Poco::JSON::Object::Ptr error = new Poco::JSON::Object();
    error->set("error", message);
    error->set("statusCode", statusCode);
    sendJSON(response, statusCode, error);
}

}