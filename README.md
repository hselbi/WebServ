# WebServ
This project is here to make you write your own HTTP server. You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won't be working on a website.

Team : [hselbi](https://github.com/hselbi) / [ZakariaMahmoud](https://github.com/ZakariaMahmoud) / [sbb3](https://github.com/sbb3)

[Subject](https://cdn.intra.42.fr/pdf/pdf/13265/en.subject.pdf)

---
[Usage](#usage)

[Introduction](#introduction)

[Parts of a web server](#parts-of-a-web-server)
  - [Server Kernel](#server-kernel)
  - [Request Parser](#request-parser)
  - [Response Builder](#response-builder)
  - [Configuration File](#configuration-file)
  - [CGI](#cgi)
  
 [Resources](#resources)


<br>

# Usage
```bash
make
./webserv [Config File](Optional)
```

# Introduction

HTTP, short for Hypertext Transfer Protocol, serves as the fundamental communication protocol for the internet. It facilitates the exchange of data between web browsers and web servers.

An HTTP web server operates as a software application responsible for receiving and responding to requests made by clients, typically web browsers. Its primary role is to host web content and provide access to users on the internet.

HTTP operates through a system of requests and responses. When a client, like a web browser, intends to fetch a webpage from a server, it dispatches an HTTP request to the server. Subsequently, the server processes this request and issues an HTTP response in return. This process enables the retrieval and display of web content.

## HTTP Message Format

HTTP messages adhere to a structured format consisting of a start line, headers, an optional message body, and delineated by carriage return and line feed characters (CRLF, \r\n).

This format serves for both HTTP requests and responses.

### HTTP Request

An HTTP request comprises a start line, headers, and, optionally, a message body. An illustration of an HTTP request is as follows:

```
GET /index.html HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
```

The request line embodies the method (e.g., GET), path or URI, and the HTTP version. Headers carry supplementary details like server hostname and the type of browser.

In the given example, no message body is included as the GET method typically does not incorporate one.

### HTTP Response

An HTTP response also features a start line, headers, and an optional message body. A sample HTTP response looks like this:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<Message Body>
```

The status line comprises the HTTP version, status code (e.g., 200 OK), and a reason phrase. Status codes provide insight into the result of the request, such as success, redirection, or errors. A brief summary of status codes is as follows:

    1xx: Informational message only
    2xx: Success
    3xx: Redirects the client to another URL
    4xx: Indicates an error on the client's part
    5xx: Indicates an error on the server's part

Headers in the response convey additional information, like content type and size. The message body contains the actual content, such as HTML code for a webpage.

## HTTP Methods

| Method | Description | Data in Request Body |
|--------|-------------|-----------------------|
| `GET`  | Retrieve a specific resource or a collection of resources without altering the data/resource. | No |
| `POST` | Initiate resource-specific processing based on the request content. | Yes |
| `DELETE` | Eliminate the resource identified by the URI. | Yes |
| `PUT` | Establish a new resource using data from the message body. If the resource already exists, update it with the body data. | Yes |
| `HEAD` | Similar to `GET`, but does not transfer the response content. | No |

These HTTP methods define the actions that clients can request from servers, specifying how the server should process the given resource identified by the URI.

### GET

HTTP GET is a method used to request data from a specified resource. It is primarily used for retrieving information and should not have any other effect on the data. The server responds to a GET request by returning the requested data in the response body. In case of success, it returns an HTTP response status code of 200 (OK). In error scenarios, it commonly results in a 404 (NOT FOUND) or 400 (BAD REQUEST) status code.

### POST

HTTP POST is a method used to submit data to be processed to a specified resource. It is often used to create new resources on the server. The data sent to the server is included in the request body. Upon successful creation, the server responds with an HTTP response code of 201 (Created). POST requests are not idempotent, meaning that the same request made multiple times may result in different outcomes.

### DELETE

HTTP DELETE is a method used to request the removal of a resource identified by a specific URI. The server, upon successful deletion, returns an HTTP response status code of 204 (No Content). DELETE requests are idempotent, meaning that making the same request multiple times has the same effect as making it once. It is a straightforward way to delete a resource on the server.

# Parts of a web server

A basic HTTP web server consists of several components that work together to receive and process HTTP requests from clients and send back responses. Below are the main parts of our webserver.

## Server Kernel

The networking component of a web server responsible for managing TCP connections. This module performs essential tasks like monitoring for incoming requests, dispatching responses, and overseeing fundamental networking operations. Its duties include socket creation and management, input and output stream handling, and the orchestration of data flow between the server and clients

Before diving into the development of your web server, I recommend exploring [this](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa) excellent guide on constructing a basic TCP client/server in C. This resource provides valuable insights into the workings of TCP in the C/C++ programming languages

Additionally, gaining proficiency in I/O multiplexing is crucial for effective server development. I suggest reading [this](https://notes.shichao.io/unp/ch6/) article, which offers a comprehensive overview of the main concepts behind select(). This knowledge will be instrumental in understanding and implementing efficient input/output handling in your server.

These resources will serve as foundational tools, providing you with the necessary understanding and skills to build a robust web server from scratch.

__Diagrame for multiplexing__

====

## Request Parser

In the context of a web server, the interpretative phase handles the task of understanding and extracting information from incoming HTTP requests. Within the structure of this web server, this interpretative process is executed by the Request class. This class, designed for parsing requests, accepts an incoming request, interprets its components, and extracts pertinent details such as the method, path, headers, and, if applicable, the message body.
In the event of a syntax error during parsing, the Request class sets error flags, bringing attention to the issue, and parsing ceases. The setBody() method allows requests to be introduced to the object either in full or in part. This capability is supported by the parser, which scans the request byte by byte and dynamically updates the parsing state as required. 
Notably, the parsing methodology mirrors that of Nginx and Node.js request parsers, ensuring compatibility with established industry standards in web server development

## Response Assembly

In this web server, the Response class handles the task of constructing HTTP responses sent to clients. It builds and stores responses, managing elements like the status line, headers, and message body. The class sets appropriate status codes, adds headers for content or server details, and formats the message body according to content type and encoding. For example, when a client requests a webpage, the server, via a Response object, fetches and constructs the HTTP response, including HTML content and relevant headers such as Content-Type and Content-Length. The Response class ensures well-structured, formatted responses aligning with client expectations.

## Configuration file

A configuration file is a text file housing diverse settings and directives that govern the operation of the web server. These settings encompass details like the port number for server listening, the root directory location, and various other operational parameters.
The configuration file typically includes a structured format showcasing supported directives. These directives define specific configurations and their values, providing a clear blueprint for the web server's behavior.

For a clearer understanding, an example file is often provided, illustrating the format of the configuration file and detailing the supported directives and their respective values. This file serves as a guide for users to customize and tailor the web server's settings to meet their specific requirements.
<br>

```nginx
server {
	server_name webserv2;
	listen 127.0.0.1:4141;                                                       # IP address or hostname (default is 127.0.0.1) and listening port, mandatory parameter
	root [SERVER_PATH]/www/html;                                                 # root folder of site directory, full or relative path, mandatory parameter
	allow_methods GET;                                                           # allowed methods in location, GET only by default
	autoindex on;                                                                # turn on/off directory listing
	index index.html index2.html index.php;                                      # default page when requesting a directory
	error_page 404 [SERVER_PATH]/www/html/custom_error_pages/404_server.html;    # default error page

	location / {
		allow_methods GET;
		root [SERVER_PATH]/www/html;                                               # root folder of the location, taken from the server. 
                                                                                  # EX: - URI /          --> /[SERVER_PATH]/www/html
                                                                                  #     - URI /index.html --> /[SERVER_PATH]/www/html/index.html
	}

	location /python-cgi
	{
		allow_methods GET POST;                                                   # allowed methods in location, GET and POST by default
		cgi_info py /usr/bin/python;                                              # location of interpreters installed on the current system
		root [SERVER_PATH]/www/html/python-cgi;                                   # cgi-bin location
		index index.py;                                                           # extensions for executable files
	}


}
```

## CGI

CGI scripting is a standard protocol employed by web servers to execute external programs. When a user requests a web page that requires processing by a CGI script, the web server runs the script and delivers the output to the user's web browser.

CGI scripts are essentially executable programs written in various programming languages like Perl, Python, or bash. They are commonly utilized to handle user-submitted data through web browsers or to generate dynamic content on web pages.

# Resources

### Networking
- [Create a simple HTTP server in c](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
- [(Video) Create a simple web server in c](https://www.youtube.com/watch?v=esXw4bdaZkc&ab_channel=JacobSorber)
- [(Video) explaining select()](https://www.youtube.com/watch?v=Y6pFtgRdUts&ab_channel=JacobSorber)
- [IBM - Nonblocking I/O and select()](https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_72/rzab6/xnonblock.htm)
- [All about sockets blocking](http://dwise1.net/pgm/sockets/blocking.html)
- [TCP Socket Programming: HTTP](https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/TCPSockets.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

### HTTP
- [MDN - HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [An Overview of the HTTP as Coverd in RFCs](https://www.inspirisys.com/HTTP_Protocol_as_covered_in_RFCs-An_Overview.pdf)
- [How the web works: HTTP and CGI explained](https://www.garshol.priv.no/download/text/http-tut.html)
- [MIME](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types)
- [HTTP Status Codes](https://umbraco.com/knowledge-base/http-status-codes/)

### RFC
- [How to Read an RFC](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)
- [RFC 9110 - HTTP Semantics ](https://www.rfc-editor.org/info/rfc9110)
- [RFC 9112 - HTTP/1.1 ](https://www.rfc-editor.org/info/rfc9112) 
- [RFC 2068 - ABNF](https://www.cs.columbia.edu/sip/syntax/rfc2068.html) 
- [RFC 3986 -  (URI) Generic Syntax](https://www.ietf.org/rfc/rfc3986)
- [RFC 6265 - HTTP State Management Mechanism (Cookies)](https://www.rfc-editor.org/rfc/rfc6265)
- [RFC 3875 - CGI](https://datatracker.ietf.org/doc/html/rfc3875)

### CGI
- [Python web Programming](https://www.tutorialspoint.com/python/python_cgi_programming.htm)
- [CPP web Programming](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)
- [(Video) Creating a file upload page](https://www.youtube.com/watch?v=_j5spdsJdV8&t=562s)

### Tools
- [Postman](https://www.postman.com/downloads/) : Send custom requests to the server
- [Sige](https://www.linode.com/docs/guides/load-testing-with-siege/) : Load testing 

### Other
- [URL Encoding](https://www.urlencoder.io/learn/#:~:text=A%20URL%20is%20composed%20from,%22%20%2C%20%22~%22%20)
- [Nginx](https://nginx.org/en/)
- [Nginx Source Code](https://github.com/nginx/nginx)
