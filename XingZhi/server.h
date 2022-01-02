#include <httpserver.hpp>
#include "backend.h"
using namespace httpserver;

const std::string get_Result(const std::string&);


const std::shared_ptr<http_response> not_found_route(const http_request& req)
{
	return std::shared_ptr<string_response>(new string_response("Not Found\n",404,"text/plain"));
}

const std::shared_ptr<http_response> not_allowed_method(const http_request& req)
{
	return std::shared_ptr<string_response>(new string_response("Not Allowed Method\n",405,"text/plain"));
}

class SigInterfaceResource:public http_resource
{
	public:
		static SigInterfaceResource& get_instance()
		{
			static SigInterfaceResource instance_;
			return instance_;
		}
		SigInterfaceResource(SigInterfaceResource&)=delete;
		void operator=(SigInterfaceResource&)=delete;
		~SigInterfaceResource()=default;
		const std::shared_ptr<http_response> render_POST(const http_request& req)
		{
				return std::shared_ptr<string_response>(
						new string_response(ProcessRequest(req.get_content()), 200 ,"text/json"));

			/* if(!req.get_header("Content-Type").compare("text/json")) */
			/* { */


			/* 	// parse json */
			/* 	// validate format */
			/* 	// transform into UDP pack */
			/* 	// add to (with condi-var) task map and send udp and wait for signal */
			/* 	// */


			/* 	return std::shared_ptr<string_response>(new string_response(get_Result(req.get_content()), 200 ,"text/json")); */
			/* }else{ */
			/* 	return std::shared_ptr<string_response>(new string_response("{code:\"-1\"}", 200 ,"text/json")); */
			/* } */

		}
	private:
		SigInterfaceResource()=default;
};


void server()
{
	webserver ws = create_webserver(8080)
		.max_connections(100)
		.connection_timeout(180)
		.not_found_resource(not_found_route)
		.method_not_allowed_resource(not_allowed_method)
		.start_method(http::http_utils::THREAD_PER_CONNECTION);
	
	ws.register_resource("/sig_interface", &SigInterfaceResource::get_instance());
	ws.start(true);
}
