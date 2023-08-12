#!/usr/bin/env python

import cgi

form = cgi.FieldStorage()

name = form.getvalue("name")

print("Content-type: text/html\n\n")
print("<html>")
print("<head>")
print("<title>Python CGI Form Example</title>")
print("</head>")
print("<body>")
print("<h1>Hello, {}!</h1>".format(name))
print("<p>This is a simple Python CGI form example.</p>")
print("<form method='post' action='post.py'>")
print("<label for='name'>Enter your name:</label>")
print("<input type='text' id='name' name='name'>")
print("<input type='submit' value='Submit'>")
print("</form>")
print("</body>")
print("</html>")
