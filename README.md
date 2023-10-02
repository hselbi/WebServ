# WebServ
This project is here to make you write your own HTTP server. You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won't be working on a website.

Team : [hselbi](https://github.com/hselbi) / [ZakariaMahmoud](https://github.com/ZakariaMahmoud) / [sbb3](https://github.com/sbb3)

[Subject](https://cdn.intra.42.fr/pdf/pdf/13265/en.subject.pdf)

---
[Usage](#usage)

[Introduction](#introduction)

[Parts of a web server](#parts-of-a-web-server)
  - [Server Core](#server-core)
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

## Request Parsing

In the context of a web server, the interpretative phase handles the task of understanding and extracting information from incoming HTTP requests. Within the structure of this web server, this interpretative process is executed by the Request class. This class, designed for parsing requests, accepts an incoming request, interprets its components, and extracts pertinent details such as the method, path, headers, and, if applicable, the message body.
In the event of a syntax error during parsing, the Request class sets error flags, bringing attention to the issue, and parsing ceases. The feed() method allows requests to be introduced to the object either in full or in part. This capability is supported by the parser, which scans the request byte by byte and dynamically updates the parsing state as required. 
Notably, the parsing methodology mirrors that of Nginx and Node.js request parsers, ensuring compatibility with established industry standards in web server development

# Resources

## Server setup

[HTTP Server: Everything you need to know to Build a simple HTTP server from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

[Building a simple server with C++](https://ncona.com/2019/04/building-a-simple-server-with-cpp/)

[C++ Web Programming](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)

### HTTP 1.1 (standard to follow) :

[Hands-On Netwok Programming with C](https://github.com/PacktPublishing/Hands-On-Network-Programming-with-C)

[HTTP/1.1 (RFC 2616)](https://www.rfc-editor.org/rfc/rfc2616.html)

[HTTP/1.1 : Message Syntax and Routing (RFC 7230)](https://www.rfc-editor.org/rfc/rfc7230.html)

[HTTP/1.1 : Semantics and Content (RFC 7231)](https://www.rfc-editor.org/rfc/rfc7231.html)

[HTTP/1.1 : Conditional Requests (RFC 7232)](https://www.rfc-editor.org/rfc/rfc7232.html)

[HTTP/1.1 : Range Requests (RFC 7233)](https://www.rfc-editor.org/rfc/rfc7233.html)

[HTTP/1.1 : Caching (RFC 7234)](https://www.rfc-editor.org/rfc/rfc7234.html)

[HTTP/1.1 : Authentication (RFC 7235)](https://www.rfc-editor.org/rfc/rfc7235.html)
