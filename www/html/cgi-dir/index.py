#!/usr/bin/env python3

import cgi
from http import cookies

# Create an instance of SimpleCookie
cookie = cookies.SimpleCookie()

# Set a cookie value
cookie["test"] = "Hello, World!"
cookie["test"]["path"] = "/"

# Print the HTTP header
print(cookie)
print("Content-Type: text/html\n")

# Print a response to the web page
print("<html><body>")
print("<h1>Cookie Set!</h1>")
print("</body></html>")
